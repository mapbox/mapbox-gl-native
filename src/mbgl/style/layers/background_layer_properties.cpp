// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/background_layer_properties.hpp>

namespace mbgl {
namespace style {

void BackgroundPaintProperties::parse(const JSValue& value) {
    backgroundColor.parse("background-color", value);
    backgroundPattern.parse("background-pattern", value);
    backgroundOpacity.parse("background-opacity", value);
}

void BackgroundPaintProperties::cascade(const CascadeParameters& parameters) {
    backgroundColor.cascade(parameters);
    backgroundPattern.cascade(parameters);
    backgroundOpacity.cascade(parameters);
}

bool BackgroundPaintProperties::recalculate(const CalculationParameters& parameters) {
    bool hasTransitions = false;

    hasTransitions |= backgroundColor.calculate(parameters);
    hasTransitions |= backgroundPattern.calculate(parameters);
    hasTransitions |= backgroundOpacity.calculate(parameters);

    return hasTransitions;
}

} // namespace style
} // namespace mbgl
