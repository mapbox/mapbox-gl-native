// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/light_properties.hpp>

namespace mbgl {
namespace style {

LightProperties::Unevaluated LightProperties::Transitionable::transitioned(const TransitionParameters& parameters, Unevaluated&& prior) const {
    return Unevaluated {
        anchor.transition(parameters, std::move(prior.anchor)),
        position.transition(parameters, std::move(prior.position)),
        color.transition(parameters, std::move(prior.color)),
        intensity.transition(parameters, std::move(prior.intensity)),
    };
}

LightProperties::Unevaluated LightProperties::Transitionable::untransitioned() const {
    return Unevaluated {
        Transitioning<PropertyValue<LightAnchorType>>(anchor.value),
        Transitioning<PropertyValue<Position>>(position.value),
        Transitioning<PropertyValue<Color>>(color.value),
        Transitioning<PropertyValue<float>>(intensity.value),
    };
}

bool LightProperties::Unevaluated::hasTransition() const {
    return false
        || anchor.hasTransition()
        || position.hasTransition()
        || color.hasTransition()
        || intensity.hasTransition()
        ;
}

LightProperties::Evaluated LightProperties::Unevaluated::evaluate(const PropertyEvaluationParameters& parameters) const {
    return Evaluated {
        anchor.evaluate(typename LightAnchor::EvaluatorType(parameters, LightAnchor::defaultValue()), parameters.now),
        position.evaluate(typename LightPosition::EvaluatorType(parameters, LightPosition::defaultValue()), parameters.now),
        color.evaluate(typename LightColor::EvaluatorType(parameters, LightColor::defaultValue()), parameters.now),
        intensity.evaluate(typename LightIntensity::EvaluatorType(parameters, LightIntensity::defaultValue()), parameters.now),
    };
}

} // namespace style
} // namespace mbgl
