#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/sprite/sprite_store.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/util/gl_object_store.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/thread_context.hpp>

#include <cassert>
#include <cmath>
#include <algorithm>

using namespace mbgl;

SpriteAtlas::SpriteAtlas(dimension width_, dimension height_, float pixelRatio_, SpriteStore& store_)
    : width(width_),
      height(height_),
      pixelWidth(std::ceil(width * pixelRatio_)),
      pixelHeight(std::ceil(height * pixelRatio_)),
      pixelRatio(pixelRatio_),
      store(store_),
      bin(width_, height_),
      dirty(true) {
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

optional<SpriteAtlasElement> SpriteAtlas::getImage(const std::string& name, const bool wrap) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    auto rect_it = images.find({ name, wrap });
    if (rect_it != images.end()) {
        return SpriteAtlasElement { rect_it->second.pos, rect_it->second.spriteImage, rect_it->second.spriteImage->pixelRatio / pixelRatio };
    }

    auto sprite = store.getSprite(name);
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

    const Holder& holder = images.emplace(Key{ name, wrap }, Holder{ sprite, rect }).first->second;
    copy(holder, wrap);

    return SpriteAtlasElement { rect, sprite, sprite->pixelRatio / pixelRatio };
}

optional<SpriteAtlasPosition> SpriteAtlas::getPosition(const std::string& name, bool repeating) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    auto img = getImage(name, repeating);
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
        const int width, const int height, const bool wrap) {

    int srcI = srcY * srcStride + srcX;
    int dstI = dstY * dstStride + dstX;
    int x, y;

    if (wrap) {
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

void SpriteAtlas::copy(const Holder& holder, const bool wrap) {
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
            uint32_t(holder.spriteImage->image.width), uint32_t(holder.spriteImage->image.height), wrap);

    dirty = true;
}

void SpriteAtlas::upload() {
    if (dirty) {
        bind();
    }
}

void SpriteAtlas::updateDirty() {
    auto dirtySprites = store.getDirty();
    if (dirtySprites.empty()) {
        return;
    }

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
}

void SpriteAtlas::bind(bool linear) {
    if (!data) {
        return; // Empty atlas
    }

    if (!texture) {
        MBGL_CHECK_ERROR(glGenTextures(1, &texture));
        MBGL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, texture));
#ifndef GL_ES_VERSION_2_0
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0));
#endif
        // We are using clamp to edge here since OpenGL ES doesn't allow GL_REPEAT on NPOT textures.
        // We use those when the pixelRatio isn't a power of two, e.g. on iPhone 6 Plus.
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        fullUploadRequired = true;
    } else {
        MBGL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, texture));
    }

    GLuint filter_val = linear ? GL_LINEAR : GL_NEAREST;
    if (filter_val != filter) {
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_val));
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_val));
        filter = filter_val;
    }

    if (dirty) {
        std::lock_guard<std::recursive_mutex> lock(mtx);

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

        dirty = false;

#ifndef GL_ES_VERSION_2_0
        // platform::showColorDebugImage("Sprite Atlas", reinterpret_cast<const char*>(data.get()),
        //                               pixelWidth, pixelHeight, pixelWidth, pixelHeight);
#endif
    }
};

SpriteAtlas::~SpriteAtlas() {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    if (texture) {
        mbgl::util::ThreadContext::getGLObjectStore()->abandonTexture(texture);
        texture = 0;
    }
}

SpriteAtlas::Holder::Holder(const std::shared_ptr<const SpriteImage>& spriteImage_,
                            const Rect<dimension>& pos_)
    : spriteImage(spriteImage_), pos(pos_) {
}

SpriteAtlas::Holder::Holder(Holder&& h) : spriteImage(std::move(h.spriteImage)), pos(h.pos) {
}
