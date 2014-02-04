#ifndef LLMR_GEOMETRY_GLYPH_ATLAS
#define LLMR_GEOMETRY_GLYPH_ATLAS

#include <llmr/geometry/binpack.hpp>

#include <string>
#include <set>
#include <map>

namespace llmr {

class Glyph {
public:
    uint32_t id = 0;
    std::string bitmap;
    uint16_t width = 0;
    uint16_t height = 0;
};

class GlyphAtlas {
public:

private:
    struct GlyphValue {
        GlyphValue(const Rect<uint16_t>& rect, uint64_t id)
            : rect(rect), ids({ id }) {}
        Rect<uint16_t> rect;
        std::set<uint64_t> ids;
    };

public:
    GlyphAtlas(uint16_t width, uint16_t height);
    ~GlyphAtlas();


    Rect<uint16_t> addGlyph(uint64_t tile_id, const std::string& face_name,
                            const Glyph& glyph, uint8_t buffer);
    void removeGlyphs(uint64_t tile_id);
    void bind();

private:
    const uint16_t width = 0;
    const uint16_t height = 0;
    BinPack<uint16_t> bin;
    std::map<std::string, std::map<uint32_t, GlyphValue>> index;
    char *const data = NULL;
    bool dirty = true;
    uint32_t texture = 0;
};

};

#endif
