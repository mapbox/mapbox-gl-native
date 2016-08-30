// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/raster_layer_properties.hpp>

namespace mbgl {
namespace style {

void RasterPaintProperties::cascade(const CascadeParameters& parameters) {
    rasterOpacity.cascade(parameters);
    rasterHueRotate.cascade(parameters);
    rasterBrightnessMin.cascade(parameters);
    rasterBrightnessMax.cascade(parameters);
    rasterSaturation.cascade(parameters);
    rasterContrast.cascade(parameters);
    rasterFadeDuration.cascade(parameters);
}

bool RasterPaintProperties::recalculate(const CalculationParameters& parameters) {
    bool hasTransitions = false;

    hasTransitions |= rasterOpacity.calculate(parameters);
    hasTransitions |= rasterHueRotate.calculate(parameters);
    hasTransitions |= rasterBrightnessMin.calculate(parameters);
    hasTransitions |= rasterBrightnessMax.calculate(parameters);
    hasTransitions |= rasterSaturation.calculate(parameters);
    hasTransitions |= rasterContrast.calculate(parameters);
    hasTransitions |= rasterFadeDuration.calculate(parameters);

    return hasTransitions;
}

} // namespace style
} // namespace mbgl
