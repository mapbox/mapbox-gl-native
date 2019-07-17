#pragma once

#include <mbgl/style/layers/symbol_layer_properties.hpp>
#include <mbgl/text/collision_feature.hpp>
#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/text/quads.hpp>

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

class SymbolInstanceCommonData {
public:
    SymbolInstanceCommonData(const GeometryCoordinates& line_,
                             const ShapedTextOrientations& shapedTextOrientations,
                             optional<PositionedIcon> shapedIcon,
                             const style::SymbolLayoutProperties::Evaluated& layout,
                             const float layoutTextSize,
                             const float textBoxScale_,
                             const style::SymbolPlacementType textPlacement,
                             const std::array<float, 2> textOffset_,
                             const std::array<float, 2> iconOffset_,
                             const GlyphPositions& positions,
                             const std::size_t layoutFeatureIndex_,
                             const std::size_t dataFeatureIndex_,
                             std::u16string key_,
                             float radialTextOffset_);
    
    GeometryCoordinates line;
    bool hasText;
    bool hasIcon;
    // Note: When singleLine == true, only `rightJustifiedGlyphQuads` is populated.
    SymbolQuads rightJustifiedGlyphQuads;
    SymbolQuads centerJustifiedGlyphQuads;
    SymbolQuads leftJustifiedGlyphQuads;

    SymbolQuads verticalGlyphQuads;

    optional<SymbolQuad> iconQuad;
    WritingModeType writingModes;
    std::size_t layoutFeatureIndex; // Index into the set of features included at layout time
    std::size_t dataFeatureIndex;   // Index into the underlying tile data feature set
    std::array<float, 2> textOffset;
    std::array<float, 2> iconOffset;
    std::u16string key;
    float textBoxScale;
    float radialTextOffset;
    bool singleLine;
};

class SymbolInstance {
public:
    SymbolInstance(const Anchor& anchor_,
                   std::shared_ptr<SymbolInstanceCommonData> data_,
                   const ShapedTextOrientations& shapedTextOrientations,
                   optional<PositionedIcon> shapedIcon,
                   const float textPadding,
                   const style::SymbolPlacementType textPlacement,
                   const float iconBoxScale,
                   const float iconPadding,
                   const IndexedSubfeature& indexedFeature_,
                   const float overscaling,
                   const float rotate);

    optional<size_t> getDefaultHorizontalPlacedTextIndex() const;
    Anchor anchor;
    std::shared_ptr<SymbolInstanceCommonData> commonData{nullptr};
    CollisionFeature textCollisionFeature;
    CollisionFeature iconCollisionFeature;
    optional<size_t> placedRightTextIndex;
    optional<size_t> placedCenterTextIndex;
    optional<size_t> placedLeftTextIndex;
    optional<size_t> placedVerticalTextIndex;
    optional<size_t> placedIconIndex;
    uint32_t crossTileID{0};
};

} // namespace mbgl
