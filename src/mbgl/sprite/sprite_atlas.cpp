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

SpriteAtlasElement::SpriteAtlasElement(Rect<uint16_t> rect, const style::Image::Impl& image)
    : sdf(image.sdf),
      pixelRatio(image.pixelRatio),
      textureRect(
        rect.x + padding,
        rect.y + padding,
        rect.w - padding * 2,
        rect.h - padding * 2
      ) {
}

SpriteAtlas::SpriteAtlas(Size size, float pixelRatio)
    : pixelSize(std::ceil(size.width * pixelRatio),
                std::ceil(size.height * pixelRatio)),
      bin(pixelSize.width, pixelSize.height),
      dirty(true) {
}

SpriteAtlas::~SpriteAtlas() = default;

void SpriteAtlas::onSpriteLoaded() {
    markAsLoaded();
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

    if (entry.iconRect) {
        copy(entry, &Entry::iconRect);
    }

    if (entry.patternRect) {
        copy(entry, &Entry::patternRect);
    }
}

void SpriteAtlas::removeImage(const std::string& id) {
    icons.clear();

    auto it = entries.find(id);
    assert(it != entries.end());

    Entry& entry = it->second;

    if (entry.iconRect) {
        bin.release(*entry.iconRect);
    }

    if (entry.patternRect) {
        bin.release(*entry.patternRect);
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

void SpriteAtlas::getIcons(IconRequestor& requestor) {
    if (isLoaded()) {
        requestor.onIconsAvailable(buildIconMap());
    } else {
        requestors.insert(&requestor);
    }
}

void SpriteAtlas::removeRequestor(IconRequestor& requestor) {
    requestors.erase(&requestor);
}

optional<SpriteAtlasElement> SpriteAtlas::getIcon(const std::string& id) {
    return getImage(id, &Entry::iconRect);
}

optional<SpriteAtlasElement> SpriteAtlas::getPattern(const std::string& id) {
    return getImage(id, &Entry::patternRect);
}

optional<SpriteAtlasElement> SpriteAtlas::getImage(const std::string& id,
                                                   optional<Rect<uint16_t>> Entry::*entryRect) {

    auto it = entries.find(id);
    if (it == entries.end()) {
        if (!entries.empty()) {
            Log::Info(Event::Sprite, "Can't find sprite named '%s'", id.c_str());
        }
        return {};
    }

    Entry& entry = it->second;

    if (entry.*entryRect) {
        assert(entry.image.get());
        return SpriteAtlasElement {
            *(entry.*entryRect),
            *entry.image
        };
    }

    const uint16_t width = entry.image->image.size.width + padding * 2;
    const uint16_t height = entry.image->image.size.height + padding * 2;

    Rect<uint16_t> rect = bin.allocate(width, height);
    if (rect.w == 0) {
        if (debug::spriteWarnings) {
            Log::Warning(Event::Sprite, "sprite atlas bitmap overflow");
        }
        return {};
    }

    entry.*entryRect = rect;
    copy(entry, entryRect);

    return SpriteAtlasElement {
        rect,
        *entry.image
    };
}

Size SpriteAtlas::getPixelSize() const {
    return pixelSize;
}

void SpriteAtlas::copy(const Entry& entry, optional<Rect<uint16_t>> Entry::*entryRect) {
    if (!image.valid()) {
        image = PremultipliedImage(getPixelSize());
        image.fill(0);
    }

    const PremultipliedImage& src = entry.image->image;
    const Rect<uint16_t>& rect = *(entry.*entryRect);

    const uint32_t x = rect.x + padding;
    const uint32_t y = rect.y + padding;
    const uint32_t w = src.size.width;
    const uint32_t h = src.size.height;

    PremultipliedImage::copy(src, image, { 0, 0 }, { x, y }, { w, h });

    if (entryRect == &Entry::patternRect) {
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
