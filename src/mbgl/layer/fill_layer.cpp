#include <mbgl/layer/fill_layer.hpp>

namespace mbgl {

RenderPass FillLayer::applyStyleProperties(const float z, const TimePoint& now, const ZoomHistory& zoomHistory) {
    applyStyleProperty(PropertyKey::FillAntialias, properties.antialias, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::FillOpacity, properties.opacity, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::FillColor, properties.fill_color, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::FillOutlineColor, properties.stroke_color, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::FillTranslate, properties.translate, z, now, zoomHistory);
    applyStyleProperty(PropertyKey::FillTranslateAnchor, properties.translateAnchor, z, now, zoomHistory);
    applyStyleProperty(PropertyKey::FillImage, properties.image, z, now, zoomHistory);

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
