#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/actor/actor.hpp>
#include <mbgl/actor/scheduler.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/gfx/upload_pass.hpp>
#include <mbgl/gfx/context.hpp>
#include <mbgl/renderer/image_manager_observer.hpp>

namespace mbgl {

static ImageManagerObserver nullObserver;

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
    images.emplace(image_->id, std::move(image_));
}

bool ImageManager::updateImage(Immutable<style::Image::Impl> image_) {
    auto oldImage = images.find(image_->id);
    assert(oldImage != images.end());
    if (oldImage == images.end()) return false;

    auto sizeChanged = oldImage->second->image.size != image_->image.size;

    if (sizeChanged) {
        updatedImageVersions.erase(image_->id);
    } else {
        updatedImageVersions[image_->id]++;
    }

    removePattern(image_->id);
    oldImage->second = std::move(image_);

    return sizeChanged;
}

void ImageManager::removeImage(const std::string& id) {
    assert(images.find(id) != images.end());
    images.erase(id);
    requestedImages.erase(id);
    removePattern(id);
}

void ImageManager::removePattern(const std::string& id) {
    auto it = patterns.find(id);
    if (it != patterns.end()) {
        // Clear pattern from the atlas image.
        const uint32_t x = it->second.bin->x;
        const uint32_t y = it->second.bin->y;
        const uint32_t w = it->second.bin->w;
        const uint32_t h = it->second.bin->h;
        PremultipliedImage::clear(atlasImage, { x, y }, { w, h });

        shelfPack.unref(*it->second.bin);
        patterns.erase(it);
    }
}

const style::Image::Impl* ImageManager::getImage(const std::string& id) const {
    const auto it = images.find(id);
    if (it != images.end()) {
        return it->second.get();
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
            } else {
                // Associate requestor with an image that was provided by the client.
                auto it = requestedImages.find(dependency.first);
                if (it != requestedImages.end()) {
                    it->second.emplace(&requestor);
                }
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
    for (auto it = requestedImages.cbegin(); it != requestedImages.cend();) {
        if (it->second.empty() && images.find(it->first) != images.end()) {
            images.erase(it->first);
            removePattern(it->first);
            it = requestedImages.erase(it);
        } else {
            ++it;
        }
    }
}

void ImageManager::checkMissingAndNotify(ImageRequestor& requestor, const ImageRequestPair& pair) {
    unsigned int missing = 0;
    for (const auto& dependency : pair.first) {
        auto it = images.find(dependency.first);
        if (it == images.end()) {
            missing++;
            requestedImages[dependency.first].emplace(&requestor);
        }
    }

    if (missing > 0) {
        ImageRequestor* requestorPtr = &requestor;

        auto emplaced = missingImageRequestors.emplace(requestorPtr, MissingImageRequestPair { pair, {} });
        assert(emplaced.second);

        for (const auto& dependency : pair.first) {
            auto it = images.find(dependency.first);
            if (it == images.end()) {
                assert(observer != nullptr);
                auto callback = std::make_unique<ActorCallback>(
                        *Scheduler::GetCurrent(),
                        [this, requestorPtr, imageId = dependency.first] {
                            auto requestorIt = missingImageRequestors.find(requestorPtr);
                            if (requestorIt != missingImageRequestors.end()) {
                                assert(requestorIt->second.callbacks.find(imageId) != requestorIt->second.callbacks.end());
                                requestorIt->second.callbacks.erase(imageId);
                            }
                        });

                auto actorRef = callback->self();
                emplaced.first->second.callbacks.emplace(dependency.first, std::move(callback));
                observer->onStyleImageMissing(dependency.first, [actorRef]() {
                    actorRef.invoke(&Callback::operator());
                });

            }
        }

    } else {
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

// When copied into the atlas texture, image data is padded by one pixel on each side. Icon
// images are padded with fully transparent pixels, while pattern images are padded with a
// copy of the image data wrapped from the opposite side. In both cases, this ensures the
// correct behavior of GL_LINEAR texture sampling mode.
static constexpr uint16_t padding = 1;

static mapbox::ShelfPack::ShelfPackOptions shelfPackOptions() {
    mapbox::ShelfPack::ShelfPackOptions options;
    options.autoResize = true;
    return options;
}

ImageManager::ImageManager()
    : shelfPack(64, 64, shelfPackOptions()) {
}

ImageManager::~ImageManager() = default;

optional<ImagePosition> ImageManager::getPattern(const std::string& id) {
    auto it = patterns.find(id);
    if (it != patterns.end()) {
        return it->second.position;
    }

    const style::Image::Impl* image = getImage(id);
    if (!image) {
        return {};
    }

    const uint16_t width = image->image.size.width + padding * 2;
    const uint16_t height = image->image.size.height + padding * 2;

    mapbox::Bin* bin = shelfPack.packOne(-1, width, height);
    if (!bin) {
        return {};
    }

    atlasImage.resize(getPixelSize());

    const PremultipliedImage& src = image->image;

    const uint32_t x = bin->x + padding;
    const uint32_t y = bin->y + padding;
    const uint32_t w = src.size.width;
    const uint32_t h = src.size.height;

    PremultipliedImage::copy(src, atlasImage, { 0, 0 }, { x, y }, { w, h });

    // Add 1 pixel wrapped padding on each side of the image.
    PremultipliedImage::copy(src, atlasImage, { 0, h - 1 }, { x, y - 1 }, { w, 1 }); // T
    PremultipliedImage::copy(src, atlasImage, { 0,     0 }, { x, y + h }, { w, 1 }); // B
    PremultipliedImage::copy(src, atlasImage, { w - 1, 0 }, { x - 1, y }, { 1, h }); // L
    PremultipliedImage::copy(src, atlasImage, { 0,     0 }, { x + w, y }, { 1, h }); // R

    dirty = true;

    return patterns.emplace(id, Pattern { bin, { *bin, *image } }).first->second.position;
}

Size ImageManager::getPixelSize() const {
    return Size {
        static_cast<uint32_t>(shelfPack.width()),
        static_cast<uint32_t>(shelfPack.height())
    };
}

void ImageManager::upload(gfx::UploadPass& uploadPass) {
    if (!atlasTexture) {
        atlasTexture = uploadPass.createTexture(atlasImage);
    } else if (dirty) {
        uploadPass.updateTexture(*atlasTexture, atlasImage);
    }

    dirty = false;
}

gfx::TextureBinding ImageManager::textureBinding() {
    assert(atlasTexture);
    assert(!dirty);
    return { atlasTexture->getResource(), gfx::TextureFilterType::Linear };
}

ImageRequestor::ImageRequestor(ImageManager& imageManager_) : imageManager(imageManager_) {
}

ImageRequestor::~ImageRequestor() {
    imageManager.removeRequestor(*this);
}

} // namespace mbgl
