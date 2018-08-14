// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/hillshade_layer_properties.hpp>

namespace mbgl {
namespace style {

HillshadePaintProperties::Unevaluated HillshadePaintProperties::Transitionable::transitioned(const TransitionParameters& parameters, Unevaluated&& prior) const {
    return Unevaluated {
        hillshadeIlluminationDirection.transition(parameters, std::move(prior.hillshadeIlluminationDirection)),
        hillshadeIlluminationAnchor.transition(parameters, std::move(prior.hillshadeIlluminationAnchor)),
        hillshadeExaggeration.transition(parameters, std::move(prior.hillshadeExaggeration)),
        hillshadeShadowColor.transition(parameters, std::move(prior.hillshadeShadowColor)),
        hillshadeHighlightColor.transition(parameters, std::move(prior.hillshadeHighlightColor)),
        hillshadeAccentColor.transition(parameters, std::move(prior.hillshadeAccentColor)),
    };
}

HillshadePaintProperties::Unevaluated HillshadePaintProperties::Transitionable::untransitioned() const {
    return Unevaluated {
        Transitioning<PropertyValue<float>>(hillshadeIlluminationDirection.value),
        Transitioning<PropertyValue<HillshadeIlluminationAnchorType>>(hillshadeIlluminationAnchor.value),
        Transitioning<PropertyValue<float>>(hillshadeExaggeration.value),
        Transitioning<PropertyValue<Color>>(hillshadeShadowColor.value),
        Transitioning<PropertyValue<Color>>(hillshadeHighlightColor.value),
        Transitioning<PropertyValue<Color>>(hillshadeAccentColor.value),
    };
}

bool HillshadePaintProperties::Transitionable::hasDataDrivenPropertyDifference(const Transitionable& other) const {
    return false
        || hillshadeIlluminationDirection.value.hasDataDrivenPropertyDifference(other.hillshadeIlluminationDirection.value)
        || hillshadeIlluminationAnchor.value.hasDataDrivenPropertyDifference(other.hillshadeIlluminationAnchor.value)
        || hillshadeExaggeration.value.hasDataDrivenPropertyDifference(other.hillshadeExaggeration.value)
        || hillshadeShadowColor.value.hasDataDrivenPropertyDifference(other.hillshadeShadowColor.value)
        || hillshadeHighlightColor.value.hasDataDrivenPropertyDifference(other.hillshadeHighlightColor.value)
        || hillshadeAccentColor.value.hasDataDrivenPropertyDifference(other.hillshadeAccentColor.value)
        ;
}

bool HillshadePaintProperties::Unevaluated::hasTransition() const {
    return false
        || hillshadeIlluminationDirection.hasTransition()
        || hillshadeIlluminationAnchor.hasTransition()
        || hillshadeExaggeration.hasTransition()
        || hillshadeShadowColor.hasTransition()
        || hillshadeHighlightColor.hasTransition()
        || hillshadeAccentColor.hasTransition()
        ;
}

HillshadePaintProperties::PossiblyEvaluated HillshadePaintProperties::Unevaluated::evaluate(const PropertyEvaluationParameters& parameters) const {
    return PossiblyEvaluated {
        hillshadeIlluminationDirection.evaluate(typename HillshadeIlluminationDirection::EvaluatorType(parameters, HillshadeIlluminationDirection::defaultValue()), parameters.now),
        hillshadeIlluminationAnchor.evaluate(typename HillshadeIlluminationAnchor::EvaluatorType(parameters, HillshadeIlluminationAnchor::defaultValue()), parameters.now),
        hillshadeExaggeration.evaluate(typename HillshadeExaggeration::EvaluatorType(parameters, HillshadeExaggeration::defaultValue()), parameters.now),
        hillshadeShadowColor.evaluate(typename HillshadeShadowColor::EvaluatorType(parameters, HillshadeShadowColor::defaultValue()), parameters.now),
        hillshadeHighlightColor.evaluate(typename HillshadeHighlightColor::EvaluatorType(parameters, HillshadeHighlightColor::defaultValue()), parameters.now),
        hillshadeAccentColor.evaluate(typename HillshadeAccentColor::EvaluatorType(parameters, HillshadeAccentColor::defaultValue()), parameters.now),
    };
}



} // namespace style
} // namespace mbgl
