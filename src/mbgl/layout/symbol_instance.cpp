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
                               const bool addToBuffers,
                               const uint32_t index_,
                               const float textBoxScale,
                               const float textPadding,
                               const SymbolPlacementType textPlacement,
                               const std::array<float, 2> textOffset_,
                               const float iconBoxScale,
                               const float iconPadding,
                               const SymbolPlacementType iconPlacement,
                               const std::array<float, 2> iconOffset_,
                               const GlyphPositionMap& positions,
                               const IndexedSubfeature& indexedFeature,
                               const std::size_t featureIndex_,
                               const std::u16string& key_) :
    anchor(anchor_),
    line(line_),
    index(index_),
    hasText(shapedTextOrientations.first || shapedTextOrientations.second),
    hasIcon(shapedIcon),

    // Create the collision features that will be used to check whether this symbol instance can be placed
    textCollisionFeature(line_, anchor, shapedTextOrientations.second ?: shapedTextOrientations.first, textBoxScale, textPadding, textPlacement, indexedFeature),
    iconCollisionFeature(line_, anchor, shapedIcon, iconBoxScale, iconPadding, iconPlacement, indexedFeature),
    featureIndex(featureIndex_),
    textOffset(textOffset_),
    iconOffset(iconOffset_),
    key(key_),
    placedText(false),
    placedIcon(false) {

    // Create the quads used for rendering the icon and glyphs.
    if (addToBuffers) {
        if (shapedIcon) {
            iconQuad = getIconQuad(*shapedIcon, layout, layoutTextSize, shapedTextOrientations.first);
        }
        if (shapedTextOrientations.first) {
            auto quads = getGlyphQuads(shapedTextOrientations.first, layout, textPlacement, positions);
            glyphQuads.insert(glyphQuads.end(), quads.begin(), quads.end());
        }
        if (shapedTextOrientations.second) {
            auto quads = getGlyphQuads(shapedTextOrientations.second, layout, textPlacement, positions);
            glyphQuads.insert(glyphQuads.end(), quads.begin(), quads.end());
        }
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
