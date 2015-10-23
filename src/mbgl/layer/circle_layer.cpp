#include <mbgl/layer/circle_layer.hpp>

namespace mbgl {

RenderPass CircleLayer::applyStyleProperties(const StyleCalculationParameters& parameters) {
    applyTransitionedStyleProperty(PropertyKey::CircleRadius, properties.radius, parameters);
    applyTransitionedStyleProperty(PropertyKey::CircleColor, properties.color, parameters);
    applyTransitionedStyleProperty(PropertyKey::CircleOpacity, properties.opacity, parameters);
    applyTransitionedStyleProperty(PropertyKey::CircleTranslate, properties.translate, parameters);
    applyStyleProperty(PropertyKey::CircleTranslateAnchor, properties.translateAnchor, parameters);
    applyTransitionedStyleProperty(PropertyKey::CircleBlur, properties.blur, parameters);
    return properties.isVisible() ? RenderPass::Translucent : RenderPass::None;
}

}
