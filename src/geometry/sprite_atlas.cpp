#include <llmr/geometry/sprite_atlas.hpp>
#include <llmr/platform/gl.hpp>
#include <llmr/platform/platform.hpp>
#include <llmr/util/math.hpp>
#include <llmr/util/std.hpp>

#include <llmr/style/sprite.hpp>

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
        char *old_data = data;

        data = nullptr;
        allocate();

        dimension w = static_cast<dimension>(width * newRatio);
        dimension h = static_cast<dimension>(height * newRatio);
        float s = std::pow(oldRatio / newRatio, 2);

        // Basic image scaling. TODO: Replace this with better image scaling.
        uint32_t *img_new = reinterpret_cast<uint32_t *>(data);
        uint32_t *img_old = reinterpret_cast<uint32_t *>(old_data);
        for (size_t i = 0, length = w * h; i < length; i++) {
            img_new[i] = img_old[static_cast<size_t>(s * i)];
        }

        free(old_data);
        dirty = true;
    }

    return dirty;
}

void copy_bitmap(const uint32_t *src, const int src_stride, const int src_x, const int src_y,
                uint32_t *dst, const int dst_stride, const int dst_x, const int dst_y,
                const int width, const int height) {
    const int stride = width * sizeof(uint32_t);
    src += src_y * src_stride + src_x;
    dst += dst_y * dst_stride + dst_x;
    for (int y = 0; y < height; y++, src += src_stride, dst += dst_stride) {
        memcpy(dst, src, stride);
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
        fprintf(stderr, "sprite atlas bitmap overflow\n");
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
        fprintf(stderr, "sprite atlas bitmap overflow\n");
        return rect;
    }

    images.emplace(name, rect);

    copy(rect, pos, sprite);

    dirty = true;

    return rect;
}

void SpriteAtlas::allocate() {
    if (!data) {
        dimension w = static_cast<dimension>(width * pixelRatio);
        dimension h = static_cast<dimension>(height * pixelRatio);
        data = (char *)calloc(w * h, sizeof(uint32_t));
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
}


void SpriteAtlas::update(const Sprite &sprite) {
    if (!sprite.isLoaded()) return;

    SpriteAtlas &atlas = *this;
    std::erase_if(uninitialized, [&sprite, &atlas](const std::string &name) {
        Rect<dimension> dst = atlas.getImage(name, sprite);
        const SpritePosition &src = sprite.getSpritePosition(name);
        if (src.width == dst.w * atlas.pixelRatio && src.height == dst.h * atlas.pixelRatio && src.pixelRatio == atlas.pixelRatio) {
            atlas.copy(dst, src, sprite);
            return true;
        } else {
            fprintf(stderr, "sprite icon dimension mismatch\n");
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

        // platform::show_color_debug_image("Sprite Atlas", data, width, height, width * pixelRatio, height * pixelRatio);

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
