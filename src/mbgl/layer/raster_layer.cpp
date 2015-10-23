#include <mbgl/layer/raster_layer.hpp>

namespace mbgl {

RenderPass RasterLayer::applyStyleProperties(const StyleCalculationParameters& parameters) {
    applyTransitionedStyleProperty(PropertyKey::RasterOpacity, properties.opacity, parameters);
    applyTransitionedStyleProperty(PropertyKey::RasterHueRotate, properties.hue_rotate, parameters);
    applyTransitionedStyleProperty(PropertyKey::RasterBrightnessLow, properties.brightness[0], parameters);
    applyTransitionedStyleProperty(PropertyKey::RasterBrightnessHigh, properties.brightness[1], parameters);
    applyTransitionedStyleProperty(PropertyKey::RasterSaturation, properties.saturation, parameters);
    applyTransitionedStyleProperty(PropertyKey::RasterContrast, properties.contrast, parameters);
    applyTransitionedStyleProperty(PropertyKey::RasterFade, properties.fade, parameters);
    return properties.isVisible() ? RenderPass::Translucent : RenderPass::None;
}

}
