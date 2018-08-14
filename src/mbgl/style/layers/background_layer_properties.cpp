// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/background_layer_properties.hpp>

namespace mbgl {
namespace style {

BackgroundPaintProperties::Unevaluated BackgroundPaintProperties::Transitionable::transitioned(const TransitionParameters& parameters, Unevaluated&& prior) const {
    return Unevaluated {
        backgroundColor.transition(parameters, std::move(prior.backgroundColor)),
        backgroundPattern.transition(parameters, std::move(prior.backgroundPattern)),
        backgroundOpacity.transition(parameters, std::move(prior.backgroundOpacity)),
    };
}

BackgroundPaintProperties::Unevaluated BackgroundPaintProperties::Transitionable::untransitioned() const {
    return Unevaluated {
        Transitioning<PropertyValue<Color>>(backgroundColor.value),
        Transitioning<PropertyValue<std::string>>(backgroundPattern.value),
        Transitioning<PropertyValue<float>>(backgroundOpacity.value),
    };
}

bool BackgroundPaintProperties::Transitionable::hasDataDrivenPropertyDifference(const Transitionable& other) const {
    return false
        || backgroundColor.value.hasDataDrivenPropertyDifference(other.backgroundColor.value)
        || backgroundPattern.value.hasDataDrivenPropertyDifference(other.backgroundPattern.value)
        || backgroundOpacity.value.hasDataDrivenPropertyDifference(other.backgroundOpacity.value)
        ;
}

bool BackgroundPaintProperties::Unevaluated::hasTransition() const {
    return false
        || backgroundColor.hasTransition()
        || backgroundPattern.hasTransition()
        || backgroundOpacity.hasTransition()
        ;
}

BackgroundPaintProperties::PossiblyEvaluated BackgroundPaintProperties::Unevaluated::evaluate(const PropertyEvaluationParameters& parameters) const {
    return PossiblyEvaluated {
        backgroundColor.evaluate(typename BackgroundColor::EvaluatorType(parameters, BackgroundColor::defaultValue()), parameters.now),
        backgroundPattern.evaluate(typename BackgroundPattern::EvaluatorType(parameters, BackgroundPattern::defaultValue()), parameters.now),
        backgroundOpacity.evaluate(typename BackgroundOpacity::EvaluatorType(parameters, BackgroundOpacity::defaultValue()), parameters.now),
    };
}



} // namespace style
} // namespace mbgl
