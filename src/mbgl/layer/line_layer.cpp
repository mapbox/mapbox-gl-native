#include <mbgl/layer/line_layer.hpp>

namespace mbgl {

RenderPass LineLayer::applyStyleProperties(const StyleCalculationParameters& parameters) {
    applyTransitionedStyleProperty(PropertyKey::LineOpacity, properties.opacity, parameters);
    applyTransitionedStyleProperty(PropertyKey::LineColor, properties.color, parameters);
    applyTransitionedStyleProperty(PropertyKey::LineTranslate, properties.translate, parameters);
    applyStyleProperty(PropertyKey::LineTranslateAnchor, properties.translateAnchor, parameters);
    applyTransitionedStyleProperty(PropertyKey::LineWidth, properties.width, parameters);
    applyTransitionedStyleProperty(PropertyKey::LineGapWidth, properties.gap_width, parameters);
    applyTransitionedStyleProperty(PropertyKey::LineBlur, properties.blur, parameters);
    applyStyleProperty(PropertyKey::LineDashArray, properties.dash_array, parameters);
    applyStyleProperty(PropertyKey::LineImage, properties.image, parameters);

    // for scaling dasharrays
    StyleCalculationParameters dashArrayParams = parameters;
    dashArrayParams.z = std::floor(dashArrayParams.z);
    applyStyleProperty(PropertyKey::LineWidth, properties.dash_line_width, dashArrayParams);

    return properties.isVisible() ? RenderPass::Translucent : RenderPass::None;
}

}
