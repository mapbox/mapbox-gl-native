#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/util/logging.hpp>
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

    removeImage(image_->id);
    addImage(std::move(image_));

    return sizeChanged;
}

void ImageManager::removeImage(const std::string& id) {
    assert(images.find(id) != images.end());
    images.erase(id);

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
}

void ImageManager::notifyIfMissingImageAdded() {
    for (auto it = missingImageRequestors.begin(); it != missingImageRequestors.end();) {
        if (it->second.callbacksRemaining == 0) {
            notify(*it->first, it->second.pair);
            missingImageRequestors.erase(it++);
        } else {
            it++;
        }
    }
}

void ImageManager::checkMissingAndNotify(ImageRequestor& requestor, const ImageRequestPair& pair) {
    unsigned int missing = 0;
    for (const auto& dependency : pair.first) {
        auto it = images.find(dependency.first);
        if (it == images.end()) {
            missing++;
        }
    }

    if (missing > 0) {
        ImageRequestor* requestorPtr = &requestor;

        missingImageRequestors.emplace(requestorPtr, MissingImageRequestPair { std::move(pair), missing });

        for (const auto& dependency : pair.first) {
            auto it = images.find(dependency.first);
            if (it == images.end()) {
                assert(observer != nullptr);
                observer->onStyleImageMissing(dependency.first, [this, requestorPtr]() {
                    auto requestorIt = missingImageRequestors.find(requestorPtr);
                    if (requestorIt != missingImageRequestors.end()) {
                        assert(requestorIt->second.callbacksRemaining > 0);
                        requestorIt->second.callbacksRemaining--;
                    }
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

void ImageManager::upload(gfx::Context& context) {
    if (!atlasTexture) {
        atlasTexture = context.createTexture(atlasImage);
    } else if (dirty) {
        context.updateTexture(*atlasTexture, atlasImage);
    }

    dirty = false;
}

gfx::TextureBinding ImageManager::textureBinding(gfx::Context& context) {
    upload(context);
    return { atlasTexture->getResource(), gfx::TextureFilterType::Linear };
}

} // namespace mbgl
