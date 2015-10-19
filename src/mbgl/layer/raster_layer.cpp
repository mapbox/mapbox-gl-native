#include <mbgl/layer/raster_layer.hpp>

namespace mbgl {

RenderPass RasterLayer::applyStyleProperties(const float z, const TimePoint& now, const ZoomHistory& zoomHistory) {
    applyTransitionedStyleProperty(PropertyKey::RasterOpacity, properties.opacity, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::RasterHueRotate, properties.hue_rotate, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::RasterBrightnessLow, properties.brightness[0], z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::RasterBrightnessHigh, properties.brightness[1], z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::RasterSaturation, properties.saturation, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::RasterContrast, properties.contrast, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::RasterFade, properties.fade, z, now, zoomHistory);
    return properties.isVisible() ? RenderPass::Translucent : RenderPass::None;
}

}
