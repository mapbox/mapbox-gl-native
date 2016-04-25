// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/layer/raster_layer_properties.hpp>

namespace mbgl {

void RasterPaintProperties::parse(const JSValue& value) {
    rasterOpacity.parse("raster-opacity", value);
    rasterHueRotate.parse("raster-hue-rotate", value);
    rasterBrightnessMin.parse("raster-brightness-min", value);
    rasterBrightnessMax.parse("raster-brightness-max", value);
    rasterSaturation.parse("raster-saturation", value);
    rasterContrast.parse("raster-contrast", value);
    rasterFadeDuration.parse("raster-fade-duration", value);
}

void RasterPaintProperties::cascade(const StyleCascadeParameters& parameters) {
    rasterOpacity.cascade(parameters);
    rasterHueRotate.cascade(parameters);
    rasterBrightnessMin.cascade(parameters);
    rasterBrightnessMax.cascade(parameters);
    rasterSaturation.cascade(parameters);
    rasterContrast.cascade(parameters);
    rasterFadeDuration.cascade(parameters);
}

bool RasterPaintProperties::recalculate(const StyleCalculationParameters& parameters) {
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

} // namespace mbgl
