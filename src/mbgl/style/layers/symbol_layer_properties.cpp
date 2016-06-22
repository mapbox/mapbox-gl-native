// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/symbol_layer_properties.hpp>

namespace mbgl {
namespace style {

void SymbolLayoutProperties::recalculate(const CalculationParameters& parameters) {
    symbolPlacement.calculate(parameters);
    symbolSpacing.calculate(parameters);
    symbolAvoidEdges.calculate(parameters);
    iconAllowOverlap.calculate(parameters);
    iconIgnorePlacement.calculate(parameters);
    iconOptional.calculate(parameters);
    iconRotationAlignment.calculate(parameters);
    iconSize.calculate(parameters);
    iconTextFit.calculate(parameters);
    iconTextFitPadding.calculate(parameters);
    iconImage.calculate(parameters);
    iconRotate.calculate(parameters);
    iconPadding.calculate(parameters);
    iconKeepUpright.calculate(parameters);
    iconOffset.calculate(parameters);
    textPitchAlignment.calculate(parameters);
    textRotationAlignment.calculate(parameters);
    textField.calculate(parameters);
    textFont.calculate(parameters);
    textSize.calculate(parameters);
    textMaxWidth.calculate(parameters);
    textLineHeight.calculate(parameters);
    textLetterSpacing.calculate(parameters);
    textJustify.calculate(parameters);
    textAnchor.calculate(parameters);
    textMaxAngle.calculate(parameters);
    textRotate.calculate(parameters);
    textPadding.calculate(parameters);
    textKeepUpright.calculate(parameters);
    textTransform.calculate(parameters);
    textOffset.calculate(parameters);
    textAllowOverlap.calculate(parameters);
    textIgnorePlacement.calculate(parameters);
    textOptional.calculate(parameters);
}

void SymbolPaintProperties::cascade(const CascadeParameters& parameters) {
    iconOpacity.cascade(parameters);
    iconColor.cascade(parameters);
    iconHaloColor.cascade(parameters);
    iconHaloWidth.cascade(parameters);
    iconHaloBlur.cascade(parameters);
    iconTranslate.cascade(parameters);
    iconTranslateAnchor.cascade(parameters);
    textOpacity.cascade(parameters);
    textColor.cascade(parameters);
    textHaloColor.cascade(parameters);
    textHaloWidth.cascade(parameters);
    textHaloBlur.cascade(parameters);
    textTranslate.cascade(parameters);
    textTranslateAnchor.cascade(parameters);
}

bool SymbolPaintProperties::recalculate(const CalculationParameters& parameters) {
    bool hasTransitions = false;

    hasTransitions |= iconOpacity.calculate(parameters);
    hasTransitions |= iconColor.calculate(parameters);
    hasTransitions |= iconHaloColor.calculate(parameters);
    hasTransitions |= iconHaloWidth.calculate(parameters);
    hasTransitions |= iconHaloBlur.calculate(parameters);
    hasTransitions |= iconTranslate.calculate(parameters);
    hasTransitions |= iconTranslateAnchor.calculate(parameters);
    hasTransitions |= textOpacity.calculate(parameters);
    hasTransitions |= textColor.calculate(parameters);
    hasTransitions |= textHaloColor.calculate(parameters);
    hasTransitions |= textHaloWidth.calculate(parameters);
    hasTransitions |= textHaloBlur.calculate(parameters);
    hasTransitions |= textTranslate.calculate(parameters);
    hasTransitions |= textTranslateAnchor.calculate(parameters);

    return hasTransitions;
}

} // namespace style
} // namespace mbgl
