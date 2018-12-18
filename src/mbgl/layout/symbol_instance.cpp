#include <mbgl/layout/symbol_instance.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>

namespace mbgl {

using namespace style;

SymbolInstance::SymbolInstance(Anchor& anchor_,
                               GeometryCoordinates line_,
                               const std::tuple<Shaping, Shaping, Shaping, Shaping>& shapedTextOrientations,
                               optional<PositionedIcon> shapedIcon,
                               const SymbolLayoutProperties::Evaluated& layout,
                               const float layoutTextSize_,
                               const float textBoxScale,
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
                               const std::u16string& key_,
                               const float overscaling,
                               const float rotate) :
    anchor(anchor_),
    line(line_),
    hasText(false),
    hasIcon(shapedIcon),

    // Create the collision features that will be used to check whether this symbol instance can be placed
    textCollisionFeature(line_, anchor, std::get<0>(shapedTextOrientations), textBoxScale, textPadding, textPlacement, indexedFeature, overscaling, rotate),
    iconCollisionFeature(line_, anchor, shapedIcon, iconBoxScale, iconPadding, indexedFeature, rotate),
    layoutFeatureIndex(layoutFeatureIndex_),
    dataFeatureIndex(dataFeatureIndex_),
    textOffset(textOffset_),
    iconOffset(iconOffset_),
    key(key_),
    layoutTextSize(layoutTextSize_) {

    // Create the quads used for rendering the icon and glyphs.
    if (shapedIcon) {
        iconQuad = getIconQuad(*shapedIcon, layout, layoutTextSize, std::get<0>(shapedTextOrientations));
    }
    if (std::get<0>(shapedTextOrientations)) {
        rightJustifiedGlyphQuads = getGlyphQuads(std::get<0>(shapedTextOrientations), layout, textPlacement, positions);
    }
    if (std::get<1>(shapedTextOrientations)) {
        centerJustifiedGlyphQuads = getGlyphQuads(std::get<1>(shapedTextOrientations), layout, textPlacement, positions);
    }
    if (std::get<2>(shapedTextOrientations)) {
        leftJustifiedGlyphQuads = getGlyphQuads(std::get<2>(shapedTextOrientations), layout, textPlacement, positions);
    }
    if (std::get<3>(shapedTextOrientations)) {
        verticalGlyphQuads = getGlyphQuads(std::get<3>(shapedTextOrientations), layout, textPlacement, positions);
    }
    // 'hasText' depends on finding at least one glyph in the shaping that's also in the GlyphPositionMap
    hasText = rightJustifiedGlyphQuads.size() > 0 || centerJustifiedGlyphQuads.size() > 0 || leftJustifiedGlyphQuads.size() > 0 || verticalGlyphQuads.size() > 0;

    if (std::get<0>(shapedTextOrientations) && std::get<3>(shapedTextOrientations)) {
        writingModes = WritingModeType::Horizontal | WritingModeType::Vertical;
    } else if (std::get<0>(shapedTextOrientations)) {
        writingModes = WritingModeType::Horizontal;
    } else if (std::get<3>(shapedTextOrientations)) {
        writingModes = WritingModeType::Vertical;
    } else {
        writingModes = WritingModeType::None;
    }
}

} // namespace mbgl
