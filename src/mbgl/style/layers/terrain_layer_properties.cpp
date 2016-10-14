// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/terrain_layer_properties.hpp>

namespace mbgl {
namespace style {

void TerrainPaintProperties::cascade(const CascadeParameters& parameters) {
    terrainShadowColor.cascade(parameters);
    terrainHighlightColor.cascade(parameters);
    terrainAccentColor.cascade(parameters);
    terrainIlluminationDirection.cascade(parameters);
    terrainIlluminationAlignment.cascade(parameters);
    terrainExaggeration.cascade(parameters);
}

bool TerrainPaintProperties::recalculate(const CalculationParameters& parameters) {
    bool hasTransitions = false;

    hasTransitions |= terrainShadowColor.calculate(parameters);
    hasTransitions |= terrainHighlightColor.calculate(parameters);
    hasTransitions |= terrainAccentColor.calculate(parameters);
    hasTransitions |= terrainIlluminationDirection.calculate(parameters);
    hasTransitions |= terrainIlluminationAlignment.calculate(parameters);
    hasTransitions |= terrainExaggeration.calculate(parameters);

    return hasTransitions;
}

} // namespace style
} // namespace mbgl
