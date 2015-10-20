#include <mbgl/layer/symbol_layer.hpp>
#include <mbgl/style/style_bucket.hpp>
#include <mbgl/style/property_evaluator.hpp>

namespace mbgl {

void SymbolLayer::recalculate(const StyleCalculationParameters& parameters) {
    paints.removeExpiredTransitions(parameters.now);

    paints.calculateTransitioned(PropertyKey::IconOpacity, properties.icon.opacity, parameters);
    paints.calculateTransitioned(PropertyKey::IconColor, properties.icon.color, parameters);
    paints.calculateTransitioned(PropertyKey::IconHaloColor, properties.icon.halo_color, parameters);
    paints.calculateTransitioned(PropertyKey::IconHaloWidth, properties.icon.halo_width, parameters);
    paints.calculateTransitioned(PropertyKey::IconHaloBlur, properties.icon.halo_blur, parameters);
    paints.calculateTransitioned(PropertyKey::IconTranslate, properties.icon.translate, parameters);
    paints.calculate(PropertyKey::IconTranslateAnchor, properties.icon.translate_anchor, parameters);

    paints.calculateTransitioned(PropertyKey::TextOpacity, properties.text.opacity, parameters);
    paints.calculateTransitioned(PropertyKey::TextColor, properties.text.color, parameters);
    paints.calculateTransitioned(PropertyKey::TextHaloColor, properties.text.halo_color, parameters);
    paints.calculateTransitioned(PropertyKey::TextHaloWidth, properties.text.halo_width, parameters);
    paints.calculateTransitioned(PropertyKey::TextHaloBlur, properties.text.halo_blur, parameters);
    paints.calculateTransitioned(PropertyKey::TextTranslate, properties.text.translate, parameters);
    paints.calculate(PropertyKey::TextTranslateAnchor, properties.text.translate_anchor, parameters);

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

    passes = properties.isVisible() ? RenderPass::Translucent : RenderPass::None;
}

}
