#include <mbgl/layer/background_layer.hpp>

namespace mbgl {

void BackgroundLayer::recalculate(const StyleCalculationParameters& parameters) {
    paints.removeExpiredTransitions(parameters.now);

    paints.calculateTransitioned(PropertyKey::BackgroundOpacity, properties.opacity, parameters);
    paints.calculateTransitioned(PropertyKey::BackgroundColor, properties.color, parameters);
    paints.calculate(PropertyKey::BackgroundImage, properties.image, parameters);

    passes = properties.isVisible() ? RenderPass::Translucent : RenderPass::None;
}

}
