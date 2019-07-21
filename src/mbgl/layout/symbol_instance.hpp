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

struct SymbolInstanceSharedData {
    SymbolInstanceSharedData(GeometryCoordinates line,
                            const ShapedTextOrientations& shapedTextOrientations,
                            const optional<PositionedIcon>& shapedIcon,
                            const style::SymbolLayoutProperties::Evaluated& layout,
                            const float layoutTextSize,
                            const style::SymbolPlacementType textPlacement,
                            const std::array<float, 2>& textOffset,
                            const GlyphPositions& positions);
    bool empty() const;
    GeometryCoordinates line;
    // Note: When singleLine == true, only `rightJustifiedGlyphQuads` is populated.
    SymbolQuads rightJustifiedGlyphQuads;
    SymbolQuads centerJustifiedGlyphQuads;
    SymbolQuads leftJustifiedGlyphQuads;
    SymbolQuads verticalGlyphQuads;
    optional<SymbolQuad> iconQuad;
};

class SymbolInstance {
public:
    SymbolInstance(Anchor& anchor_,
                   std::shared_ptr<SymbolInstanceSharedData> sharedData,
                   const ShapedTextOrientations& shapedTextOrientations,
                   const optional<PositionedIcon>& shapedIcon,
                   const float textBoxScale,
                   const float textPadding,
                   const style::SymbolPlacementType textPlacement,
                   const std::array<float, 2>& textOffset,
                   const float iconBoxScale,
                   const float iconPadding,
                   const std::array<float, 2>& iconOffset,
                   const IndexedSubfeature& indexedFeature,
                   const std::size_t layoutFeatureIndex,
                   const std::size_t dataFeatureIndex,
                   std::u16string key,
                   const float overscaling,
                   const float rotate,
                   float radialTextOffset);

    optional<size_t> getDefaultHorizontalPlacedTextIndex() const;
    const GeometryCoordinates& line() const;
    const SymbolQuads& rightJustifiedGlyphQuads() const;
    const SymbolQuads& leftJustifiedGlyphQuads() const;
    const SymbolQuads& centerJustifiedGlyphQuads() const;
    const SymbolQuads& verticalGlyphQuads() const;
    const optional<SymbolQuad>& iconQuad() const;
    void releaseSharedData();

private:
    std::shared_ptr<SymbolInstanceSharedData> sharedData;

public:
    Anchor anchor;
    bool hasText;
    bool hasIcon;

    std::size_t rightJustifiedGlyphQuadsSize;
    std::size_t centerJustifiedGlyphQuadsSize;
    std::size_t leftJustifiedGlyphQuadsSize;
    std::size_t verticalGlyphQuadsSize;

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
