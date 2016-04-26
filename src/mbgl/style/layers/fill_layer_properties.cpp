// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/fill_layer_properties.hpp>

namespace mbgl {
namespace style {

void FillPaintProperties::parse(const JSValue& value) {
    fillAntialias.parse("fill-antialias", value);
    fillOpacity.parse("fill-opacity", value);
    fillColor.parse("fill-color", value);
    fillOutlineColor.parse("fill-outline-color", value);
    fillTranslate.parse("fill-translate", value);
    fillTranslateAnchor.parse("fill-translate-anchor", value);
    fillPattern.parse("fill-pattern", value);
}

void FillPaintProperties::cascade(const CascadeParameters& parameters) {
    fillAntialias.cascade(parameters);
    fillOpacity.cascade(parameters);
    fillColor.cascade(parameters);
    fillOutlineColor.cascade(parameters);
    fillTranslate.cascade(parameters);
    fillTranslateAnchor.cascade(parameters);
    fillPattern.cascade(parameters);
}

bool FillPaintProperties::recalculate(const CalculationParameters& parameters) {
    bool hasTransitions = false;

    hasTransitions |= fillAntialias.calculate(parameters);
    hasTransitions |= fillOpacity.calculate(parameters);
    hasTransitions |= fillColor.calculate(parameters);
    hasTransitions |= fillOutlineColor.calculate(parameters);
    hasTransitions |= fillTranslate.calculate(parameters);
    hasTransitions |= fillTranslateAnchor.calculate(parameters);
    hasTransitions |= fillPattern.calculate(parameters);

    return hasTransitions;
}

} // namespace style
} // namespace mbgl
