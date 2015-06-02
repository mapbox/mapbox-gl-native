#include <mbgl/map/environment.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/scaling.hpp>

#include <mbgl/map/sprite.hpp>

#include <cassert>
#include <cmath>
#include <algorithm>


using namespace mbgl;

SpriteAtlas::SpriteAtlas(dimension width_, dimension height_)
    : width(width_),
      height(height_),
      bin(width_, height_),
      dirty(true) {
}

bool SpriteAtlas::resize(const float newRatio) {
    if (pixelRatio == newRatio) return false;

    std::lock_guard<std::recursive_mutex> lock(mtx);

    const float oldRatio = pixelRatio;
    pixelRatio = newRatio;

    if (data) {
        const auto oldData = std::move(data);
        allocate();

        const int old_w = width * oldRatio;
        const int old_h = height * oldRatio;
        const int new_w = width * newRatio;
        const int new_h = height * newRatio;

        // Basic image scaling. TODO: Replace this with better image scaling.
        for (int y = 0; y < new_h; y++) {
            const int old_yoffset = ((y * old_h) / new_h) * old_w;
            const int new_yoffset = y * new_w;
            for (int x = 0; x < new_w; x++) {
                const int old_x = (x * old_w) / new_w;
                data[new_yoffset + x] = oldData[old_yoffset + old_x];
            }
        }

        dirty = true;
        fullUploadRequired = true;

        // Mark all sprite images as in need of update
        for (const auto &pair : images) {
            uninitialized.emplace(pair.first);
        }
    }

    return dirty;
}

Rect<SpriteAtlas::dimension> SpriteAtlas::allocateImage(const size_t pixel_width, const size_t pixel_height) {
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

    rect.originalW = pixel_width;
    rect.originalH = pixel_height;

    return rect;
}

Rect<SpriteAtlas::dimension> SpriteAtlas::getImage(const std::string& name, const bool wrap) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    auto rect_it = images.find(name);
    if (rect_it != images.end()) {
        return rect_it->second;
    }

    const SpritePosition &pos = sprite->getSpritePosition(name);
    if (!pos.width || !pos.height) {
        return Rect<dimension> { 0, 0, 0, 0 };
    }

    Rect<dimension> rect = allocateImage(pos.width / pos.pixelRatio, pos.height / pos.pixelRatio);
    if (rect.w == 0) {
        if (debug::spriteWarnings) {
            Log::Warning(Event::Sprite, "sprite atlas bitmap overflow");
        }
        return rect;
    }

    images.emplace(name, rect);

    copy(rect, pos, wrap);

    return rect;
}

SpriteAtlasPosition SpriteAtlas::getPosition(const std::string& name, bool repeating) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    Rect<dimension> rect = getImage(name, repeating);
    if (repeating) {
        // When the image is repeating, get the correct position of the image, rather than the
        // one rounded up to 4 pixels.
        const SpritePosition &pos = sprite->getSpritePosition(name);
        if (!pos.width || !pos.height) {
            return SpriteAtlasPosition {};
        }

        rect.w = pos.width / pos.pixelRatio;
        rect.h = pos.height / pos.pixelRatio;
    }

    const float padding = 1;

    return SpriteAtlasPosition {
        {{ float(rect.w), float(rect.h) }},
        {{ float(rect.x + padding)          / width, float(rect.y + padding)          / height }},
        {{ float(rect.x + padding + rect.w) / width, float(rect.y + padding + rect.h) / height }}
    };
}

void SpriteAtlas::allocate() {
    if (!data) {
        dimension w = static_cast<dimension>(width * pixelRatio);
        dimension h = static_cast<dimension>(height * pixelRatio);
        data = std::make_unique<uint32_t[]>(w * h);
        std::fill(data.get(), data.get() + w * h, 0);
    }
}

