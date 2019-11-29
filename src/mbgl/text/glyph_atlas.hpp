#pragma once

#include <mbgl/text/glyph.hpp>

#include <mapbox/shelf-pack.hpp>

namespace mbgl {

struct GlyphPosition {
    Rect<uint16_t> rect;
    GlyphMetrics metrics;
};

struct GlyphPositionData {
    std::map<GlyphID, GlyphPosition> glyphPositionMap{};
    optional<int32_t> ascender{nullopt};
    optional<int32_t> descender{nullopt};
};

using GlyphPositions = std::map<FontStackHash, GlyphPositionData>;

class GlyphAtlas {
public:
    AlphaImage image;
    GlyphPositions positions;
};

GlyphAtlas makeGlyphAtlas(const GlyphMap&);

} // namespace mbgl
