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
            const uint8_t *s = reinterpret_cast<const uint8_t *>(src + x);
            uint8_t *d = reinterpret_cast<uint8_t *>(dst + x);

            // Premultiply the bitmap.
            // Note: We don't need to clamp the component values to 0..255, since
            // the source value is already 0..255 and the operation means they will
            // stay within the range of 0..255 and won't overflow.
            const uint8_t a = s[3];
            d[0] = s[0] * a / 255;
            d[1] = s[1] * a / 255;
            d[2] = s[2] * a / 255;
            d[3] = a;
        }
    }
}

Rect<SpriteAtlas::dimension> SpriteAtlas::allocateImage(size_t pixel_width, size_t pixel_height) {
    // We have to allocate a new area in the bin, and store an empty image in it.
    // Add a 1px border around every image.
    Rect<dimension> rect = bin.allocate(pixel_width + 2 * buffer, pixel_height + 2 * buffer);
    if (rect.w == 0) {
        return rect;
    }

    rect.x += buffer;
    rect.y += buffer;
    rect.w -= 2 * buffer;
    rect.h -= 2 * buffer;

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
    // `repeating` indicates that the image will be used in a repeating pattern
    // repeating pattern images are assumed to have a 1px padding that mirrors the opposite edge
    // positions for repeating images are adjusted to exclude the edge
    Rect<dimension> rect = getImage(name);
    const int r = repeating ? 1 : 0;
    return SpriteAtlasPosition {
        {{ float(rect.w)           / pixelRatio, float(rect.h)            / pixelRatio }},
        {{ float(rect.x + r)            / width, float(rect.y + r)            / height }},
        {{ float(rect.x + rect.w - 2*r) / width, float(rect.y + rect.h - 2*r) / height }}
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
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        first = true;
    } else {
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    GLuint filter_val = linear ? GL_LINEAR : GL_NEAREST;
    if (filter_val != filter) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_val);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_val);
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
    }
};

SpriteAtlas::~SpriteAtlas() {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    glDeleteTextures(1, &texture);
    texture = 0;
    ::operator delete(data), data = nullptr;
}
