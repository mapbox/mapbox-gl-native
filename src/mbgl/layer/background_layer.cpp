#include <mbgl/layer/background_layer.hpp>

namespace mbgl {

RenderPass BackgroundLayer::applyStyleProperties(const StyleCalculationParameters& parameters) {
    applyTransitionedStyleProperty(PropertyKey::BackgroundOpacity, properties.opacity, parameters);
    applyTransitionedStyleProperty(PropertyKey::BackgroundColor, properties.color, parameters);
    applyStyleProperty(PropertyKey::BackgroundImage, properties.image, parameters);
    return properties.isVisible() ? RenderPass::Translucent : RenderPass::None;
}

}
