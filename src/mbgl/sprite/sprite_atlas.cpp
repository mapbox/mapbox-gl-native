#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/sprite/sprite_store.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/util/gl_object_store.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/scaling.hpp>
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

Rect<SpriteAtlas::dimension> SpriteAtlas::allocateImage(float src_width, float src_height) {

    const uint16_t pixel_width = std::ceil(src_width / pixelRatio);
    const uint16_t pixel_height = std::ceil(src_height / pixelRatio);

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

mapbox::util::optional<SpriteAtlasElement> SpriteAtlas::getImage(const std::string& name, const bool wrap) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    auto rect_it = images.find({ name, wrap });
    if (rect_it != images.end()) {
        return SpriteAtlasElement { rect_it->second.pos, rect_it->second.texture, rect_it->second.texture->pixelRatio / pixelRatio };
    }

    auto sprite = store.getSprite(name);
    if (!sprite) {
        return {};
    }

    Rect<dimension> rect = allocateImage(sprite->width * sprite->pixelRatio, sprite->height * sprite->pixelRatio);
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

mapbox::util::optional<SpriteAtlasPosition> SpriteAtlas::getPosition(const std::string& name, bool repeating) {
    std::lock_guard<std::recursive_mutex> lock(mtx);

    auto img = getImage(name, repeating);
    if (!img) {
        return {};
    }

    auto rect = (*img).pos;

    const float padding = 1;
    auto image = (*img).texture;

    const float w = image->width * (*img).relativePixelRatio;
    const float h = image->height * (*img).relativePixelRatio;

    return SpriteAtlasPosition {
        {{ float(image->width), float(image->height) }},
        {{ float(rect.x + padding)     / width, float(rect.y + padding)     / height }},
        {{ float(rect.x + padding + w) / width, float(rect.y + padding + h) / height }}
    };
}

void SpriteAtlas::copy(const Holder& holder, const bool wrap) {
    if (!data) {
        data = std::make_unique<uint32_t[]>(pixelWidth * pixelHeight);
        std::fill(data.get(), data.get() + pixelWidth * pixelHeight, 0);
    }

    const uint32_t *srcData = reinterpret_cast<const uint32_t *>(holder.texture->data.data());
    if (!srcData) return;
    const vec2<uint32_t> srcSize { holder.texture->pixelWidth, holder.texture->pixelHeight };
    const Rect<uint32_t> srcPos { 0, 0, srcSize.x, srcSize.y };
    const auto& dst = holder.pos;

    const int offset = 1;

    uint32_t *const dstData = data.get();
    const vec2<uint32_t> dstSize{ pixelWidth, pixelHeight };
    const Rect<uint32_t> dstPos{ static_cast<uint32_t>((offset + dst.x) * pixelRatio),
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
            holder.texture = spriteIterator->second;
            if (holder.texture != nullptr) {
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

SpriteAtlas::Holder::Holder(const std::shared_ptr<const SpriteImage>& texture_,
                            const Rect<dimension>& pos_)
    : texture(texture_), pos(pos_) {
}

SpriteAtlas::Holder::Holder(Holder&& h) : texture(std::move(h.texture)), pos(h.pos) {
}
