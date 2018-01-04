#include <mbgl/layout/symbol_instance.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>

namespace mbgl {

using namespace style;

SymbolInstance::SymbolInstance(Anchor& anchor_,
                               GeometryCoordinates line_,
                               const std::pair<Shaping, Shaping>& shapedTextOrientations,
                               optional<PositionedIcon> shapedIcon,
                               const SymbolLayoutProperties::Evaluated& layout,
                               const float layoutTextSize,
                               const uint32_t index_,
                               const float textBoxScale,
                               const float textPadding,
                               const SymbolPlacementType textPlacement,
                               const std::array<float, 2> textOffset_,
                               const float iconBoxScale,
                               const float iconPadding,
                               const std::array<float, 2> iconOffset_,
                               const GlyphPositionMap& positions,
                               const IndexedSubfeature& indexedFeature,
                               const std::size_t featureIndex_,
                               const std::u16string& key_,
                               const float overscaling) :
    anchor(anchor_),
    line(line_),
    index(index_),
    hasText(shapedTextOrientations.first || shapedTextOrientations.second),
    hasIcon(shapedIcon),

    // Create the collision features that will be used to check whether this symbol instance can be placed
    textCollisionFeature(line_, anchor, shapedTextOrientations.first, textBoxScale, textPadding, textPlacement, indexedFeature, overscaling),
    iconCollisionFeature(line_, anchor, shapedIcon, iconBoxScale, iconPadding, indexedFeature),
    featureIndex(featureIndex_),
    textOffset(textOffset_),
    iconOffset(iconOffset_),
    key(key_) {

    // Create the quads used for rendering the icon and glyphs.
    if (shapedIcon) {
        iconQuad = getIconQuad(*shapedIcon, layout, layoutTextSize, shapedTextOrientations.first);
    }
    if (shapedTextOrientations.first) {
        horizontalGlyphQuads = getGlyphQuads(shapedTextOrientations.first, layout, textPlacement, positions);
    }
    if (shapedTextOrientations.second) {
        verticalGlyphQuads = getGlyphQuads(shapedTextOrientations.second, layout, textPlacement, positions);
    }

    if (shapedTextOrientations.first && shapedTextOrientations.second) {
        writingModes = WritingModeType::Horizontal | WritingModeType::Vertical;
    } else if (shapedTextOrientations.first) {
        writingModes = WritingModeType::Horizontal;
    } else if (shapedTextOrientations.second) {
        writingModes = WritingModeType::Vertical;
    } else {
        writingModes = WritingModeType::None;
    }
}

} // namespace mbgl
