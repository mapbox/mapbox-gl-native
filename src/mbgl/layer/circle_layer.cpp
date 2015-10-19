#include <mbgl/layer/circle_layer.hpp>

namespace mbgl {

RenderPass CircleLayer::applyStyleProperties(const float z, const TimePoint& now, const ZoomHistory& zoomHistory) {
    applyTransitionedStyleProperty(PropertyKey::CircleRadius, properties.radius, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::CircleColor, properties.color, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::CircleOpacity, properties.opacity, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::CircleTranslate, properties.translate, z, now, zoomHistory);
    applyStyleProperty(PropertyKey::CircleTranslateAnchor, properties.translateAnchor, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::CircleBlur, properties.blur, z, now, zoomHistory);
    return properties.isVisible() ? RenderPass::Translucent : RenderPass::None;
}

}
