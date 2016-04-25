// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/layer/fill_layer_properties.hpp>

namespace mbgl {

void FillPaintProperties::parse(const JSValue& value) {
    fillAntialias.parse("fill-antialias", value);
    fillOpacity.parse("fill-opacity", value);
    fillColor.parse("fill-color", value);
    fillOutlineColor.parse("fill-outline-color", value);
    fillTranslate.parse("fill-translate", value);
    fillTranslateAnchor.parse("fill-translate-anchor", value);
    fillPattern.parse("fill-pattern", value);
}

void FillPaintProperties::cascade(const StyleCascadeParameters& parameters) {
    fillAntialias.cascade(parameters);
    fillOpacity.cascade(parameters);
    fillColor.cascade(parameters);
    fillOutlineColor.cascade(parameters);
    fillTranslate.cascade(parameters);
    fillTranslateAnchor.cascade(parameters);
    fillPattern.cascade(parameters);
}

bool FillPaintProperties::recalculate(const StyleCalculationParameters& parameters) {
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

} // namespace mbgl
