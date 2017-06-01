#pragma once

#include <mbgl/text/glyph.hpp>

#include <mapbox/shelf-pack.hpp>

namespace mbgl {

struct GlyphPosition {
    Rect<uint16_t> rect;
    GlyphMetrics metrics;
};

using GlyphPositions = std::map<GlyphID, GlyphPosition>;

class GlyphAtlas {
public:
    AlphaImage image;
    GlyphPositions positions;
};

GlyphAtlas makeGlyphAtlas(const Glyphs&);

} // namespace mbgl
