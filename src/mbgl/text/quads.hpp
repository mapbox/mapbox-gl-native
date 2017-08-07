#pragma once

#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

#include <vector>

namespace mbgl {

class Anchor;
class PositionedIcon;

class SymbolQuad {
public:
    SymbolQuad(Point<float> tl_,
               Point<float> tr_,
               Point<float> bl_,
               Point<float> br_,
               Rect<uint16_t> tex_,
               WritingModeType writingMode_,
               Point<float> glyphOffset_)
        : tl(std::move(tl_)),
        tr(std::move(tr_)),
        bl(std::move(bl_)),
        br(std::move(br_)),
        tex(std::move(tex_)),
        writingMode(writingMode_),
        glyphOffset(glyphOffset_) {}

    Point<float> tl;
    Point<float> tr;
    Point<float> bl;
    Point<float> br;
    Rect<uint16_t> tex;
    WritingModeType writingMode;
    Point<float> glyphOffset;
};

using SymbolQuads = std::vector<SymbolQuad>;

SymbolQuad getIconQuad(const PositionedIcon& shapedIcon,
                       const style::SymbolLayoutProperties::Evaluated&,
                       const float layoutTextSize,
                       const Shaping& shapedText);

SymbolQuads getGlyphQuads(const Shaping& shapedText,
                          const style::SymbolLayoutProperties::Evaluated&,
                          style::SymbolPlacementType placement,
                          const GlyphPositionMap& positions);

} // namespace mbgl
