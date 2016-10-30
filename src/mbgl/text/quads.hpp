#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

#include <vector>

namespace mbgl {

struct Anchor;
class PositionedIcon;

struct SymbolQuad {
    explicit SymbolQuad(Point<float> tl_, Point<float> tr_, Point<float> bl_, Point<float> br_,
            Rect<uint16_t> tex_, float anchorAngle_, float glyphAngle_, Point<float> anchorPoint_,
            float minScale_, float maxScale_)
        : tl(std::move(tl_)),
        tr(std::move(tr_)),
        bl(std::move(bl_)),
        br(std::move(br_)),
        tex(std::move(tex_)),
        anchorAngle(anchorAngle_),
        glyphAngle(glyphAngle_),
        anchorPoint(std::move(anchorPoint_)),
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
        const GeometryCoordinates& line, const style::SymbolLayoutProperties::Evaluated&,
        style::SymbolPlacementType placement, const Shaping& shapedText);

SymbolQuads getGlyphQuads(Anchor& anchor, const Shaping& shapedText,
        const float boxScale, const GeometryCoordinates& line, const style::SymbolLayoutProperties::Evaluated&,
        style::SymbolPlacementType placement, const GlyphPositions& face);

} // namespace mbgl
