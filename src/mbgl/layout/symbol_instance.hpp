#pragma once

#include <mbgl/text/quads.hpp>
#include <mbgl/text/glyph.hpp>
#include <mbgl/text/collision_feature.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>

namespace mbgl {

class Anchor;
class IndexedSubfeature;

class SymbolInstance {
public:
    SymbolInstance(Anchor& anchor,
                   const GeometryCoordinates& line,
                   const std::pair<Shaping, Shaping>& shapedTextOrientations,
                   optional<PositionedIcon> shapedIcon,
                   const style::SymbolLayoutProperties::Evaluated&,
                   const float layoutTextSize,
                   const bool inside,
                   const uint32_t index,
                   const float textBoxScale,
                   const float textPadding,
                   style::SymbolPlacementType textPlacement,
                   const float iconBoxScale,
                   const float iconPadding,
                   style::SymbolPlacementType iconPlacement,
                   const GlyphPositions& face,
                   const IndexedSubfeature&,
                   const std::size_t featureIndex);

    Point<float> point;
    uint32_t index;
    bool hasText;
    bool hasIcon;
    SymbolQuads glyphQuads;
    optional<SymbolQuad> iconQuad;
    CollisionFeature textCollisionFeature;
    CollisionFeature iconCollisionFeature;
    WritingModeType writingModes;
    std::size_t featureIndex;
};

} // namespace mbgl