void SpriteAtlas::copy(const Rect<dimension>& dst, const SpritePosition& src, const bool wrap) {
    if (!sprite->raster) return;

    const uint32_t *srcData = reinterpret_cast<const uint32_t *>(sprite->raster->getData());
    if (!srcData) return;
    const vec2<uint32_t> srcSize { sprite->raster->getWidth(), sprite->raster->getHeight() };
    const Rect<uint32_t> srcPos { src.x, src.y, src.width, src.height };

    const int offset = 1;

    allocate();
    uint32_t *const dstData = data.get();
    const vec2<uint32_t> dstSize { static_cast<unsigned int>(width * pixelRatio),
                                   static_cast<unsigned int>(height * pixelRatio) };
    const Rect<uint32_t> dstPos { static_cast<uint32_t>((offset + dst.x) * pixelRatio),
                                  static_cast<uint32_t>((offset + dst.y) * pixelRatio),
                                  static_cast<uint32_t>(dst.originalW * pixelRatio),
                                  static_cast<uint32_t>(dst.originalH * pixelRatio) };

    util::bilinearScale(srcData, srcSize, srcPos, dstData, dstSize, dstPos, wrap);

    // Add borders around the copied image if required.
    if (wrap) {
        // We're copying from the same image so we don't have to scale again.
        const uint32_t border = 1;
        const uint32_t borderX = dstPos.x != 0 ? border : 0;
        const uint32_t borderY = dstPos.y != 0 ? border : 0;

        // Left border
        util::nearestNeighborScale(
            dstData, dstSize, { dstPos.x + dstPos.w - borderX, dstPos.y, borderX, dstPos.h },
            dstData, dstSize, { dstPos.x - borderX, dstPos.y, borderX, dstPos.h });

        // Right border
        util::nearestNeighborScale(dstData, dstSize, { dstPos.x, dstPos.y, border, dstPos.h },
                                   dstData, dstSize,
                                   { dstPos.x + dstPos.w, dstPos.y, border, dstPos.h });

        // Top border
        util::nearestNeighborScale(
            dstData, dstSize, { dstPos.x - borderX, dstPos.y + dstPos.h - borderY,
                                dstPos.w + border + borderX, borderY },
            dstData, dstSize,
            { dstPos.x - borderX, dstPos.y - borderY, dstPos.w + 2 * borderX, borderY });

        // Bottom border
        util::nearestNeighborScale(
            dstData, dstSize, { dstPos.x - borderX, dstPos.y, dstPos.w + 2 * borderX, border },
            dstData, dstSize,
            { dstPos.x - borderX, dstPos.y + dstPos.h, dstPos.w + border + borderX, border });
    }

    dirty = true;
}

void SpriteAtlas::setSprite(util::ptr<Sprite> sprite_) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    sprite = sprite_;

    if (!sprite->isLoaded()) return;

    util::erase_if(uninitialized, [this](const std::string &name) {
        Rect<dimension> dst = getImage(name, false);
        const SpritePosition& src = sprite->getSpritePosition(name);
        if (!src) {
            if (debug::spriteWarnings) {
                Log::Warning(Event::Sprite, "sprite doesn't have image with name '%s'", name.c_str());
            }
            return true;
        }

        if (src.width == dst.w * pixelRatio && src.height == dst.h * pixelRatio && src.pixelRatio == pixelRatio) {
            copy(dst, src, false);
            return true;
        } else {
            if (debug::spriteWarnings) {
                Log::Warning(Event::Sprite, "sprite icon dimension mismatch");
            }
            return false;
        }
    });
}

void SpriteAtlas::upload() {
    if (dirty) {
        bind();
    }
}

void SpriteAtlas::bind(bool linear) {
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
        allocate();

        if (fullUploadRequired) {
            MBGL_CHECK_ERROR(glTexImage2D(
                GL_TEXTURE_2D, // GLenum target
                0, // GLint level
                GL_RGBA, // GLint internalformat
                width * pixelRatio, // GLsizei width
                height * pixelRatio, // GLsizei height
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
                width * pixelRatio, // GLsizei width
                height * pixelRatio, // GLsizei height
                GL_RGBA, // GLenum format
                GL_UNSIGNED_BYTE, // GLenum type
                data.get() // const GLvoid *pixels
            ));
        }

        dirty = false;

#ifndef GL_ES_VERSION_2_0
        // platform::showColorDebugImage("Sprite Atlas", reinterpret_cast<const char *>(data), width * pixelRatio, height * pixelRatio, width * pixelRatio, height * pixelRatio);
#endif
    }
};

SpriteAtlas::~SpriteAtlas() {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    if (texture) {
        Environment::Get().abandonTexture(texture);
        texture = 0;
    }
}
