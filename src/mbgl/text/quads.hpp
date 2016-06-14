#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/tile/geometry_tile.hpp>

#include <vector>

namespace mbgl {

struct Anchor;
class PositionedIcon;

namespace style {
class SymbolLayoutProperties;
}

struct SymbolQuad {
    explicit SymbolQuad(const Point<float> &tl_, const Point<float> &tr_,
            const Point<float> &bl_, const Point<float> &br_,
            const Rect<uint16_t> &tex_, float anchorAngle_, float glyphAngle_, const Point<float> &anchorPoint_,
            float minScale_, float maxScale_)
        : tl(tl_),
        tr(tr_),
        bl(bl_),
        br(br_),
        tex(tex_),
        anchorAngle(anchorAngle_),
        glyphAngle(glyphAngle_),
        anchorPoint(anchorPoint_),
        minScale(minScale_),
        maxScale(maxScale_) {}

    Point<float> tl, tr, bl, br;
    Rect<uint16_t> tex;
    float anchorAngle, glyphAngle;
    Point<float> anchorPoint;
    float minScale, maxScale;
};

typedef std::vector<SymbolQuad> SymbolQuads;

SymbolQuads getIconQuads(Anchor& anchor, const PositionedIcon& shapedIcon,
        const GeometryCoordinates& line, const style::SymbolLayoutProperties&,
        const bool alongLine);

SymbolQuads getGlyphQuads(Anchor& anchor, const Shaping& shapedText,
        const float boxScale, const GeometryCoordinates& line, const style::SymbolLayoutProperties&,
        const bool alongLine, const GlyphPositions& face);

} // namespace mbgl
