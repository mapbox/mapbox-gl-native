#include <mbgl/renderer/image_manager.hpp>

#include <mbgl/actor/actor.hpp>
#include <mbgl/actor/scheduler.hpp>
#include <mbgl/renderer/image_manager_observer.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/logging.hpp>

namespace mbgl {

static ImageManagerObserver nullObserver;

ImageManager::ImageManager() = default;

ImageManager::~ImageManager() = default;

void ImageManager::setObserver(ImageManagerObserver* observer_) {
    observer = observer_ ? observer_ : &nullObserver;
}

void ImageManager::setLoaded(bool loaded_) {
    if (loaded == loaded_) {
        return;
    }

    loaded = loaded_;

    if (loaded) {
        for (const auto& entry : requestors) {
            checkMissingAndNotify(*entry.first, entry.second);
        }
        requestors.clear();
    }
}

bool ImageManager::isLoaded() const {
    return loaded;
}

void ImageManager::addImage(Immutable<style::Image::Impl> image_) {
    assert(images.find(image_->id) == images.end());
    // Increase cache size if requested image was provided.
    if (requestedImages.find(image_->id) != requestedImages.end()) {
        requestedImagesCacheSize += image_->image.bytes();
    }
    images.emplace(image_->id, std::move(image_));
}

bool ImageManager::updateImage(Immutable<style::Image::Impl> image_) {
    auto oldImage = images.find(image_->id);
    assert(oldImage != images.end());
    if (oldImage == images.end()) return false;

    auto sizeChanged = oldImage->second->image.size != image_->image.size;

    if (sizeChanged) {
        // Update cache size if requested image size has changed.
        if (requestedImages.find(image_->id) != requestedImages.end()) {
            int64_t diff = image_->image.bytes() - oldImage->second->image.bytes();
            assert(static_cast<int64_t>(requestedImagesCacheSize + diff) >= 0ll);
            requestedImagesCacheSize += diff;
        }
        updatedImageVersions.erase(image_->id);
    } else {
        updatedImageVersions[image_->id]++;
    }

    oldImage->second = std::move(image_);

    return sizeChanged;
}

void ImageManager::removeImage(const std::string& id) {
    auto it = images.find(id);
    assert(it != images.end());
    // Reduce cache size for requested images.
    auto requestedIt = requestedImages.find(it->second->id);
    if (requestedIt != requestedImages.end()) {
        assert(requestedImagesCacheSize >= it->second->image.bytes());
        requestedImagesCacheSize -= it->second->image.bytes();
        requestedImages.erase(requestedIt);
    }
    images.erase(it);
    updatedImageVersions.erase(id);
}

const style::Image::Impl* ImageManager::getImage(const std::string& id) const {
    if (auto* image = getSharedImage(id)) {
        return image->get();
    }
    return nullptr;
}

const Immutable<style::Image::Impl>* ImageManager::getSharedImage(const std::string& id) const {
    const auto it = images.find(id);
    if (it != images.end()) {
        return &(it->second);
    }
    return nullptr;
}

void ImageManager::getImages(ImageRequestor& requestor, ImageRequestPair&& pair) {
    // remove previous requests from this tile
    removeRequestor(requestor);

    // If all the icon dependencies are already present ((i.e. if they've been addeded via
    // runtime styling), then notify the requestor immediately. Otherwise, if the
    // sprite has not loaded, then wait for it. When the sprite has loaded check
    // if all icons are available. If any are missing, call `onStyleImageMissing`
    // to give the user a chance to provide the icon. If they are not provided
    // by the next frame we'll assume they are permanently missing.
    if (!isLoaded()) {
        bool hasAllDependencies = true;
        for (const auto& dependency : pair.first) {
            if (images.find(dependency.first) == images.end()) {
                hasAllDependencies = false;
                break;
            }
        }

        if (hasAllDependencies) {
            notify(requestor, pair);
        } else {
            requestors.emplace(&requestor, std::move(pair));
        }
    } else {
        checkMissingAndNotify(requestor, std::move(pair));
    }
}

void ImageManager::removeRequestor(ImageRequestor& requestor) {
    requestors.erase(&requestor);
    missingImageRequestors.erase(&requestor);
    for (auto& requestedImage : requestedImages) {
        requestedImage.second.erase(&requestor);
    }
}

void ImageManager::notifyIfMissingImageAdded() {
    for (auto it = missingImageRequestors.begin(); it != missingImageRequestors.end();) {
        if (it->second.callbacks.empty()) {
            notify(*it->first, it->second.pair);
            it = missingImageRequestors.erase(it);
        } else {
            ++it;
        }
    }
}

void ImageManager::reduceMemoryUse() {
    std::vector<std::string> unusedIDs;
    unusedIDs.reserve(requestedImages.size());

    for (const auto& pair : requestedImages) {
        if (pair.second.empty() && images.find(pair.first) != images.end()) {
            unusedIDs.push_back(pair.first);
        }
    }

    if (!unusedIDs.empty()) {
        observer->onRemoveUnusedStyleImages(unusedIDs);
    }
}

void ImageManager::reduceMemoryUseIfCacheSizeExceedsLimit() {
    if (requestedImagesCacheSize > util::DEFAULT_ON_DEMAND_IMAGES_CACHE_SIZE) {
        reduceMemoryUse();
    }
}

void ImageManager::checkMissingAndNotify(ImageRequestor& requestor, const ImageRequestPair& pair) {
    std::vector<std::string> missingImages;
    missingImages.reserve(pair.first.size());
    for (const auto& dependency : pair.first) {
        if (images.find(dependency.first) == images.end()) {
            missingImages.push_back(dependency.first);
        }
    }

    if (!missingImages.empty()) {
        ImageRequestor* requestorPtr = &requestor;

        auto emplaced = missingImageRequestors.emplace(requestorPtr, MissingImageRequestPair { pair, {} });
        assert(emplaced.second);

        for (const auto& missingImage : missingImages) {
            assert(observer != nullptr);
            requestedImages[missingImage].emplace(&requestor);
            auto callback = std::make_unique<ActorCallback>(
                *Scheduler::GetCurrent(),
                [this, requestorPtr, missingImage] {
                    auto requestorIt = missingImageRequestors.find(requestorPtr);
                    if (requestorIt != missingImageRequestors.end()) {
                        assert(requestorIt->second.callbacks.find(missingImage) != requestorIt->second.callbacks.end());
                        requestorIt->second.callbacks.erase(missingImage);
                    }
            });

            auto actorRef = callback->self();
            emplaced.first->second.callbacks.emplace(missingImage, std::move(callback));
            observer->onStyleImageMissing(missingImage, [actorRef] {
                actorRef.invoke(&Callback::operator());
            });
        }
    } else {
        // Associate requestor with an image that was provided by the client.
        for (const auto& dependency : pair.first) {
            if (requestedImages.find(dependency.first) != requestedImages.end()) {
                requestedImages[dependency.first].emplace(&requestor);
            }
        }
        notify(requestor, pair);
    }
}

void ImageManager::notify(ImageRequestor& requestor, const ImageRequestPair& pair) const {
    ImageMap iconMap;
    ImageMap patternMap;
    ImageVersionMap versionMap;

    for (const auto& dependency : pair.first) {
        auto it = images.find(dependency.first);
        if (it != images.end()) {
            dependency.second == ImageType::Pattern ? patternMap.emplace(*it) : iconMap.emplace(*it);

            auto versionIt = updatedImageVersions.find(dependency.first);
            if (versionIt != updatedImageVersions.end()) {
                versionMap.emplace(versionIt->first, versionIt->second);
            }
        }
    }

    requestor.onImagesAvailable(iconMap, patternMap, std::move(versionMap), pair.second);
}

void ImageManager::dumpDebugLogs() const {
    Log::Info(Event::General, "ImageManager::loaded: %d", loaded);
}

ImageRequestor::ImageRequestor(ImageManager& imageManager_) : imageManager(imageManager_) {
}

ImageRequestor::~ImageRequestor() {
    imageManager.removeRequestor(*this);
}

} // namespace mbgl
