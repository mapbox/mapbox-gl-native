#include <mbgl/layer/line_layer.hpp>

namespace mbgl {

RenderPass LineLayer::applyStyleProperties(const float z, const TimePoint& now, const ZoomHistory& zoomHistory) {
    applyTransitionedStyleProperty(PropertyKey::LineOpacity, properties.opacity, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::LineColor, properties.color, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::LineTranslate, properties.translate, z, now, zoomHistory);
    applyStyleProperty(PropertyKey::LineTranslateAnchor, properties.translateAnchor, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::LineWidth, properties.width, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::LineGapWidth, properties.gap_width, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::LineBlur, properties.blur, z, now, zoomHistory);
    applyStyleProperty(PropertyKey::LineDashArray, properties.dash_array, z, now, zoomHistory);
    applyStyleProperty(PropertyKey::LineImage, properties.image, z, now, zoomHistory);

    // for scaling dasharrays
    applyStyleProperty(PropertyKey::LineWidth, properties.dash_line_width, std::floor(z), now, zoomHistory);

    return properties.isVisible() ? RenderPass::Translucent : RenderPass::None;
}

}
