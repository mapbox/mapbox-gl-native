#ifndef LLMR_GEOMETRY_GLYPH_ATLAS
#define LLMR_GEOMETRY_GLYPH_ATLAS

#include <llmr/geometry/binpack.hpp>

#include <string>
#include <set>
#include <map>
#include <mutex>
#include <atomic>

namespace llmr {

class VectorTileGlyph;

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
                            const VectorTileGlyph& glyph);
    void removeGlyphs(uint64_t tile_id);
    void bind();

public:
    const uint16_t width = 0;
    const uint16_t height = 0;

private:
    std::mutex mtx;
    BinPack<uint16_t> bin;
    std::map<std::string, std::map<uint32_t, GlyphValue>> index;
    char *const data = nullptr;
    std::atomic<bool> dirty;
    uint32_t texture = 0;
};

};

#endif
