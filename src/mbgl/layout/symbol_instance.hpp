#pragma once

#include <mbgl/text/quads.hpp>
#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/text/collision_feature.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>


namespace mbgl {

class Anchor;
class IndexedSubfeature;

struct ShapedTextOrientations {
    Shaping horizontal;
    Shaping vertical;
    // The following are used with variable text placement on.
    Shaping& right = horizontal; 
    Shaping center;
    Shaping left;
    bool singleLine = false;
};

class SymbolInstance {
public:
    SymbolInstance(Anchor& anchor,
                   GeometryCoordinates line,
                   const ShapedTextOrientations& shapedTextOrientations,
                   optional<PositionedIcon> shapedIcon,
                   const style::SymbolLayoutProperties::Evaluated&,
                   const float layoutTextSize,
                   const float textBoxScale,
                   const float textPadding,
                   style::SymbolPlacementType textPlacement,
                   const std::array<float, 2> textOffset,
                   const float iconBoxScale,
                   const float iconPadding,
                   const std::array<float, 2> iconOffset,
                   const GlyphPositions&,
                   const IndexedSubfeature&,
                   const std::size_t layoutFeatureIndex,
                   const std::size_t dataFeatureIndex,
                   std::u16string key,
                   const float overscaling,
                   const float rotate,
                   float radialTextOffset);

    optional<size_t> getDefaultHorizontalPlacedTextIndex() const;
    Anchor anchor;
    GeometryCoordinates line;
    bool hasText;
    bool hasIcon;
    // Note: When singleLine == true, only `rightJustifiedGlyphQuads` is populated.
    SymbolQuads rightJustifiedGlyphQuads;
    SymbolQuads centerJustifiedGlyphQuads;
    SymbolQuads leftJustifiedGlyphQuads;

    SymbolQuads verticalGlyphQuads;

    optional<SymbolQuad> iconQuad;
    CollisionFeature textCollisionFeature;
    CollisionFeature iconCollisionFeature;
    WritingModeType writingModes;
    std::size_t layoutFeatureIndex; // Index into the set of features included at layout time
    std::size_t dataFeatureIndex;   // Index into the underlying tile data feature set
    std::array<float, 2> textOffset;
    std::array<float, 2> iconOffset;
    std::u16string key;
    bool isDuplicate;
    optional<size_t> placedRightTextIndex;
    optional<size_t> placedCenterTextIndex;
    optional<size_t> placedLeftTextIndex;
    optional<size_t> placedVerticalTextIndex;
    optional<size_t> placedIconIndex;
    float textBoxScale;
    float radialTextOffset;
    bool singleLine;
    uint32_t crossTileID = 0;
};

} // namespace mbgl
