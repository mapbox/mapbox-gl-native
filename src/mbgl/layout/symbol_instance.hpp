#pragma once

#include <mbgl/text/quads.hpp>
#include <mbgl/text/collision_feature.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>

namespace mbgl {

struct Anchor;
class IndexedSubfeature;

class SymbolInstance {
public:
    explicit SymbolInstance(Anchor& anchor, const GeometryCoordinates& line,
            const Shaping& shapedText, const PositionedIcon& shapedIcon,
            const style::SymbolLayoutProperties::Evaluated&, const bool inside, const uint32_t index,
            const float textBoxScale, const float textPadding, style::SymbolPlacementType textPlacement,
            const float iconBoxScale, const float iconPadding, style::SymbolPlacementType iconPlacement,
            const GlyphPositions& face, const IndexedSubfeature& indexedfeature);

    Point<float> point;
    uint32_t index;
    bool hasText;
    bool hasIcon;
    SymbolQuads glyphQuads;
    SymbolQuads iconQuads;
    CollisionFeature textCollisionFeature;
    CollisionFeature iconCollisionFeature;
};

} // namespace mbgl
