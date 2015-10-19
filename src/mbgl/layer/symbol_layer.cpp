#include <mbgl/layer/symbol_layer.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/style/property_evaluator.hpp>

namespace mbgl {

RenderPass SymbolLayer::applyStyleProperties(const float z, const TimePoint& now, const ZoomHistory& zoomHistory) {
    applyTransitionedStyleProperty(PropertyKey::IconOpacity, properties.icon.opacity, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::IconColor, properties.icon.color, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::IconHaloColor, properties.icon.halo_color, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::IconHaloWidth, properties.icon.halo_width, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::IconHaloBlur, properties.icon.halo_blur, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::IconTranslate, properties.icon.translate, z, now, zoomHistory);
    applyStyleProperty(PropertyKey::IconTranslateAnchor, properties.icon.translate_anchor, z, now, zoomHistory);

    applyTransitionedStyleProperty(PropertyKey::TextOpacity, properties.text.opacity, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::TextColor, properties.text.color, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::TextHaloColor, properties.text.halo_color, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::TextHaloWidth, properties.text.halo_width, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::TextHaloBlur, properties.text.halo_blur, z, now, zoomHistory);
    applyTransitionedStyleProperty(PropertyKey::TextTranslate, properties.text.translate, z, now, zoomHistory);
    applyStyleProperty(PropertyKey::TextTranslateAnchor, properties.text.translate_anchor, z, now, zoomHistory);

    // text-size and icon-size are layout properties but they also need to be evaluated as paint properties:
    auto it = bucket->layout.properties.find(PropertyKey::IconSize);
    if (it != bucket->layout.properties.end()) {
        const PropertyEvaluator<float> evaluator(z, zoomHistory);
        properties.icon.size = mapbox::util::apply_visitor(evaluator, it->second);
    }
    it = bucket->layout.properties.find(PropertyKey::TextSize);
    if (it != bucket->layout.properties.end()) {
        const PropertyEvaluator<float> evaluator(z, zoomHistory);
        properties.text.size = mapbox::util::apply_visitor(evaluator, it->second);
    }

    return properties.isVisible() ? RenderPass::Translucent : RenderPass::None;
}

}
