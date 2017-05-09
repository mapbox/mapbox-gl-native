#include <mbgl/sprite/sprite_atlas.hpp>
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

SpriteAtlasElement::SpriteAtlasElement(Rect<uint16_t> rect_,
                                       const style::Image& image,
                                       Size size_, float pixelRatio)
    : pos(std::move(rect_)),
      sdf(image.isSdf()),
      relativePixelRatio(image.getPixelRatio() / pixelRatio),
      width(image.getWidth()),
      height(image.getHeight()) {

    const float padding = 1;

    const float w = image.getWidth() * relativePixelRatio;
    const float h = image.getHeight() * relativePixelRatio;

    size = {{ float(image.getWidth()), image.getHeight() }};
    tl   = {{ float(pos.x + padding)     / size_.width, float(pos.y + padding)     / size_.height }};
    br   = {{ float(pos.x + padding + w) / size_.width, float(pos.y + padding + h) / size_.height }};
}

SpriteAtlas::SpriteAtlas(Size size_, float pixelRatio_)
    : size(std::move(size_)),
      pixelRatio(pixelRatio_),
      bin(size.width, size.height),
      dirty(true) {
}

SpriteAtlas::~SpriteAtlas() = default;

void SpriteAtlas::onSpriteLoaded(Images&& result) {
    markAsLoaded();

    for (auto& pair : result) {
        addImage(pair.first, std::move(pair.second));
    }

    for (auto requestor : requestors) {
        requestor->onIconsAvailable(buildIconMap());
    }
    requestors.clear();
}

void SpriteAtlas::addImage(const std::string& id, std::unique_ptr<style::Image> image_) {
    icons.clear();

    auto it = entries.find(id);
    if (it == entries.end()) {
        entries.emplace(id, Entry { std::move(image_), {}, {} });
        return;
    }

    Entry& entry = it->second;

    // There is already a sprite with that name in our store.
    assert(entry.image->getImage().size == image_->getImage().size);

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

const style::Image* SpriteAtlas::getImage(const std::string& id) const {
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
            *entry.image,
            size,
            pixelRatio
        };
    }

    const uint16_t pixelWidth = std::ceil(entry.image->getImage().size.width / pixelRatio);
    const uint16_t pixelHeight = std::ceil(entry.image->getImage().size.height / pixelRatio);

    // Increase to next number divisible by 4, but at least 1.
    // This is so we can scale down the texture coordinates and pack them
    // into 2 bytes rather than 4 bytes.
    const uint16_t packWidth = (pixelWidth + 1) + (4 - (pixelWidth + 1) % 4);
    const uint16_t packHeight = (pixelHeight + 1) + (4 - (pixelHeight + 1) % 4);

    // We have to allocate a new area in the bin, and store an empty image in it.
    Rect<uint16_t> rect = bin.allocate(packWidth, packHeight);
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
        *entry.image,
        size,
        pixelRatio
    };
}

void SpriteAtlas::copy(const Entry& entry, optional<Rect<uint16_t>> Entry::*entryRect) {
    if (!image.valid()) {
        image = PremultipliedImage({ static_cast<uint32_t>(std::ceil(size.width * pixelRatio)),
                                     static_cast<uint32_t>(std::ceil(size.height * pixelRatio)) });
        image.fill(0);
    }

    const PremultipliedImage& src = entry.image->getImage();
    const Rect<uint16_t>& rect = *(entry.*entryRect);

    const uint32_t padding = 1;
    const uint32_t x = (rect.x + padding) * pixelRatio;
    const uint32_t y = (rect.y + padding) * pixelRatio;
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
