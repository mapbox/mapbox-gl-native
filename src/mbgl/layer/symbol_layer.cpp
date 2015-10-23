#include <mbgl/layer/symbol_layer.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/style/property_evaluator.hpp>

namespace mbgl {

RenderPass SymbolLayer::applyStyleProperties(const StyleCalculationParameters& parameters) {
    applyTransitionedStyleProperty(PropertyKey::IconOpacity, properties.icon.opacity, parameters);
    applyTransitionedStyleProperty(PropertyKey::IconColor, properties.icon.color, parameters);
    applyTransitionedStyleProperty(PropertyKey::IconHaloColor, properties.icon.halo_color, parameters);
    applyTransitionedStyleProperty(PropertyKey::IconHaloWidth, properties.icon.halo_width, parameters);
    applyTransitionedStyleProperty(PropertyKey::IconHaloBlur, properties.icon.halo_blur, parameters);
    applyTransitionedStyleProperty(PropertyKey::IconTranslate, properties.icon.translate, parameters);
    applyStyleProperty(PropertyKey::IconTranslateAnchor, properties.icon.translate_anchor, parameters);

    applyTransitionedStyleProperty(PropertyKey::TextOpacity, properties.text.opacity, parameters);
    applyTransitionedStyleProperty(PropertyKey::TextColor, properties.text.color, parameters);
    applyTransitionedStyleProperty(PropertyKey::TextHaloColor, properties.text.halo_color, parameters);
    applyTransitionedStyleProperty(PropertyKey::TextHaloWidth, properties.text.halo_width, parameters);
    applyTransitionedStyleProperty(PropertyKey::TextHaloBlur, properties.text.halo_blur, parameters);
    applyTransitionedStyleProperty(PropertyKey::TextTranslate, properties.text.translate, parameters);
    applyStyleProperty(PropertyKey::TextTranslateAnchor, properties.text.translate_anchor, parameters);

    // text-size and icon-size are layout properties but they also need to be evaluated as paint properties:
    auto it = bucket->layout.properties.find(PropertyKey::IconSize);
    if (it != bucket->layout.properties.end()) {
        const PropertyEvaluator<float> evaluator(parameters);
        properties.icon.size = mapbox::util::apply_visitor(evaluator, it->second);
    }
    it = bucket->layout.properties.find(PropertyKey::TextSize);
    if (it != bucket->layout.properties.end()) {
        const PropertyEvaluator<float> evaluator(parameters);
        properties.text.size = mapbox::util::apply_visitor(evaluator, it->second);
    }

    return properties.isVisible() ? RenderPass::Translucent : RenderPass::None;
}

}
