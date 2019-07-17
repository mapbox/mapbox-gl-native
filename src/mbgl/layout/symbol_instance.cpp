#include <mbgl/layout/symbol_instance.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>
#include <utility>

namespace mbgl {

using namespace style;

namespace {

const Shaping& getAnyShaping(const ShapedTextOrientations& shapedTextOrientations) {
    if (shapedTextOrientations.right)
        return shapedTextOrientations.right;
    if (shapedTextOrientations.center)
        return shapedTextOrientations.center;
    if (shapedTextOrientations.left)
        return shapedTextOrientations.left;
    if (shapedTextOrientations.vertical)
        return shapedTextOrientations.vertical;
    return shapedTextOrientations.horizontal;
}

} // namespace

SymbolInstanceCommonData::SymbolInstanceCommonData(const GeometryCoordinates& line_,
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
                                                   float radialTextOffset_)
    : line(line_),
    hasText(false),
    hasIcon(shapedIcon),
    writingModes(WritingModeType::None),
    layoutFeatureIndex(layoutFeatureIndex_),
    dataFeatureIndex(dataFeatureIndex_),
    textOffset(textOffset_),
    iconOffset(iconOffset_),
    key(std::move(key_)),
    textBoxScale(textBoxScale_),
    radialTextOffset(radialTextOffset_),
    singleLine(shapedTextOrientations.singleLine){
        
        // Create the quads used for rendering the icon and glyphs.
        if (shapedIcon) {
            iconQuad =
            getIconQuad(*shapedIcon, layout, layoutTextSize, shapedTextOrientations.horizontal);
        }
        
        bool singleLineInitialized = false;
        const auto initHorizontalGlyphQuads = [&](SymbolQuads& quads, const Shaping& shaping) {
            writingModes |= WritingModeType::Horizontal;
            if (!singleLine) {
                quads = getGlyphQuads(shaping, textOffset, layout, textPlacement, positions);
                return;
            }
            if (!singleLineInitialized) {
                rightJustifiedGlyphQuads =
                getGlyphQuads(shaping, textOffset, layout, textPlacement, positions);
                singleLineInitialized = true;
            }
        };
        
        if (shapedTextOrientations.right) {
            initHorizontalGlyphQuads(rightJustifiedGlyphQuads, shapedTextOrientations.right);
        }
        
        if (shapedTextOrientations.center) {
            initHorizontalGlyphQuads(centerJustifiedGlyphQuads, shapedTextOrientations.center);
        }
        
        if (shapedTextOrientations.left) {
            initHorizontalGlyphQuads(leftJustifiedGlyphQuads, shapedTextOrientations.left);
        }
        
        if (shapedTextOrientations.vertical) {
            writingModes |= WritingModeType::Vertical;
            verticalGlyphQuads = getGlyphQuads(shapedTextOrientations.vertical, textOffset, layout,
                                               textPlacement, positions);
        }
        
        // 'hasText' depends on finding at least one glyph in the shaping that's also in the
        // GlyphPositionMap
        hasText = !rightJustifiedGlyphQuads.empty() || !centerJustifiedGlyphQuads.empty() ||
        !leftJustifiedGlyphQuads.empty() || !verticalGlyphQuads.empty();
    }
    
SymbolInstance::SymbolInstance(const Anchor& anchor_,
                               std::shared_ptr<SymbolInstanceCommonData> data_,
                               const ShapedTextOrientations& shapedTextOrientations,
                               optional<PositionedIcon> shapedIcon,
                               const float textPadding,
                               const style::SymbolPlacementType textPlacement,
                               const float iconBoxScale,
                               const float iconPadding,
                               const IndexedSubfeature& indexedFeature,
                               const float overscaling,
                               const float rotate)
    : anchor(std::move(anchor_)),
    commonData(std::move(data_)),
      // Create the collision features that will be used to check whether this symbol instance can
      // be placedc As a collision approximation, we can use either the vertical or any of the
      // horizontal versions of the feature
      textCollisionFeature(commonData->line,
                           anchor,
                           getAnyShaping(shapedTextOrientations),
                           commonData->textBoxScale,
                           textPadding,
                           textPlacement,
                           indexedFeature,
                           overscaling,
                           rotate),
      iconCollisionFeature(commonData->line,
                           anchor,
                           shapedIcon,
                           iconBoxScale,
                           iconPadding,
                           indexedFeature,
                           rotate) {
}

optional<size_t> SymbolInstance::getDefaultHorizontalPlacedTextIndex() const {
    if (placedRightTextIndex)
        return placedRightTextIndex;
    if (placedCenterTextIndex)
        return placedCenterTextIndex;
    if (placedLeftTextIndex)
        return placedLeftTextIndex;
    return nullopt;
}
} // namespace mbgl
