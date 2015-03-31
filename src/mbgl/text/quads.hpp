#ifndef MBGL_TEXT_QUADS
#define MBGL_TEXT_QUADS

#include <mbgl/text/types.hpp>
#include <mbgl/text/glyph.hpp>

#include <mbgl/util/vec.hpp>

namespace mbgl {

    struct Anchor;
    class StyleLayoutSymbol;
    class PositionedIcon;

    PlacedGlyphs getIconQuads(Anchor &anchor, const PositionedIcon &shapedIcon,
            const std::vector<Coordinate> &line, const StyleLayoutSymbol &layout,
            const bool alongLine);
    
    PlacedGlyphs getGlyphQuads(Anchor &anchor, const Shaping &shapedText,
            const float boxScale, const std::vector<Coordinate> &line, const StyleLayoutSymbol &layout,
            const bool alongLine, const GlyphPositions &face);
}

#endif
