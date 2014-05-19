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

Rect<SpriteAtlas::dimension> SpriteAtlas::getIcon(const int size, const std::string &name) {
    std::lock_guard<std::mutex> lock(mtx);

    std::map<std::string, Rect<dimension>> &size_index = index[size];

    auto rect_it = size_index.find(name);
    if (rect_it != size_index.end()) {
        return rect_it->second;
    }

    // We have to allocate a new area in the bin, and store an empty image in it.
    // Add a 1px border around every image.
    const dimension pack_size = size + 2 * buffer;

    Rect<dimension> rect = bin.allocate(pack_size, pack_size);
    if (rect.w == 0) {
        fprintf(stderr, "sprite atlas bitmap overflow");
        return rect;
    }

    size_index.emplace(name, rect);

    allocate();


    // Draw an antialiased circle.
    const int img_size = size * pixelRatio;
    const int img_offset_x = (rect.x + buffer) * pixelRatio;
    const int img_offset_y = (rect.y + buffer) * pixelRatio;

    uint32_t *sprite_img = reinterpret_cast<uint32_t *>(data);
    const float blur = 1.0f / size / pixelRatio;

    const uint8_t r = 0x7F;
    const uint8_t g = 0x7F;
    const uint8_t b = 0x7F;

    const int sprite_stride = width * pixelRatio;
    for (int y = 0; y < img_size; y++) {
        const int img_y = (img_offset_y + y) * sprite_stride + img_offset_x;
        for (int x = 0; x < img_size; x++) {

            const float dist = util::length(float(x) / img_size - 0.5f, float(y) / img_size - 0.5f);
            const float t = util::smoothstep(0.5f, 0.5f - blur, dist);
            const uint8_t alpha = t * 255;

            uint32_t color = (uint32_t(r * t) << 0) |
                             (uint32_t(g * t) << 8) |
                             (uint32_t(b * t) << 16) |
                             (uint32_t(alpha) << 24);
            sprite_img[img_y + x] = color;
        }
    }

    uninitialized.emplace(size, name);

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

Rect<SpriteAtlas::dimension> SpriteAtlas::setIcon(const int size, const std::string &name, const std::string &icon) {
    Rect<dimension> rect = getIcon(size, name);

    // Copy the bitmap
    const int img_size = size * pixelRatio;
    const int img_offset_x = (rect.x + buffer) * pixelRatio;
    const int img_offset_y = (rect.y + buffer) * pixelRatio;

    if (std::pow(size * pixelRatio, 2) * 4 /* rgba */ != icon.size()) { fprintf(stderr, "mismatched icon buffer size!"); }

    const uint32_t *icon_img = reinterpret_cast<const uint32_t *>(icon.data());
    uint32_t *sprite_img = reinterpret_cast<uint32_t *>(data);

    const int sprite_stride = width * pixelRatio;
    const int icon_stride = size * pixelRatio;
    for (size_t y = 0; y < img_size; y++) {
        const int img_y = (img_offset_y + y) * sprite_stride + img_offset_x;
        const int icon_y = y * icon_stride;
        for (size_t x = 0; x < img_size; x++) {
            sprite_img[img_y + x] = icon_img[icon_y + x];
        }
    }

    dirty = true;

    return rect;
}

void SpriteAtlas::update(const Sprite &sprite) {
    if (!sprite.isLoaded()) return;

    SpriteAtlas &atlas = *this;
    std::erase_if(uninitialized, [&sprite, &atlas](const std::pair<int, std::string> &pair) {
        const int &size = pair.first;
        const std::string &name = pair.second;
        const SpritePosition &src = sprite.getSpritePosition(name + "-" + std::to_string(size));
        if (src.width == size && src.height == size && src.pixelRatio == atlas.pixelRatio) {
            const uint32_t *src_img = reinterpret_cast<const uint32_t *>(sprite.raster.getData());

            uint32_t *dst_img = reinterpret_cast<uint32_t *>(atlas.data);
            Rect<dimension> dst = atlas.getIcon(size, name);
            dst.x = (dst.x + buffer) * atlas.pixelRatio;
            dst.y = (dst.y + buffer) * atlas.pixelRatio;
            dst.w = (dst.w - 2 * buffer) * atlas.pixelRatio;
            dst.h = (dst.h - 2 * buffer) * atlas.pixelRatio;


            const int src_image_stride = sprite.raster.width;
            const int dst_image_stride = atlas.width * atlas.pixelRatio;
            const int src_stride = src.width * src.pixelRatio;

            for (int y = 0; y < dst.h; y++) {
                const int src_pos = (src.y * src.pixelRatio + y) * src_image_stride + src.x * src.pixelRatio;
                const int dst_pos = (dst.y + y) * dst_image_stride + dst.x;

                // TODO: this is crashing
                // memcpy(dst_img + dst_pos, src_img + src_pos, src_stride * sizeof(uint32_t));
            }


            return true;
        } else {
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

        platform::show_color_debug_image("Sprite Atlas", data, width, height, width * pixelRatio, height * pixelRatio);

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
