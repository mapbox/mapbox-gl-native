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
    std::lock_guard<std::mutex> lock(mutex);
    for (const auto& pair : newSprites) {
        _setSprite(pair.first, pair.second);
    }
}

void SpriteAtlas::setSprite(const std::string& name, std::shared_ptr<const SpriteImage> sprite) {
    std::lock_guard<std::mutex> lock(mutex);
    _setSprite(name, sprite);
}

void SpriteAtlas::removeSprite(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    _setSprite(name);
}

void SpriteAtlas::_setSprite(const std::string& name,
                             const std::shared_ptr<const SpriteImage>& sprite) {
    if (sprite) {
        auto it = sprites.find(name);
        if (it != sprites.end()) {
            // There is already a sprite with that name in our store.
            if (it->second->image.size != sprite->image.size) {
                Log::Warning(Event::Sprite, "Can't change sprite dimensions for '%s'", name.c_str());
                return;
            }
            it->second = sprite;
        } else {
            sprites.emplace(name, sprite);
        }

        // Always add/replace the value in the dirty list.
        auto dirty_it = dirtySprites.find(name);
        if (dirty_it != dirtySprites.end()) {
            dirty_it->second = sprite;
        } else {
            dirtySprites.emplace(name, sprite);
        }
    } else if (sprites.erase(name) > 0) {
        dirtySprites.emplace(name, nullptr);
    }
}

std::shared_ptr<const SpriteImage> SpriteAtlas::getSprite(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    const auto it = sprites.find(name);
    if (it != sprites.end()) {
        return it->second;
    } else {
        if (!sprites.empty()) {
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
    std::lock_guard<std::recursive_mutex> lock(mtx);

    auto rect_it = images.find({ name, mode });
    if (rect_it != images.end()) {
        return SpriteAtlasElement { rect_it->second.pos, rect_it->second.spriteImage, rect_it->second.spriteImage->pixelRatio / pixelRatio };
    }

    auto sprite = getSprite(name);
    if (!sprite) {
        return {};
    }

    Rect<uint16_t> rect = allocateImage(*sprite);
    if (rect.w == 0) {
        if (debug::spriteWarnings) {
            Log::Warning(Event::Sprite, "sprite atlas bitmap overflow");
        }
        return {};
    }

    const Holder& holder = images.emplace(Key{ name, mode }, Holder{ sprite, rect }).first->second;
    copy(holder, mode);

    return SpriteAtlasElement { rect, sprite, sprite->pixelRatio / pixelRatio };
}

optional<SpriteAtlasPosition> SpriteAtlas::getPosition(const std::string& name,
                                                       const SpritePatternMode mode) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

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

void SpriteAtlas::copy(const Holder& holder, const SpritePatternMode mode) {
    if (!image.valid()) {
        image = PremultipliedImage({ static_cast<uint32_t>(std::ceil(size.width * pixelRatio)),
                                     static_cast<uint32_t>(std::ceil(size.height * pixelRatio)) });
        image.fill(0);
    }

    if (!holder.spriteImage->image.valid()) {
        return;
    }

    const uint32_t padding = 1;
    const uint32_t x = (holder.pos.x + padding) * pixelRatio;
    const uint32_t y = (holder.pos.y + padding) * pixelRatio;
    const uint32_t w = holder.spriteImage->image.size.width;
    const uint32_t h = holder.spriteImage->image.size.height;

    PremultipliedImage::copy(holder.spriteImage->image, image, { 0, 0 }, { x, y }, { w, h });

    if (mode == SpritePatternMode::Repeating) {
        // Add 1 pixel wrapped padding on each side of the image.
        PremultipliedImage::copy(holder.spriteImage->image, image, { 0, h - 1 }, { x, y - 1 }, { w, 1 }); // T
        PremultipliedImage::copy(holder.spriteImage->image, image, { 0,     0 }, { x, y + h }, { w, 1 }); // B
        PremultipliedImage::copy(holder.spriteImage->image, image, { w - 1, 0 }, { x - 1, y }, { 1, h }); // L
        PremultipliedImage::copy(holder.spriteImage->image, image, { 0,     0 }, { x + w, y }, { 1, h }); // R
    }

    dirty = true;
}

void SpriteAtlas::updateDirty() {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    auto imageIterator = images.begin();
    auto spriteIterator = dirtySprites.begin();
    while (imageIterator != images.end() && spriteIterator != dirtySprites.end()) {
        if (imageIterator->first.first < spriteIterator->first) {
            ++imageIterator;
        } else if (spriteIterator->first < imageIterator->first.first) {
            ++spriteIterator;
        } else {
            // The two names match;
            Holder& holder = imageIterator->second;
            holder.spriteImage = spriteIterator->second;
            if (holder.spriteImage != nullptr) {
                copy(holder, imageIterator->first.second);
                ++imageIterator;
            } else {
                images.erase(imageIterator++);
            }
            // Don't advance the spriteIterator because there might be another sprite with the same
            // name, but a different wrap value.
        }
    }

    dirtySprites.clear();
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

SpriteAtlas::Holder::Holder(std::shared_ptr<const SpriteImage> spriteImage_, Rect<uint16_t> pos_)
    : spriteImage(std::move(spriteImage_)), pos(std::move(pos_)) {
}

SpriteAtlas::Holder::Holder(Holder&& h) : spriteImage(std::move(h.spriteImage)), pos(h.pos) {
}

} // namespace mbgl
