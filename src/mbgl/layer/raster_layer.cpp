#include <mbgl/layer/raster_layer.hpp>

namespace mbgl {

void RasterLayer::recalculate(const StyleCalculationParameters& parameters) {
    paints.removeExpiredTransitions(parameters.now);

    paints.calculateTransitioned(PropertyKey::RasterOpacity, properties.opacity, parameters);
    paints.calculateTransitioned(PropertyKey::RasterHueRotate, properties.hue_rotate, parameters);
    paints.calculateTransitioned(PropertyKey::RasterBrightnessLow, properties.brightness[0], parameters);
    paints.calculateTransitioned(PropertyKey::RasterBrightnessHigh, properties.brightness[1], parameters);
    paints.calculateTransitioned(PropertyKey::RasterSaturation, properties.saturation, parameters);
    paints.calculateTransitioned(PropertyKey::RasterContrast, properties.contrast, parameters);
    paints.calculateTransitioned(PropertyKey::RasterFade, properties.fade, parameters);

    passes = properties.isVisible() ? RenderPass::Translucent : RenderPass::None;
}

}
