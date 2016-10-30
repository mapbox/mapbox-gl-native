#include <mbgl/layout/symbol_instance.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>

namespace mbgl {

using namespace style;

SymbolInstance::SymbolInstance(Anchor& anchor, const GeometryCoordinates& line,
        const Shaping& shapedText, const PositionedIcon& shapedIcon,
        const SymbolLayoutProperties::Evaluated& layout, const bool addToBuffers, const uint32_t index_,
        const float textBoxScale, const float textPadding, const SymbolPlacementType textPlacement,
        const float iconBoxScale, const float iconPadding, const SymbolPlacementType iconPlacement,
        const GlyphPositions& face, const IndexedSubfeature& indexedFeature) :
    point(anchor.point),
    index(index_),
    hasText(shapedText),
    hasIcon(shapedIcon),

    // Create the quads used for rendering the glyphs.
    glyphQuads(addToBuffers && shapedText ?
            getGlyphQuads(anchor, shapedText, textBoxScale, line, layout, textPlacement, face) :
            SymbolQuads()),

    // Create the quad used for rendering the icon.
    iconQuads(addToBuffers && shapedIcon ?
            getIconQuads(anchor, shapedIcon, line, layout, iconPlacement, shapedText) :
            SymbolQuads()),

    // Create the collision features that will be used to check whether this symbol instance can be placed
    textCollisionFeature(line, anchor, shapedText, textBoxScale, textPadding, textPlacement, indexedFeature),
    iconCollisionFeature(line, anchor, shapedIcon, iconBoxScale, iconPadding, iconPlacement, indexedFeature)
    {}

} // namespace mbgl
