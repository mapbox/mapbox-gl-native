#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/constants.hpp>

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
        uint32_t *old_data = data;

        data = nullptr;
        allocate();

        const int old_w = width * oldRatio;
        const int old_h = height * oldRatio;
        const int new_w = width * newRatio;
        const int new_h = height * newRatio;

        // Basic image scaling. TODO: Replace this with better image scaling.
        uint32_t *img_new = reinterpret_cast<uint32_t *>(data);
        const uint32_t *img_old = reinterpret_cast<const uint32_t *>(old_data);

        for (int y = 0; y < new_h; y++) {
            const int old_yoffset = ((y * old_h) / new_h) * old_w;
            const int new_yoffset = y * new_w;
            for (int x = 0; x < new_w; x++) {
                const int old_x = (x * old_w) / new_w;
                img_new[new_yoffset + x] = img_old[old_yoffset + old_x];
            }
        }

        ::operator delete(old_data);
        dirty = true;

        // Mark all sprite images as in need of update
        for (const auto &pair : images) {
            uninitialized.emplace(pair.first);
        }
    }

    return dirty;
}

void copy_bitmap(const uint32_t *src, const int src_stride, const int src_x, const int src_y,
                uint32_t *dst, const int dst_stride, const int dst_x, const int dst_y,
                const int width, const int height) {
    src += src_y * src_stride + src_x;
    dst += dst_y * dst_stride + dst_x;
    for (int y = 0; y < height; y++, src += src_stride, dst += dst_stride) {
        for (int x = 0; x < width; x++) {
            dst[x] = src[x];
        }
    }
}

Rect<SpriteAtlas::dimension> SpriteAtlas::allocateImage(const size_t pixel_width, const size_t pixel_height) {
    // Increase to next number divisible by 4, but at least 1.
    // This is so we can scale down the texture coordinates and pack them
    // into 2 bytes rather than 4 bytes.
    const uint16_t pack_width = pixel_width + (4 - pixel_width % 4);
    const uint16_t pack_height = pixel_height + (4 - pixel_width % 4);

    // We have to allocate a new area in the bin, and store an empty image in it.
    // Add a 1px border around every image.
    Rect<dimension> rect = bin.allocate(pack_width, pack_height);
    if (rect.w == 0) {
        return rect;
    }

    rect.x += buffer;
    rect.y += buffer;

    return rect;
}

Rect<SpriteAtlas::dimension> SpriteAtlas::getImage(const std::string& name) {
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
            fprintf(stderr, "[WARNING] sprite atlas bitmap overflow\n");
        }
        return rect;
    }

    images.emplace(name, rect);

    copy(rect, pos);

    return rect;
}

SpriteAtlasPosition SpriteAtlas::getPosition(const std::string& name, bool repeating) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    Rect<dimension> rect = getImage(name);
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

    return SpriteAtlasPosition {
        {{ float(rect.w), float(rect.h) }},
        {{ float(rect.x)          / width, float(rect.y)          / height }},
        {{ float(rect.x + rect.w) / width, float(rect.y + rect.h) / height }}
    };
}

void SpriteAtlas::allocate() {
    if (!data) {
        dimension w = static_cast<dimension>(width * pixelRatio);
        dimension h = static_cast<dimension>(height * pixelRatio);
        data = static_cast<uint32_t*>(::operator new(w * h * sizeof(uint32_t)));
        std::fill(data, data + w * h, 0);
    }
}

void SpriteAtlas::copy(const Rect<dimension>& dst, const SpritePosition& src) {
    if (!sprite->raster) return;
    const uint32_t *src_img = reinterpret_cast<const uint32_t *>(sprite->raster->getData());
    if (!src_img) return;
    allocate();
    uint32_t *dst_img = reinterpret_cast<uint32_t *>(data);

    copy_bitmap(
        /* source buffer */  src_img,
        /* source stride */  sprite->raster->getWidth(),
        /* source x */       src.x,
        /* source y */       src.y,
        /* dest buffer */    dst_img,
        /* dest stride */    width * pixelRatio,
        /* dest x */         dst.x * pixelRatio,
        /* dest y */         dst.y * pixelRatio,
        /* icon dimension */ src.width,
        /* icon dimension */ src.height
    );

    dirty = true;
}

void SpriteAtlas::setSprite(util::ptr<Sprite> sprite_) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    sprite = sprite_;

    if (!sprite->isLoaded()) return;

    util::erase_if(uninitialized, [this](const std::string &name) {
        Rect<dimension> dst = getImage(name);
        const SpritePosition& src = sprite->getSpritePosition(name);
        if (!src) {
            if (debug::spriteWarnings) {
                fprintf(stderr, "[WARNING] sprite doesn't have image with name '%s'\n", name.c_str());
            }
            return true;
        }

        if (src.width == dst.w * pixelRatio && src.height == dst.h * pixelRatio && src.pixelRatio == pixelRatio) {
            copy(dst, src);
            return true;
        } else {
            if (debug::spriteWarnings) {
                fprintf(stderr, "[WARNING] sprite icon dimension mismatch\n");
            }
            return false;
        }
    });
}

void SpriteAtlas::bind(bool linear) {
    bool first = false;
    if (!texture) {
        MBGL_CHECK_ERROR(glGenTextures(1, &texture));
        MBGL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, texture));
#ifndef GL_ES_VERSION_2_0
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0));
#endif
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        first = true;
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

        if (first) {
            glTexImage2D(
                GL_TEXTURE_2D, // GLenum target
                0, // GLint level
                GL_RGBA, // GLint internalformat
                width * pixelRatio, // GLsizei width
                height * pixelRatio, // GLsizei height
                0, // GLint border
                GL_RGBA, // GLenum format
                GL_UNSIGNED_BYTE, // GLenum type
                data // const GLvoid * data
            );
        } else {
            glTexSubImage2D(
                GL_TEXTURE_2D, // GLenum target
                0, // GLint level
                0, // GLint xoffset
                0, // GLint yoffset
                width * pixelRatio, // GLsizei width
                height * pixelRatio, // GLsizei height
                GL_RGBA, // GLenum format
                GL_UNSIGNED_BYTE, // GLenum type
                data // const GLvoid *pixels
            );
        }

        dirty = false;

        // platform::show_color_debug_image("Sprite Atlas", reinterpret_cast<const char *>(data), width, height, width * pixelRatio, height * pixelRatio);
    }
};

SpriteAtlas::~SpriteAtlas() {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    MBGL_CHECK_ERROR(glDeleteTextures(1, &texture));
    texture = 0;
    ::operator delete(data), data = nullptr;
}
