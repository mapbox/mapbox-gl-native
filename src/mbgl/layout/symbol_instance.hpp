#pragma once

#include <mbgl/text/quads.hpp>
#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/text/collision_feature.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>


namespace mbgl {

class Anchor;
class IndexedSubfeature;

class SymbolInstance {
public:
    SymbolInstance(Anchor& anchor,
                   GeometryCoordinates line,
                   const std::pair<Shaping, Shaping>& shapedTextOrientations,
                   optional<PositionedIcon> shapedIcon,
                   const style::SymbolLayoutProperties::Evaluated&,
                   const float layoutTextSize,
                   const uint32_t index,
                   const float textBoxScale,
                   const float textPadding,
                   style::SymbolPlacementType textPlacement,
                   const std::array<float, 2> textOffset,
                   const float iconBoxScale,
                   const float iconPadding,
                   const std::array<float, 2> iconOffset,
                   const GlyphPositionMap&,
                   const IndexedSubfeature&,
                   const std::size_t featureIndex,
                   const std::u16string& key,
                   const float overscaling);

    Anchor anchor;
    GeometryCoordinates line;
    uint32_t index;
    bool hasText;
    bool hasIcon;
    SymbolQuads horizontalGlyphQuads;
    SymbolQuads verticalGlyphQuads;
    optional<SymbolQuad> iconQuad;
    CollisionFeature textCollisionFeature;
    CollisionFeature iconCollisionFeature;
    WritingModeType writingModes;
    std::size_t featureIndex;
    std::array<float, 2> textOffset;
    std::array<float, 2> iconOffset;
    std::u16string key;
    bool isDuplicate;
    optional<size_t> placedTextIndex;
    optional<size_t> placedVerticalTextIndex;
    optional<size_t> placedIconIndex;
    uint32_t crossTileID = 0;
};

} // namespace mbgl
