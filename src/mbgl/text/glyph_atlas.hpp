#pragma once

#include <mbgl/text/glyph.hpp>

#include <mapbox/shelf-pack.hpp>

namespace mbgl {

struct GlyphPosition {
    Rect<uint16_t> rect;
    GlyphMetrics metrics;
};

using GlyphPositionMap = std::map<GlyphID, GlyphPosition>;
using GlyphPositions = std::map<FontStack, GlyphPositionMap>;

class GlyphAtlas {
public:
    AlphaImage image;
    GlyphPositions positions;
};

GlyphAtlas makeGlyphAtlas(const GlyphMap&);

} // namespace mbgl
