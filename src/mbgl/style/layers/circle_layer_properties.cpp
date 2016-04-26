// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/circle_layer_properties.hpp>

namespace mbgl {
namespace style {

void CirclePaintProperties::parse(const JSValue& value) {
    circleRadius.parse("circle-radius", value);
    circleColor.parse("circle-color", value);
    circleBlur.parse("circle-blur", value);
    circleOpacity.parse("circle-opacity", value);
    circleTranslate.parse("circle-translate", value);
    circleTranslateAnchor.parse("circle-translate-anchor", value);
}

void CirclePaintProperties::cascade(const CascadeParameters& parameters) {
    circleRadius.cascade(parameters);
    circleColor.cascade(parameters);
    circleBlur.cascade(parameters);
    circleOpacity.cascade(parameters);
    circleTranslate.cascade(parameters);
    circleTranslateAnchor.cascade(parameters);
}

bool CirclePaintProperties::recalculate(const CalculationParameters& parameters) {
    bool hasTransitions = false;

    hasTransitions |= circleRadius.calculate(parameters);
    hasTransitions |= circleColor.calculate(parameters);
    hasTransitions |= circleBlur.calculate(parameters);
    hasTransitions |= circleOpacity.calculate(parameters);
    hasTransitions |= circleTranslate.calculate(parameters);
    hasTransitions |= circleTranslateAnchor.calculate(parameters);

    return hasTransitions;
}

} // namespace style
} // namespace mbgl
