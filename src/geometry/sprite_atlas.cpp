#include <llmr/geometry/sprite_atlas.hpp>
#include <llmr/platform/gl.hpp>
#include <llmr/platform/platform.hpp>
#include <llmr/util/math.hpp>
#include <llmr/util/std.hpp>
#include <llmr/util/constants.hpp>

#include <llmr/map/sprite.hpp>

#include <cassert>
#include <cmath>
#include <algorithm>


using namespace llmr;

SpriteAtlas::SpriteAtlas(dimension width, dimension height)
    : width(width),
      height(height),
      bin(width, height),
      dirty(true) {
}

bool SpriteAtlas::resize(const float newRatio) {
    if (pixelRatio == newRatio) return false;

    std::lock_guard<std::mutex> lock(mtx);

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

        free(old_data);
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

void draw_circle(uint32_t *dst, const int dst_stride, const int dst_x, const int dst_y,
                const int width, const int height, const float blur,
                const uint8_t r = 0xFF, const uint8_t g = 0xFF, const uint8_t b = 0xFF) {
    const int sprite_stride = dst_stride;
    const int radius = util::min(width, height);
    for (int y = 0; y < height; y++) {
        const int img_y = (dst_y + y) * sprite_stride + dst_x;
        for (int x = 0; x < height; x++) {
            const float dist = util::length(float(x) / radius - 0.5f, float(y) / radius - 0.5f);
            const float t = util::smoothstep(0.5f, 0.5f - blur, dist);
            const uint8_t alpha = t * 255;

            uint32_t color = (uint32_t(r * t) << 0) |
                             (uint32_t(g * t) << 8) |
                             (uint32_t(b * t) << 16) |
                             (uint32_t(alpha) << 24);
            dst[img_y + x] = color;
        }
    }
}

Rect<SpriteAtlas::dimension> SpriteAtlas::allocateImage(size_t width, size_t height) {
    // We have to allocate a new area in the bin, and store an empty image in it.
    // Add a 1px border around every image.
    Rect<dimension> rect = bin.allocate(width + 2 * buffer, height + 2 * buffer);
    if (rect.w == 0) {
        return rect;
    }

    rect.x += buffer;
    rect.y += buffer;
    rect.w -= 2 * buffer;
    rect.h -= 2 * buffer;

    return rect;
}

Rect<SpriteAtlas::dimension> SpriteAtlas::getIcon(const int size, const std::string &name) {
    std::lock_guard<std::mutex> lock(mtx);

    const std::string id = name + "-" + std::to_string(size);

    auto rect_it = images.find(id);
    if (rect_it != images.end()) {
        return rect_it->second;
    }

    Rect<dimension> rect = allocateImage(size, size);
    if (rect.w == 0) {
        if (debug::spriteWarnings) {
            fprintf(stderr, "[WARNING] sprite atlas bitmap overflow\n");
        }
        return rect;
    }

    images.emplace(id, rect);

    allocate();

    // Draw an antialiased circle.
    draw_circle(
        reinterpret_cast<uint32_t *>(data),
        width * pixelRatio,
        rect.x * pixelRatio,
        rect.y * pixelRatio,
        size * pixelRatio,
        size * pixelRatio,
        1.0f / size / pixelRatio
    );

    uninitialized.emplace(id);

    dirty = true;

    return rect;
}

Rect<SpriteAtlas::dimension> SpriteAtlas::getImage(const std::string &name, const Sprite &sprite) {
    std::lock_guard<std::mutex> lock(mtx);

    auto rect_it = images.find(name);
    if (rect_it != images.end()) {
        return rect_it->second;
    }

    const SpritePosition &pos = sprite.getSpritePosition(name);
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

    copy(rect, pos, sprite);

    return rect;
}

void SpriteAtlas::allocate() {
    if (!data) {
        dimension w = static_cast<dimension>(width * pixelRatio);
        dimension h = static_cast<dimension>(height * pixelRatio);
        data = (uint32_t *)calloc(w * h, sizeof(uint32_t));
    }
}

void SpriteAtlas::copy(const Rect<dimension> &dst, const SpritePosition &src, const Sprite &sprite) {
    if (!sprite.raster) return;
    const uint32_t *src_img = reinterpret_cast<const uint32_t *>(sprite.raster->getData());
    allocate();
    uint32_t *dst_img = reinterpret_cast<uint32_t *>(data);

    copy_bitmap(
        /* source buffer */  src_img,
        /* source stride */  sprite.raster->getWidth(),
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


void SpriteAtlas::update(const Sprite &sprite) {
    if (!sprite.isLoaded()) return;

    SpriteAtlas &atlas = *this;
    util::erase_if(uninitialized, [&sprite, &atlas](const std::string &name) {
        Rect<dimension> dst = atlas.getImage(name, sprite);
        const SpritePosition &src = sprite.getSpritePosition(name);
        if (!src) {
            if (debug::spriteWarnings) {
                fprintf(stderr, "[WARNING] sprite doesn't have image with name '%s'\n", name.c_str());
            }
            return true;
        }

        if (src.width == dst.w * atlas.pixelRatio && src.height == dst.h * atlas.pixelRatio && src.pixelRatio == atlas.pixelRatio) {
            atlas.copy(dst, src, sprite);
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
    if (!texture) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
        std::lock_guard<std::mutex> lock(mtx);
        allocate();
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

#if defined(DEBUG)
        // platform::show_color_debug_image("Sprite Atlas", data, width, height, width * pixelRatio, height * pixelRatio);
#endif
        dirty = false;
    }
};

SpriteAtlas::~SpriteAtlas() {
    std::lock_guard<std::mutex> lock(mtx);
    if (data) {
        free(data);
        data = nullptr;
    }
}
