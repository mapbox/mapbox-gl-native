#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/map/vector_tile.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/platform/platform.hpp>

#include <cassert>
#include <algorithm>


using namespace mbgl;

GlyphAtlas::GlyphAtlas(uint16_t width, uint16_t height)
    : width(width),
      height(height),
      bin(width, height),
      data(new char[width *height]),
      dirty(true) {
}

GlyphAtlas::~GlyphAtlas() {
    delete[] data;
}

Rect<uint16_t> GlyphAtlas::addGlyph(uint64_t tile_id, const std::string& face_name,
                                    const SDFGlyph& glyph) {
    std::lock_guard<std::mutex> lock(mtx);

    // Use constant value for now.
    const uint8_t buffer = 3;

    std::map<uint32_t, GlyphValue>& face = index[face_name];
    std::map<uint32_t, GlyphValue>::iterator it = face.find(glyph.id);

    // The glyph is already in this texture.
    if (it != face.end()) {
        GlyphValue& value = it->second;
        value.ids.insert(tile_id);
        return value.rect;
    }

    // The glyph bitmap has zero width.
    if (!glyph.bitmap.size()) {
        return Rect<uint16_t>{ 0, 0, 0, 0 };
    }

    uint16_t buffered_width = glyph.metrics.width + buffer * 2;
    uint16_t buffered_height = glyph.metrics.height + buffer * 2;

    // Add a 1px border around every image.
    uint16_t pack_width = buffered_width;
    uint16_t pack_height = buffered_height;

    // Increase to next number divisible by 4, but at least 1.
    // This is so we can scale down the texture coordinates and pack them
    // into 2 bytes rather than 4 bytes.
    pack_width += (4 - pack_width % 4);
    pack_height += (4 - pack_height % 4);

    Rect<uint16_t> rect = bin.allocate(pack_width, pack_height);
    if (rect.w == 0) {
        fprintf(stderr, "glyph bitmap overflow");
        return rect;
    }

    assert(rect.x + rect.w <= width);
    assert(rect.y + rect.h <= height);

    face.emplace(glyph.id, GlyphValue { rect, tile_id });

    // Copy the bitmap
    char *target = data;
    const char *source = glyph.bitmap.data();
    for (uint32_t y = 0; y < buffered_height; y++) {
        uint32_t y1 = width * (rect.y + y) + rect.x;
        uint32_t y2 = buffered_width * y;
        for (uint32_t x = 0; x < buffered_width; x++) {
            target[y1 + x] = source[y2 + x];
        }
    }

    dirty = true;

    return rect;
}

void GlyphAtlas::removeGlyphs(uint64_t tile_id) {
    std::lock_guard<std::mutex> lock(mtx);

    for (auto& faces : index) {
        std::map<uint32_t, GlyphValue>& face = faces.second;
        for (auto it = face.begin(); it != face.end(); /* we advance in the body */) {
            GlyphValue& value = it->second;
            value.ids.erase(tile_id);

            if (!value.ids.size()) {
                const Rect<uint16_t>& rect = value.rect;

                // Clear out the bitmap.
                char *target = data;
                for (uint32_t y = 0; y < rect.h; y++) {
                    uint32_t y1 = width * (rect.y + y) + rect.x;
                    for (uint32_t x = 0; x < rect.w; x++) {
                        target[y1 + x] = 0;
                    }
                }

                dirty = true;

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
        bind();

        std::lock_guard<std::mutex> lock(mtx);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);
        dirty = false;

#if defined(DEBUG)
        // platform::show_debug_image("Glyph Atlas", data, width, height);
#endif
    }
}

void GlyphAtlas::bind() {
    if (!texture) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    } else {
        glBindTexture(GL_TEXTURE_2D, texture);
    }
};
