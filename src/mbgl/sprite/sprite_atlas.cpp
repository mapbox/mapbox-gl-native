#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/style/image_impl.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/exception.hpp>

#include <cassert>
#include <cmath>
#include <algorithm>

namespace mbgl {

// When copied into the atlas texture, image data is padded by one pixel on each side. Icon
// images are padded with fully transparent pixels, while pattern images are padded with a
// copy of the image data wrapped from the opposite side. In both cases, this ensures the
// correct behavior of GL_LINEAR texture sampling mode.
static constexpr uint16_t padding = 1;

SpriteAtlasElement::SpriteAtlasElement(const mapbox::Bin& bin, const style::Image::Impl& image)
    : sdf(image.sdf),
      pixelRatio(image.pixelRatio),
      textureRect(
        bin.x + padding,
        bin.y + padding,
        bin.w - padding * 2,
        bin.h - padding * 2
      ) {
}

static mapbox::ShelfPack::ShelfPackOptions shelfPackOptions() {
    mapbox::ShelfPack::ShelfPackOptions options;
    options.autoResize = true;
    return options;
}

SpriteAtlas::SpriteAtlas()
    : shelfPack(64, 64, shelfPackOptions()) {
}

SpriteAtlas::~SpriteAtlas() = default;

void SpriteAtlas::onSpriteLoaded() {
    loaded = true;
    for (auto requestor : requestors) {
        requestor->onIconsAvailable(buildIconMap());
    }
    requestors.clear();
}

void SpriteAtlas::addImage(Immutable<style::Image::Impl> image_) {
    icons.clear();

    auto it = entries.find(image_->id);
    if (it == entries.end()) {
        entries.emplace(image_->id, Entry { image_, {}, {} });
        return;
    }

    Entry& entry = it->second;

    // There is already a sprite with that name in our store.
    assert(entry.image->image.size == image_->image.size);

    entry.image = std::move(image_);

    if (entry.iconBin) {
        copy(entry, &Entry::iconBin);
    }

    if (entry.patternBin) {
        copy(entry, &Entry::patternBin);
    }
}

void SpriteAtlas::removeImage(const std::string& id) {
    icons.clear();

    auto it = entries.find(id);
    assert(it != entries.end());

    Entry& entry = it->second;

    if (entry.iconBin) {
        shelfPack.unref(*entry.iconBin);
    }

    if (entry.patternBin) {
        shelfPack.unref(*entry.patternBin);
    }

    entries.erase(it);
}

const style::Image::Impl* SpriteAtlas::getImage(const std::string& id) const {
    const auto it = entries.find(id);
    if (it != entries.end()) {
        return it->second.image.get();
    }
    if (!entries.empty()) {
        Log::Info(Event::Sprite, "Can't find sprite named '%s'", id.c_str());
    }
    return nullptr;
}

void SpriteAtlas::getIcons(IconRequestor& requestor, IconDependencies dependencies) {
    // If the sprite has been loaded, or if all the icon dependencies are already present
    // (i.e. if they've been addeded via runtime styling), then notify the requestor immediately.
    // Otherwise, delay notification until the sprite is loaded. At that point, if any of the
    // dependencies are still unavailable, we'll just assume they are permanently missing.
    bool hasAllDependencies = true;
    if (!isLoaded()) {
        for (const auto& dependency : dependencies) {
            if (entries.find(dependency) == entries.end()) {
                hasAllDependencies = false;
            }
        }
    }
    if (isLoaded() || hasAllDependencies) {
        requestor.onIconsAvailable(buildIconMap());
    } else {
        requestors.insert(&requestor);
    }
}

void SpriteAtlas::removeRequestor(IconRequestor& requestor) {
    requestors.erase(&requestor);
}

optional<SpriteAtlasElement> SpriteAtlas::getIcon(const std::string& id) {
    return getImage(id, &Entry::iconBin);
}

optional<SpriteAtlasElement> SpriteAtlas::getPattern(const std::string& id) {
    return getImage(id, &Entry::patternBin);
}

optional<SpriteAtlasElement> SpriteAtlas::getImage(const std::string& id, mapbox::Bin* Entry::*entryBin) {
    auto it = entries.find(id);
    if (it == entries.end()) {
        if (!entries.empty()) {
            Log::Info(Event::Sprite, "Can't find sprite named '%s'", id.c_str());
        }
        return {};
    }

    Entry& entry = it->second;

    if (entry.*entryBin) {
        assert(entry.image.get());
        return SpriteAtlasElement {
            *(entry.*entryBin),
            *entry.image
        };
    }

    const uint16_t width = entry.image->image.size.width + padding * 2;
    const uint16_t height = entry.image->image.size.height + padding * 2;

    mapbox::Bin* bin = shelfPack.packOne(-1, width, height);
    if (!bin) {
        if (debug::spriteWarnings) {
            Log::Warning(Event::Sprite, "sprite atlas bitmap overflow");
        }
        return {};
    }

    entry.*entryBin = bin;
    copy(entry, entryBin);

    return SpriteAtlasElement {
        *bin,
        *entry.image
    };
}

Size SpriteAtlas::getPixelSize() const {
    return Size {
        static_cast<uint32_t>(shelfPack.width()),
        static_cast<uint32_t>(shelfPack.height())
    };
}

void SpriteAtlas::copy(const Entry& entry, mapbox::Bin* Entry::*entryBin) {
    if (!image.valid()) {
        image = PremultipliedImage(getPixelSize());
        image.fill(0);
    } else if (image.size != getPixelSize()) {
        PremultipliedImage newImage(getPixelSize());
        PremultipliedImage::copy(image, newImage, { 0, 0 }, { 0, 0 }, image.size);
        image = std::move(newImage);
    }

    const PremultipliedImage& src = entry.image->image;
    const mapbox::Bin& bin = *(entry.*entryBin);

    const uint32_t x = bin.x + padding;
    const uint32_t y = bin.y + padding;
    const uint32_t w = src.size.width;
    const uint32_t h = src.size.height;

    PremultipliedImage::copy(src, image, { 0, 0 }, { x, y }, { w, h });

    if (entryBin == &Entry::patternBin) {
        // Add 1 pixel wrapped padding on each side of the image.
        PremultipliedImage::copy(src, image, { 0, h - 1 }, { x, y - 1 }, { w, 1 }); // T
        PremultipliedImage::copy(src, image, { 0,     0 }, { x, y + h }, { w, 1 }); // B
        PremultipliedImage::copy(src, image, { w - 1, 0 }, { x - 1, y }, { 1, h }); // L
        PremultipliedImage::copy(src, image, { 0,     0 }, { x + w, y }, { 1, h }); // R
    }

    dirty = true;
}

IconMap SpriteAtlas::buildIconMap() {
    if (icons.empty()) {
        for (const auto& entry : entries) {
            icons.emplace(std::piecewise_construct,
                          std::forward_as_tuple(entry.first),
                          std::forward_as_tuple(*getIcon(entry.first)));

        }
    }
    return icons;
}

void SpriteAtlas::upload(gl::Context& context, gl::TextureUnit unit) {
    if (!texture) {
        texture = context.createTexture(image, unit);
    } else if (dirty) {
        context.updateTexture(*texture, image, unit);
    }

#if not MBGL_USE_GLES2
//    if (dirty) {
//        platform::showColorDebugImage("Sprite Atlas",
//                                      reinterpret_cast<const char*>(image.data.get()), size.width,
//                                      size.height, image.size.width, image.size.height);
//    }
#endif // MBGL_USE_GLES2

    dirty = false;
}

void SpriteAtlas::bind(bool linear, gl::Context& context, gl::TextureUnit unit) {
    upload(context, unit);
    context.bindTexture(*texture, unit,
                        linear ? gl::TextureFilter::Linear : gl::TextureFilter::Nearest);
}

void SpriteAtlas::dumpDebugLogs() const {
    Log::Info(Event::General, "SpriteAtlas::loaded: %d", loaded);
}

} // namespace mbgl
