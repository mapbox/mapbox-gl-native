#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/sprite/sprite_atlas_observer.hpp>
#include <mbgl/sprite/sprite_parser.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/response.hpp>

#include <cassert>
#include <cmath>
#include <algorithm>

namespace mbgl {

static SpriteAtlasObserver nullObserver;

struct SpriteAtlas::Loader {
    std::shared_ptr<const std::string> image;
    std::shared_ptr<const std::string> json;
    std::unique_ptr<AsyncRequest> jsonRequest;
    std::unique_ptr<AsyncRequest> spriteRequest;
};

SpriteAtlas::SpriteAtlas(Size size_, float pixelRatio_)
    : size(std::move(size_)),
      pixelRatio(pixelRatio_),
      observer(&nullObserver),
      bin(size.width, size.height),
      dirty(true) {
}

SpriteAtlas::~SpriteAtlas() = default;

void SpriteAtlas::load(const std::string& url, FileSource& fileSource) {
    if (url.empty()) {
        // Treat a non-existent sprite as a successfully loaded empty sprite.
        loaded = true;
        return;
    }

    loader = std::make_unique<Loader>();

    loader->jsonRequest = fileSource.request(Resource::spriteJSON(url, pixelRatio), [this](Response res) {
        if (res.error) {
            observer->onSpriteError(std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            loader->json = std::make_shared<const std::string>();
            emitSpriteLoadedIfComplete();
        } else {
            // Only trigger a sprite loaded event we got new data.
            loader->json = res.data;
            emitSpriteLoadedIfComplete();
        }
    });

    loader->spriteRequest = fileSource.request(Resource::spriteImage(url, pixelRatio), [this](Response res) {
        if (res.error) {
            observer->onSpriteError(std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            loader->image = std::make_shared<const std::string>();
            emitSpriteLoadedIfComplete();
        } else {
            loader->image = res.data;
            emitSpriteLoadedIfComplete();
        }
    });
}

void SpriteAtlas::emitSpriteLoadedIfComplete() {
    assert(loader);

    if (!loader->image || !loader->json) {
        return;
    }

    auto result = parseSprite(*loader->image, *loader->json);
    if (result.is<Sprites>()) {
        loaded = true;
        setSprites(result.get<Sprites>());
        observer->onSpriteLoaded();
    } else {
        observer->onSpriteError(result.get<std::exception_ptr>());
    }
}

void SpriteAtlas::setObserver(SpriteAtlasObserver* observer_) {
    observer = observer_;
}

void SpriteAtlas::dumpDebugLogs() const {
    Log::Info(Event::General, "SpriteAtlas::loaded: %d", loaded);
}

void SpriteAtlas::setSprites(const Sprites& newSprites) {
    std::lock_guard<std::recursive_mutex> lock(mutex);
    for (const auto& pair : newSprites) {
        _setSprite(pair.first, pair.second);
    }
}

void SpriteAtlas::setSprite(const std::string& name, std::shared_ptr<const SpriteImage> sprite) {
    std::lock_guard<std::recursive_mutex> lock(mutex);
    _setSprite(name, sprite);
}

void SpriteAtlas::removeSprite(const std::string& name) {
    std::lock_guard<std::recursive_mutex> lock(mutex);

    auto it = entries.find(name);
    if (it == entries.end()) {
        return;
    }

    Entry& entry = it->second;

    if (entry.iconRect) {
        bin.release(*entry.iconRect);
    }

    if (entry.patternRect) {
        bin.release(*entry.patternRect);
    }

    entries.erase(it);
}

void SpriteAtlas::_setSprite(const std::string& name,
                             const std::shared_ptr<const SpriteImage>& sprite) {
    auto it = entries.find(name);
    if (it == entries.end()) {
        entries.emplace(name, Entry { sprite, {}, {} });
        return;
    }

    Entry& entry = it->second;

    // There is already a sprite with that name in our store.
    if (entry.spriteImage->image.size != sprite->image.size) {
        Log::Warning(Event::Sprite, "Can't change sprite dimensions for '%s'", name.c_str());
        return;
    }

    entry.spriteImage = sprite;

    if (entry.iconRect) {
        copy(entry.spriteImage->image, *entry.iconRect, SpritePatternMode::Single);
    }

    if (entry.patternRect) {
        copy(entry.spriteImage->image, *entry.patternRect, SpritePatternMode::Repeating);
    }
}

std::shared_ptr<const SpriteImage> SpriteAtlas::getSprite(const std::string& name) {
    std::lock_guard<std::recursive_mutex> lock(mutex);
    const auto it = entries.find(name);
    if (it != entries.end()) {
        return it->second.spriteImage;
    } else {
        if (!entries.empty()) {
            Log::Info(Event::Sprite, "Can't find sprite named '%s'", name.c_str());
        }
        return nullptr;
    }
}

Rect<uint16_t> SpriteAtlas::allocateImage(const SpriteImage& spriteImage) {

    const uint16_t pixel_width = std::ceil(spriteImage.image.size.width / pixelRatio);
    const uint16_t pixel_height = std::ceil(spriteImage.image.size.height / pixelRatio);

    // Increase to next number divisible by 4, but at least 1.
    // This is so we can scale down the texture coordinates and pack them
    // into 2 bytes rather than 4 bytes.
    const uint16_t pack_width = (pixel_width + 1) + (4 - (pixel_width + 1) % 4);
    const uint16_t pack_height = (pixel_height + 1) + (4 - (pixel_height + 1) % 4);

    // We have to allocate a new area in the bin, and store an empty image in it.
    // Add a 1px border around every image.
    Rect<uint16_t> rect = bin.allocate(pack_width, pack_height);
    if (rect.w == 0) {
        return rect;
    }

    return rect;
}

optional<SpriteAtlasElement> SpriteAtlas::getImage(const std::string& name,
                                                   const SpritePatternMode mode) {
    std::lock_guard<std::recursive_mutex> lock(mutex);

    auto it = entries.find(name);
    if (it == entries.end()) {
        if (!entries.empty()) {
            Log::Info(Event::Sprite, "Can't find sprite named '%s'", name.c_str());
        }
        return {};
    }

    Entry& entry = it->second;

    if (mode == SpritePatternMode::Single && entry.iconRect) {
        return SpriteAtlasElement {
            *entry.iconRect,
            entry.spriteImage,
            entry.spriteImage->pixelRatio / pixelRatio
        };
    }

    if (mode == SpritePatternMode::Repeating && entry.patternRect) {
        return SpriteAtlasElement {
            *entry.patternRect,
            entry.spriteImage,
            entry.spriteImage->pixelRatio / pixelRatio
        };
    }

    Rect<uint16_t> rect = allocateImage(*entry.spriteImage);
    if (rect.w == 0) {
        if (debug::spriteWarnings) {
            Log::Warning(Event::Sprite, "sprite atlas bitmap overflow");
        }
        return {};
    }

    copy(entry.spriteImage->image, rect, mode);

    if (mode == SpritePatternMode::Single) {
        entry.iconRect = rect;
    } else {
        entry.patternRect = rect;
    }

    return SpriteAtlasElement {
        rect,
        entry.spriteImage,
        entry.spriteImage->pixelRatio / pixelRatio
    };
}

optional<SpriteAtlasPosition> SpriteAtlas::getPosition(const std::string& name,
                                                       const SpritePatternMode mode) {
    std::lock_guard<std::recursive_mutex> lock(mutex);

    auto img = getImage(name, mode);
    if (!img) {
        return {};
    }

    auto rect = (*img).pos;

    const float padding = 1;
    auto spriteImage = (*img).spriteImage;

    const float w = spriteImage->getWidth() * (*img).relativePixelRatio;
    const float h = spriteImage->getHeight() * (*img).relativePixelRatio;

    return SpriteAtlasPosition {
        {{ float(spriteImage->getWidth()), spriteImage->getHeight() }},
        {{ float(rect.x + padding)     / size.width, float(rect.y + padding)     / size.height }},
        {{ float(rect.x + padding + w) / size.width, float(rect.y + padding + h) / size.height }}
    };
}

void SpriteAtlas::copy(const PremultipliedImage& src, Rect<uint16_t> pos, const SpritePatternMode mode) {
    if (!image.valid()) {
        image = PremultipliedImage({ static_cast<uint32_t>(std::ceil(size.width * pixelRatio)),
                                     static_cast<uint32_t>(std::ceil(size.height * pixelRatio)) });
        image.fill(0);
    }

    if (!src.valid()) {
        return;
    }

    const uint32_t padding = 1;
    const uint32_t x = (pos.x + padding) * pixelRatio;
    const uint32_t y = (pos.y + padding) * pixelRatio;
    const uint32_t w = src.size.width;
    const uint32_t h = src.size.height;

    PremultipliedImage::copy(src, image, { 0, 0 }, { x, y }, { w, h });

    if (mode == SpritePatternMode::Repeating) {
        // Add 1 pixel wrapped padding on each side of the image.
        PremultipliedImage::copy(src, image, { 0, h - 1 }, { x, y - 1 }, { w, 1 }); // T
        PremultipliedImage::copy(src, image, { 0,     0 }, { x, y + h }, { w, 1 }); // B
        PremultipliedImage::copy(src, image, { w - 1, 0 }, { x - 1, y }, { 1, h }); // L
        PremultipliedImage::copy(src, image, { 0,     0 }, { x + w, y }, { 1, h }); // R
    }

    dirty = true;
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

} // namespace mbgl
