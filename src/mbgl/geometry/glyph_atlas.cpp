#include <mbgl/geometry/glyph_atlas.hpp>

#include <mbgl/text/font_stack.hpp>

#include <mbgl/platform/gl.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/platform/platform.hpp>

#include <cassert>
#include <algorithm>


using namespace mbgl;

GlyphAtlas::GlyphAtlas(uint16_t width_, uint16_t height_)
    : width(width_),
      height(height_),
      bin(width_, height_),
      data(std::make_unique<uint8_t[]>(width_ * height_)),
      dirty(true) {
}

void GlyphAtlas::addGlyphs(uintptr_t tileUID,
                           const std::u32string& text,
                           const std::string& stackName,
                           const FontStack& fontStack,
                           GlyphPositions& face)
{
    std::lock_guard<std::mutex> lock(mtx);

    const std::map<uint32_t, SDFGlyph>& sdfs = fontStack.getSDFs();

    for (uint32_t chr : text)
    {
        auto sdf_it = sdfs.find(chr);
        if (sdf_it == sdfs.end()) {
            continue;
        }

        const SDFGlyph& sdf = sdf_it->second;
        Rect<uint16_t> rect = addGlyph(tileUID, stackName, sdf);
        face.emplace(chr, Glyph{rect, sdf.metrics});
    }
}

Rect<uint16_t> GlyphAtlas::addGlyph(uintptr_t tileUID,
                                    const std::string& stackName,
                                    const SDFGlyph& glyph)
{
    // Use constant value for now.
    const uint8_t buffer = 3;

    std::map<uint32_t, GlyphValue>& face = index[stackName];
    std::map<uint32_t, GlyphValue>::iterator it = face.find(glyph.id);

    // The glyph is already in this texture.
    if (it != face.end()) {
        GlyphValue& value = it->second;
        value.ids.insert(tileUID);
        return value.rect;
    }

    // The glyph bitmap has zero width.
    if (!glyph.bitmap.size()) {
        return Rect<uint16_t>{ 0, 0, 0, 0 };
    }

    uint16_t buffered_width = glyph.metrics.width + buffer * 2;
    uint16_t buffered_height = glyph.metrics.height + buffer * 2;

    // Add a 1px border around every image.
    const uint16_t padding = 1;
    uint16_t pack_width = buffered_width + 2 * padding;
    uint16_t pack_height = buffered_height + 2 * padding;

    // Increase to next number divisible by 4, but at least 1.
    // This is so we can scale down the texture coordinates and pack them
    // into 2 bytes rather than 4 bytes.
    pack_width += (4 - pack_width % 4);
    pack_height += (4 - pack_height % 4);

    Rect<uint16_t> rect = bin.allocate(pack_width, pack_height);
    if (rect.w == 0) {
        Log::Error(Event::OpenGL, "glyph bitmap overflow");
        return rect;
    }

    assert(rect.x + rect.w <= width);
    assert(rect.y + rect.h <= height);

    face.emplace(glyph.id, GlyphValue { rect, tileUID });

    // Copy the bitmap
    const uint8_t* source = reinterpret_cast<const uint8_t*>(glyph.bitmap.data());
    for (uint32_t y = 0; y < buffered_height; y++) {
        uint32_t y1 = width * (rect.y + y + padding) + rect.x + padding;
        uint32_t y2 = buffered_width * y;
        for (uint32_t x = 0; x < buffered_width; x++) {
            data[y1 + x] = source[y2 + x];
        }
    }

    dirty = true;

    return rect;
}

void GlyphAtlas::removeGlyphs(uintptr_t tileUID) {
    std::lock_guard<std::mutex> lock(mtx);

    for (auto& faces : index) {
        std::map<uint32_t, GlyphValue>& face = faces.second;
        for (auto it = face.begin(); it != face.end(); /* we advance in the body */) {
            GlyphValue& value = it->second;
            value.ids.erase(tileUID);

            if (!value.ids.size()) {
                const Rect<uint16_t>& rect = value.rect;

                // Clear out the bitmap.
                uint8_t *target = data.get();
                for (uint32_t y = 0; y < rect.h; y++) {
                    uint32_t y1 = width * (rect.y + y) + rect.x;
                    for (uint32_t x = 0; x < rect.w; x++) {
                        target[y1 + x] = 0;
                    }
                }

                bin.release(rect);

                // Make sure to post-increment the iterator: This will return the
                // current iterator, but will go to the next position before we
                // erase the element from the map. That way, the iterator stays
                // valid.
                face.erase(it++);
            } else {
                ++it;
            }
        }
    }
}

void GlyphAtlas::upload() {
    if (dirty) {
        const bool first = !texture;
        bind();

        std::lock_guard<std::mutex> lock(mtx);

        if (first) {
            MBGL_CHECK_ERROR(glTexImage2D(
                GL_TEXTURE_2D, // GLenum target
                0, // GLint level
                GL_ALPHA, // GLint internalformat
                width, // GLsizei width
                height, // GLsizei height
                0, // GLint border
                GL_ALPHA, // GLenum format
                GL_UNSIGNED_BYTE, // GLenum type
                data.get() // const GLvoid* data
            ));
        } else {
            MBGL_CHECK_ERROR(glTexSubImage2D(
                GL_TEXTURE_2D, // GLenum target
                0, // GLint level
                0, // GLint xoffset
                0, // GLint yoffset
                width, // GLsizei width
                height, // GLsizei height
                GL_ALPHA, // GLenum format
                GL_UNSIGNED_BYTE, // GLenum type
                data.get() // const GLvoid* data
            ));
        }

        dirty = false;

#if defined(DEBUG)
        // platform::showDebugImage("Glyph Atlas", data.get(), width, height);
#endif
    }
}

void GlyphAtlas::bind() {
    if (!texture) {
        MBGL_CHECK_ERROR(glGenTextures(1, &texture));
        MBGL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, texture));
#ifndef GL_ES_VERSION_2_0
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0));
#endif
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        MBGL_CHECK_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    } else {
        MBGL_CHECK_ERROR(glBindTexture(GL_TEXTURE_2D, texture));
    }
};
