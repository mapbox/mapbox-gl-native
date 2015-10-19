#include <mbgl/layer/background_layer.hpp>

namespace mbgl {

RenderPass BackgroundLayer::applyStyleProperties(const float z, const TimePoint& now, const ZoomHistory& zoomHistory) {
    applyTransitionedStyleProperty(PropertyKey::BackgroundOpacity, properties.opacity, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::BackgroundColor, properties.color, z, now, zoomHistory);
    applyStyleProperty(PropertyKey::BackgroundImage, properties.image, z, now, zoomHistory);
    return properties.isVisible() ? RenderPass::Translucent : RenderPass::None;
}

}
