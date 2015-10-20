#include <mbgl/layer/line_layer.hpp>

namespace mbgl {

void LineLayer::recalculate(const StyleCalculationParameters& parameters) {
    paints.removeExpiredTransitions(parameters.now);

    paints.calculateTransitioned(PropertyKey::LineOpacity, properties.opacity, parameters);
    paints.calculateTransitioned(PropertyKey::LineColor, properties.color, parameters);
    paints.calculateTransitioned(PropertyKey::LineTranslate, properties.translate, parameters);
    paints.calculate(PropertyKey::LineTranslateAnchor, properties.translateAnchor, parameters);
    paints.calculateTransitioned(PropertyKey::LineWidth, properties.width, parameters);
    paints.calculateTransitioned(PropertyKey::LineGapWidth, properties.gap_width, parameters);
    paints.calculateTransitioned(PropertyKey::LineBlur, properties.blur, parameters);
    paints.calculate(PropertyKey::LineDashArray, properties.dash_array, parameters);
    paints.calculate(PropertyKey::LineImage, properties.image, parameters);

    // for scaling dasharrays
    StyleCalculationParameters dashArrayParams = parameters;
    dashArrayParams.z = std::floor(dashArrayParams.z);
    paints.calculate(PropertyKey::LineWidth, properties.dash_line_width, dashArrayParams);

    passes = properties.isVisible() ? RenderPass::Translucent : RenderPass::None;
}

}
