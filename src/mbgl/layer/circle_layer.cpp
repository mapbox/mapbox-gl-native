#include <mbgl/layer/circle_layer.hpp>

namespace mbgl {

void CircleLayer::recalculate(const StyleCalculationParameters& parameters) {
    paints.removeExpiredTransitions(parameters.now);

    paints.calculateTransitioned(PropertyKey::CircleRadius, properties.radius, parameters);
    paints.calculateTransitioned(PropertyKey::CircleColor, properties.color, parameters);
    paints.calculateTransitioned(PropertyKey::CircleOpacity, properties.opacity, parameters);
    paints.calculateTransitioned(PropertyKey::CircleTranslate, properties.translate, parameters);
    paints.calculate(PropertyKey::CircleTranslateAnchor, properties.translateAnchor, parameters);
    paints.calculateTransitioned(PropertyKey::CircleBlur, properties.blur, parameters);

    passes = properties.isVisible() ? RenderPass::Translucent : RenderPass::None;
}

}
