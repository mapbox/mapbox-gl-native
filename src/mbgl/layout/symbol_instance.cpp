#include <mbgl/layout/symbol_instance.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>

namespace mbgl {

using namespace style;

SymbolInstance::SymbolInstance(Anchor& anchor,
                               const GeometryCoordinates& line,
                               const std::pair<Shaping, Shaping>& shapedTextOrientations,
                               const PositionedIcon& shapedIcon,
                               const SymbolLayoutProperties::Evaluated& layout,
                               const float layoutTextSize,
                               const bool addToBuffers,
                               const uint32_t index_,
                               const float textBoxScale,
                               const float textPadding,
                               const SymbolPlacementType textPlacement,
                               const float iconBoxScale,
                               const float iconPadding,
                               const SymbolPlacementType iconPlacement,
                               const GlyphPositions& face,
                               const IndexedSubfeature& indexedFeature,
                               const std::size_t featureIndex_) :
    point(anchor.point),
    index(index_),
    hasText(shapedTextOrientations.first || shapedTextOrientations.second),
    hasIcon(shapedIcon),


    // Create the collision features that will be used to check whether this symbol instance can be placed
    textCollisionFeature(line, anchor, shapedTextOrientations.second ?: shapedTextOrientations.first, textBoxScale, textPadding, textPlacement, indexedFeature),
    iconCollisionFeature(line, anchor, shapedIcon, iconBoxScale, iconPadding, iconPlacement, indexedFeature),
    featureIndex(featureIndex_) {
        
    

    // Create the quads used for rendering the icon and glyphs.
    if (addToBuffers) {
        if (shapedIcon) {
            iconQuad = getIconQuad(anchor, shapedIcon, line, layout, layoutTextSize, iconPlacement, shapedTextOrientations.first);
        }
        if (shapedTextOrientations.first) {
            auto quads = getGlyphQuads(anchor, shapedTextOrientations.first, textBoxScale, line, layout, textPlacement, face);
            glyphQuads.insert(glyphQuads.end(), quads.begin(), quads.end());
        }
        if (shapedTextOrientations.second) {
            auto quads = getGlyphQuads(anchor, shapedTextOrientations.second, textBoxScale, line, layout, textPlacement, face);
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
