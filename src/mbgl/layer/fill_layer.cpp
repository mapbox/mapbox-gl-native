#include <mbgl/layer/fill_layer.hpp>

namespace mbgl {

void FillLayer::recalculate(const StyleCalculationParameters& parameters) {
    paints.removeExpiredTransitions(parameters.now);

    paints.calculate(PropertyKey::FillAntialias, properties.antialias, parameters);
    paints.calculateTransitioned(PropertyKey::FillOpacity, properties.opacity, parameters);
    paints.calculateTransitioned(PropertyKey::FillColor, properties.fill_color, parameters);
    paints.calculateTransitioned(PropertyKey::FillOutlineColor, properties.stroke_color, parameters);
    paints.calculateTransitioned(PropertyKey::FillTranslate, properties.translate, parameters);
    paints.calculate(PropertyKey::FillTranslateAnchor, properties.translateAnchor, parameters);
    paints.calculate(PropertyKey::FillImage, properties.image, parameters);

    passes = RenderPass::None;

    if (properties.antialias) {
        passes |= RenderPass::Translucent;
    }

    if (!properties.image.from.empty() || (properties.fill_color[3] * properties.opacity) < 1.0f) {
        passes |= RenderPass::Translucent;
    } else {
        passes |= RenderPass::Opaque;
    }
}

}
