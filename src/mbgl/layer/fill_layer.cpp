#include <mbgl/layer/fill_layer.hpp>
#include <mbgl/style/property_parsing.hpp>

namespace mbgl {

void FillLayer::parsePaints(const JSVal& layer) {
    paints.parseEach(layer, [&] (ClassProperties& paint, const JSVal& value) {
        parseProperty<Function<bool>>("fill-antialias", PropertyKey::FillAntialias, paint, value);
        parseProperty<Function<float>>("fill-opacity", PropertyKey::FillOpacity, paint, value);
        parseProperty<PropertyTransition>("fill-opacity-transition", PropertyKey::FillOpacity, paint, value);
        parseProperty<Function<Color>>("fill-color", PropertyKey::FillColor, paint, value);
        parseProperty<PropertyTransition>("fill-color-transition", PropertyKey::FillColor, paint, value);
        parseProperty<Function<Color>>("fill-outline-color", PropertyKey::FillOutlineColor, paint, value);
        parseProperty<PropertyTransition>("fill-outline-color-transition", PropertyKey::FillOutlineColor, paint, value);
        parseProperty<Function<std::array<float, 2>>>("fill-translate", PropertyKey::FillTranslate, paint, value);
        parseProperty<PropertyTransition>("fill-translate-transition", PropertyKey::FillTranslate, paint, value);
        parseProperty<Function<TranslateAnchorType>>("fill-translate-anchor", PropertyKey::FillTranslateAnchor, paint, value);
        parseProperty<PiecewiseConstantFunction<Faded<std::string>>>("fill-pattern", PropertyKey::FillImage, paint, value, "fill-pattern-transition");
    });
}

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
