#include <mbgl/layer/fill_layer.hpp>

namespace mbgl {

RenderPass FillLayer::applyStyleProperties(const StyleCalculationParameters& parameters) {
    applyStyleProperty(PropertyKey::FillAntialias, properties.antialias, parameters);
    applyTransitionedStyleProperty(PropertyKey::FillOpacity, properties.opacity, parameters);
    applyTransitionedStyleProperty(PropertyKey::FillColor, properties.fill_color, parameters);
    applyTransitionedStyleProperty(PropertyKey::FillOutlineColor, properties.stroke_color, parameters);
    applyTransitionedStyleProperty(PropertyKey::FillTranslate, properties.translate, parameters);
    applyStyleProperty(PropertyKey::FillTranslateAnchor, properties.translateAnchor, parameters);
    applyStyleProperty(PropertyKey::FillImage, properties.image, parameters);

    RenderPass result = RenderPass::None;

    if (properties.antialias) {
        result |= RenderPass::Translucent;
    }

    if (!properties.image.from.empty() || (properties.fill_color[3] * properties.opacity) < 1.0f) {
        result |= RenderPass::Translucent;
    } else {
        result |= RenderPass::Opaque;
    }

    return result;
}

}
