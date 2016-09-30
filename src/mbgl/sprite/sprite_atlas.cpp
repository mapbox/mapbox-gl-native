#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/sprite/sprite_atlas_observer.hpp>
#include <mbgl/sprite/sprite_parser.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/platform/platform.hpp>
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

SpriteAtlas::SpriteAtlas(dimension width_, dimension height_, float pixelRatio_)
    : width(width_),
      height(height_),
      pixelWidth(std::ceil(width * pixelRatio_)),
      pixelHeight(std::ceil(height * pixelRatio_)),
      pixelRatio(pixelRatio_),
      observer(&nullObserver),
      bin(width_, height_),
      dirtyFlag(true) {
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
            if ((it->second->image.width != sprite->image.width || it->second->image.height != sprite->image.height)) {
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

Rect<SpriteAtlas::dimension> SpriteAtlas::allocateImage(const SpriteImage& spriteImage) {

    const uint16_t pixel_width = std::ceil(spriteImage.image.width / pixelRatio);
    const uint16_t pixel_height = std::ceil(spriteImage.image.height / pixelRatio);

    // Increase to next number divisible by 4, but at least 1.
    // This is so we can scale down the texture coordinates and pack them
    // into 2 bytes rather than 4 bytes.
    const uint16_t pack_width = (pixel_width + 1) + (4 - (pixel_width + 1) % 4);
    const uint16_t pack_height = (pixel_height + 1) + (4 - (pixel_height + 1) % 4);

    // We have to allocate a new area in the bin, and store an empty image in it.
    // Add a 1px border around every image.
    Rect<dimension> rect = bin.allocate(pack_width, pack_height);
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

    Rect<dimension> rect = allocateImage(*sprite);
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
        {{ float(rect.x + padding)     / width, float(rect.y + padding)     / height }},
        {{ float(rect.x + padding + w) / width, float(rect.y + padding + h) / height }}
    };
}

void copyBitmap(const uint32_t *src, const uint32_t srcStride, const uint32_t srcX, const uint32_t srcY,
        uint32_t *const dst, const uint32_t dstStride, const uint32_t dstX, const uint32_t dstY, int dstSize,
        const int width, const int height, const SpritePatternMode mode) {

    int srcI = srcY * srcStride + srcX;
    int dstI = dstY * dstStride + dstX;
    int x, y;

    if (mode == SpritePatternMode::Repeating) {
        // add 1 pixel wrapped padding on each side of the image
        dstI -= dstStride;
        for (y = -1; y <= height; y++, srcI = ((y + height) % height + srcY) * srcStride + srcX, dstI += dstStride) {
            for (x = -1; x <= width; x++) {
                const int dstIndex = (dstI + x + dstSize) % dstSize;
                dst[dstIndex] = src[srcI + ((x + width) % width)];
            }
        }

    } else {
        for (y = 0; y < height; y++, srcI += srcStride, dstI += dstStride) {
            for (x = 0; x < width; x++) {
                const int dstIndex = (dstI + x + dstSize) % dstSize;
                dst[dstIndex] = src[srcI + x];
            }
        }
    }
}

void SpriteAtlas::copy(const Holder& holder, const SpritePatternMode mode) {
    if (!data) {
        data = std::make_unique<uint32_t[]>(pixelWidth * pixelHeight);
        std::fill(data.get(), data.get() + pixelWidth * pixelHeight, 0);
    }

    const uint32_t *srcData = reinterpret_cast<const uint32_t *>(holder.spriteImage->image.data.get());
    if (!srcData) return;
    uint32_t *const dstData = data.get();

    const int padding = 1;

    copyBitmap(srcData, uint32_t(holder.spriteImage->image.width), 0, 0,
            dstData, pixelWidth, (holder.pos.x + padding) * pixelRatio, (holder.pos.y + padding) * pixelRatio, pixelWidth * pixelHeight,
            uint32_t(holder.spriteImage->image.width), uint32_t(holder.spriteImage->image.height), mode);

    dirtyFlag = true;
}

void SpriteAtlas::upload(gl::Context& context, gl::TextureUnit unit) {
    if (dirtyFlag) {
        bind(false, context, unit);
    }
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

void SpriteAtlas::bind(bool linear, gl::Context& context, gl::TextureUnit unit) {
    if (!data) {
        return; // Empty atlas
    }

    if (!texture) {
        texture = context.createTexture();
        context.activeTexture = unit;
        context.texture[unit] = *texture;
#if not MBGL_USE_GLES2
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0));
#endif // MBGL_USE_GLES2
        // We are using clamp to edge here since OpenGL ES doesn't allow GL_REPEAT on NPOT textures.
        // We use those when the pixelRatio isn't a power of two, e.g. on iPhone 6 Plus.
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        fullUploadRequired = true;
    } else if (context.texture[unit] != *texture) {
        context.activeTexture = unit;
        context.texture[unit] = *texture;
    }

    GLuint filter_val = linear ? GL_LINEAR : GL_NEAREST;
    if (filter_val != filter) {
        context.activeTexture = unit;
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_val));
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_val));
        filter = filter_val;
    }

    if (dirtyFlag) {
        std::lock_guard<std::recursive_mutex> lock(mtx);

        context.activeTexture = unit;
        if (fullUploadRequired) {
            MBGL_CHECK_ERROR(glTexImage2D(
                GL_TEXTURE_2D, // GLenum target
                0, // GLint level
                GL_RGBA, // GLint internalformat
                pixelWidth, // GLsizei width
                pixelHeight, // GLsizei height
                0, // GLint border
                GL_RGBA, // GLenum format
                GL_UNSIGNED_BYTE, // GLenum type
                data.get() // const GLvoid * data
            ));
            fullUploadRequired = false;
        } else {
            MBGL_CHECK_ERROR(glTexSubImage2D(
                GL_TEXTURE_2D, // GLenum target
                0, // GLint level
                0, // GLint xoffset
                0, // GLint yoffset
                pixelWidth, // GLsizei width
                pixelHeight, // GLsizei height
                GL_RGBA, // GLenum format
                GL_UNSIGNED_BYTE, // GLenum type
                data.get() // const GLvoid *pixels
            ));
        }

        dirtyFlag = false;

#if not MBGL_USE_GLES2
        // platform::showColorDebugImage("Sprite Atlas", reinterpret_cast<const char*>(data.get()),
        //                               pixelWidth, pixelHeight, pixelWidth, pixelHeight);
#endif // MBGL_USE_GLES2
    }
}

SpriteAtlas::Holder::Holder(std::shared_ptr<const SpriteImage> spriteImage_, Rect<dimension> pos_)
    : spriteImage(std::move(spriteImage_)), pos(std::move(pos_)) {
}

SpriteAtlas::Holder::Holder(Holder&& h) : spriteImage(std::move(h.spriteImage)), pos(h.pos) {
}

} // namespace mbgl
