#ifndef MBGL_TEXT_QUADS
#define MBGL_TEXT_QUADS

#include <mbgl/text/glyph.hpp>
#include <mbgl/util/vec.hpp>

#include <vector>

namespace mbgl {

    struct SymbolQuad {
        explicit SymbolQuad(const vec2<float> &tl_, const vec2<float> &tr_,
                const vec2<float> &bl_, const vec2<float> &br_,
                const Rect<uint16_t> &tex_, float angle_, const vec2<float> &anchor_,
                float minScale_, float maxScale_)
            : tl(tl_),
            tr(tr_),
            bl(bl_),
            br(br_),
            tex(tex_),
            angle(angle_),
            anchor(anchor_),
            minScale(minScale_),
            maxScale(maxScale_) {}

        vec2<float> tl, tr, bl, br;
        Rect<uint16_t> tex;
        float angle;
        vec2<float> anchor;
        float minScale, maxScale;
    };

    typedef std::vector<SymbolQuad> SymbolQuads;

    struct Anchor;
    class StyleLayoutSymbol;
    class PositionedIcon;

    SymbolQuads getIconQuads(Anchor &anchor, const PositionedIcon &shapedIcon,
            const std::vector<Coordinate> &line, const StyleLayoutSymbol &layout,
            const bool alongLine);
    
    SymbolQuads getGlyphQuads(Anchor &anchor, const Shaping &shapedText,
            const float boxScale, const std::vector<Coordinate> &line, const StyleLayoutSymbol &layout,
            const bool alongLine, const GlyphPositions &face);
}

#endif
