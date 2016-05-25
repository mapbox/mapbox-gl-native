#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/tile/geometry_tile.hpp>

#include <vector>

namespace mbgl {

    struct SymbolQuad {
        explicit SymbolQuad(const Point<float> &tl_, const Point<float> &tr_,
                const Point<float> &bl_, const Point<float> &br_,
                const Rect<uint16_t> &tex_, float angle_, const Point<float> &anchorPoint_,
                float minScale_, float maxScale_)
            : tl(tl_),
            tr(tr_),
            bl(bl_),
            br(br_),
            tex(tex_),
            angle(angle_),
            anchorPoint(anchorPoint_),
            minScale(minScale_),
            maxScale(maxScale_) {}

        Point<float> tl, tr, bl, br;
        Rect<uint16_t> tex;
        float angle;
        Point<float> anchorPoint;
        float minScale, maxScale;
    };

    typedef std::vector<SymbolQuad> SymbolQuads;

    struct Anchor;
    class SymbolLayoutProperties;
    class PositionedIcon;

    SymbolQuads getIconQuads(Anchor& anchor, const PositionedIcon& shapedIcon,
            const GeometryCoordinates& line, const SymbolLayoutProperties& layout,
            const bool alongLine);
    
    SymbolQuads getGlyphQuads(Anchor& anchor, const Shaping& shapedText,
            const float boxScale, const GeometryCoordinates& line, const SymbolLayoutProperties& layout,
            const bool alongLine, const GlyphPositions& face);
} // namespace mbgl
