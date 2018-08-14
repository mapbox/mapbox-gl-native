// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/heatmap_layer_properties.hpp>

namespace mbgl {
namespace style {

HeatmapPaintProperties::Unevaluated HeatmapPaintProperties::Transitionable::transitioned(const TransitionParameters& parameters, Unevaluated&& prior) const {
    return Unevaluated {
        heatmapRadius.transition(parameters, std::move(prior.heatmapRadius)),
        heatmapWeight.transition(parameters, std::move(prior.heatmapWeight)),
        heatmapIntensity.transition(parameters, std::move(prior.heatmapIntensity)),
        heatmapColor.transition(parameters, std::move(prior.heatmapColor)),
        heatmapOpacity.transition(parameters, std::move(prior.heatmapOpacity)),
    };
}

HeatmapPaintProperties::Unevaluated HeatmapPaintProperties::Transitionable::untransitioned() const {
    return Unevaluated {
        Transitioning<PropertyValue<float>>(heatmapRadius.value),
        Transitioning<PropertyValue<float>>(heatmapWeight.value),
        Transitioning<PropertyValue<float>>(heatmapIntensity.value),
        Transitioning<ColorRampPropertyValue>(heatmapColor.value),
        Transitioning<PropertyValue<float>>(heatmapOpacity.value),
    };
}

bool HeatmapPaintProperties::Transitionable::hasDataDrivenPropertyDifference(const Transitionable& other) const {
    return false
        || heatmapRadius.value.hasDataDrivenPropertyDifference(other.heatmapRadius.value)
        || heatmapWeight.value.hasDataDrivenPropertyDifference(other.heatmapWeight.value)
        || heatmapIntensity.value.hasDataDrivenPropertyDifference(other.heatmapIntensity.value)
        || heatmapColor.value.hasDataDrivenPropertyDifference(other.heatmapColor.value)
        || heatmapOpacity.value.hasDataDrivenPropertyDifference(other.heatmapOpacity.value)
        ;
}

bool HeatmapPaintProperties::Unevaluated::hasTransition() const {
    return false
        || heatmapRadius.hasTransition()
        || heatmapWeight.hasTransition()
        || heatmapIntensity.hasTransition()
        || heatmapColor.hasTransition()
        || heatmapOpacity.hasTransition()
        ;
}

HeatmapPaintProperties::PossiblyEvaluated HeatmapPaintProperties::Unevaluated::evaluate(const PropertyEvaluationParameters& parameters) const {
    return PossiblyEvaluated {
        heatmapRadius.evaluate(typename HeatmapRadius::EvaluatorType(parameters, HeatmapRadius::defaultValue()), parameters.now),
        heatmapWeight.evaluate(typename HeatmapWeight::EvaluatorType(parameters, HeatmapWeight::defaultValue()), parameters.now),
        heatmapIntensity.evaluate(typename HeatmapIntensity::EvaluatorType(parameters, HeatmapIntensity::defaultValue()), parameters.now),
        heatmapColor.evaluate(typename HeatmapColor::EvaluatorType(parameters, HeatmapColor::defaultValue()), parameters.now),
        heatmapOpacity.evaluate(typename HeatmapOpacity::EvaluatorType(parameters, HeatmapOpacity::defaultValue()), parameters.now),
    };
}

HeatmapPaintProperties::Binders HeatmapPaintProperties::PossiblyEvaluated::createBinders(float z) const {
    return Binders {
        PaintPropertyBinder<float, typename attributes::a_radius::Type>::create(heatmapRadius, z, HeatmapRadius::defaultValue()),
        PaintPropertyBinder<float, typename attributes::a_weight::Type>::create(heatmapWeight, z, HeatmapWeight::defaultValue()),
    };
}

std::bitset<8> HeatmapPaintProperties::PossiblyEvaluated::constants() const {
    std::bitset<8> result;
    result.set(0, heatmapRadius.isConstant());
    result.set(1, heatmapWeight.isConstant());
    return result;
}

std::vector<std::string> HeatmapPaintProperties::PossiblyEvaluated::defines() const {
    std::vector<std::string> result;
    result.push_back(heatmapRadius.isConstant()
        ? std::string("#define HAS_UNIFORM_") + HeatmapRadius::Uniform::name()
        : std::string());
    result.push_back(heatmapWeight.isConstant()
        ? std::string("#define HAS_UNIFORM_") + HeatmapWeight::Uniform::name()
        : std::string());
    return result;
}

void HeatmapPaintProperties::Binders::populateVertexVectors(const GeometryTileFeature& feature, std::size_t length) {
    heatmapRadius->populateVertexVector(feature, length);
    heatmapWeight->populateVertexVector(feature, length);
}

void HeatmapPaintProperties::Binders::upload(gl::Context& context) {
    heatmapRadius->upload(context);
    heatmapWeight->upload(context);
}

HeatmapPaintProperties::Binders::AttributeBindings HeatmapPaintProperties::Binders::attributeBindings(const PossiblyEvaluated& currentProperties) const {
    return AttributeBindings {
        heatmapRadius->attributeBinding(currentProperties.heatmapRadius),
        heatmapWeight->attributeBinding(currentProperties.heatmapWeight),
    };
}

HeatmapPaintProperties::Binders::UniformValues HeatmapPaintProperties::Binders::uniformValues(float currentZoom, const PossiblyEvaluated& currentProperties) const {
    return UniformValues {
        typename InterpolationUniform<attributes::a_radius>::Value {
            heatmapRadius->interpolationFactor(currentZoom)
        },
        typename InterpolationUniform<attributes::a_weight>::Value {
            heatmapWeight->interpolationFactor(currentZoom)
        },
        typename uniforms::u_radius::Value {
            heatmapRadius->uniformValue(currentProperties.heatmapRadius)
        },
        typename uniforms::u_weight::Value {
            heatmapWeight->uniformValue(currentProperties.heatmapWeight)
        },
    };
}


} // namespace style
} // namespace mbgl
