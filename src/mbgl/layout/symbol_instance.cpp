#include <mbgl/layout/symbol_instance.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>
#include <utility>

namespace mbgl {

using namespace style;

namespace {

const Shaping& getAnyShaping(const ShapedTextOrientations& shapedTextOrientations) {
    if (shapedTextOrientations.right) return shapedTextOrientations.right;
    if (shapedTextOrientations.center) return shapedTextOrientations.center;
    if (shapedTextOrientations.left) return shapedTextOrientations.left;
    if (shapedTextOrientations.vertical) return shapedTextOrientations.vertical;
    return shapedTextOrientations.horizontal;
}

} // namespace

SymbolInstance::SymbolInstance(Anchor& anchor_,
                               GeometryCoordinates line_,
                               const ShapedTextOrientations& shapedTextOrientations,
                               optional<PositionedIcon> shapedIcon,
                               const SymbolLayoutProperties::Evaluated& layout,
                               const float layoutTextSize,
                               const float textBoxScale_,
                               const float textPadding,
                               const SymbolPlacementType textPlacement,
                               const std::array<float, 2> textOffset_,
                               const float iconBoxScale,
                               const float iconPadding,
                               const std::array<float, 2> iconOffset_,
                               const GlyphPositions& positions,
                               const IndexedSubfeature& indexedFeature,
                               const std::size_t layoutFeatureIndex_,
                               const std::size_t dataFeatureIndex_,
                               std::u16string key_,
                               const float overscaling,
                               const float rotate,
                               float radialTextOffset_) :
    anchor(anchor_),
    line(line_),
    hasText(false),
    hasIcon(shapedIcon),

    // Create the collision features that will be used to check whether this symbol instance can be placed
    // As a collision approximation, we can use either the vertical or any of the horizontal versions of the feature
    textCollisionFeature(line_, anchor, getAnyShaping(shapedTextOrientations), textBoxScale_, textPadding, textPlacement, indexedFeature, overscaling, rotate),
    iconCollisionFeature(line_, anchor, shapedIcon, iconBoxScale, iconPadding, indexedFeature, rotate),
    writingModes(WritingModeType::None),
    layoutFeatureIndex(layoutFeatureIndex_),
    dataFeatureIndex(dataFeatureIndex_),
    textOffset(textOffset_),
    iconOffset(iconOffset_),
    key(std::move(key_)),
    textBoxScale(textBoxScale_),
    radialTextOffset(radialTextOffset_),
    singleLine(shapedTextOrientations.singleLine) {

    // Create the quads used for rendering the icon and glyphs.
    if (shapedIcon) {
        iconQuad = getIconQuad(*shapedIcon, layout, layoutTextSize, shapedTextOrientations.horizontal);
    }
    
    bool singleLineInitialized = false;
    const auto initHorizontalGlyphQuads = [&] (SymbolQuads& quads, const Shaping& shaping) {
        writingModes |= WritingModeType::Horizontal;
        if (!singleLine) {
            quads = getGlyphQuads(shaping, textOffset, layout, textPlacement, positions);
            return;
        }
        if (!singleLineInitialized) {
            rightJustifiedGlyphQuads = getGlyphQuads(shaping, textOffset, layout, textPlacement, positions);
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
        verticalGlyphQuads = getGlyphQuads(shapedTextOrientations.vertical, textOffset, layout, textPlacement, positions);
    }

    // 'hasText' depends on finding at least one glyph in the shaping that's also in the GlyphPositionMap
    hasText = !rightJustifiedGlyphQuads.empty() || !centerJustifiedGlyphQuads.empty() || !leftJustifiedGlyphQuads.empty() || !verticalGlyphQuads.empty();
}

optional<size_t> SymbolInstance::getDefaultHorizontalPlacedTextIndex() const {
    if (placedRightTextIndex) return placedRightTextIndex;
    if (placedCenterTextIndex) return placedCenterTextIndex;
    if (placedLeftTextIndex) return placedLeftTextIndex;
    return nullopt;
}
} // namespace mbgl
