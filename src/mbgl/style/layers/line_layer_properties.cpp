// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/line_layer_properties.hpp>

namespace mbgl {
namespace style {

void LineLayoutProperties::parse(const JSValue& value) {
    lineCap.parse("line-cap", value);
    lineJoin.parse("line-join", value);
    lineMiterLimit.parse("line-miter-limit", value);
    lineRoundLimit.parse("line-round-limit", value);
}

void LineLayoutProperties::recalculate(const CalculationParameters& parameters) {
    lineCap.calculate(parameters);
    lineJoin.calculate(parameters);
    lineMiterLimit.calculate(parameters);
    lineRoundLimit.calculate(parameters);
}

void LinePaintProperties::parse(const JSValue& value) {
    lineOpacity.parse("line-opacity", value);
    lineColor.parse("line-color", value);
    lineTranslate.parse("line-translate", value);
    lineTranslateAnchor.parse("line-translate-anchor", value);
    lineWidth.parse("line-width", value);
    lineGapWidth.parse("line-gap-width", value);
    lineOffset.parse("line-offset", value);
    lineBlur.parse("line-blur", value);
    lineDasharray.parse("line-dasharray", value);
    linePattern.parse("line-pattern", value);
}

void LinePaintProperties::cascade(const CascadeParameters& parameters) {
    lineOpacity.cascade(parameters);
    lineColor.cascade(parameters);
    lineTranslate.cascade(parameters);
    lineTranslateAnchor.cascade(parameters);
    lineWidth.cascade(parameters);
    lineGapWidth.cascade(parameters);
    lineOffset.cascade(parameters);
    lineBlur.cascade(parameters);
    lineDasharray.cascade(parameters);
    linePattern.cascade(parameters);
}

bool LinePaintProperties::recalculate(const CalculationParameters& parameters) {
    bool hasTransitions = false;

    hasTransitions |= lineOpacity.calculate(parameters);
    hasTransitions |= lineColor.calculate(parameters);
    hasTransitions |= lineTranslate.calculate(parameters);
    hasTransitions |= lineTranslateAnchor.calculate(parameters);
    hasTransitions |= lineWidth.calculate(parameters);
    hasTransitions |= lineGapWidth.calculate(parameters);
    hasTransitions |= lineOffset.calculate(parameters);
    hasTransitions |= lineBlur.calculate(parameters);
    hasTransitions |= lineDasharray.calculate(parameters);
    hasTransitions |= linePattern.calculate(parameters);

    return hasTransitions;
}

} // namespace style
} // namespace mbgl
