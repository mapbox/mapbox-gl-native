// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/fill_extrusion_layer_properties.hpp>

namespace mbgl {
namespace style {

FillExtrusionPaintProperties::Unevaluated FillExtrusionPaintProperties::Transitionable::transitioned(const TransitionParameters& parameters, Unevaluated&& prior) const {
    return Unevaluated {
        fillExtrusionOpacity.transition(parameters, std::move(prior.fillExtrusionOpacity)),
        fillExtrusionColor.transition(parameters, std::move(prior.fillExtrusionColor)),
        fillExtrusionTranslate.transition(parameters, std::move(prior.fillExtrusionTranslate)),
        fillExtrusionTranslateAnchor.transition(parameters, std::move(prior.fillExtrusionTranslateAnchor)),
        fillExtrusionPattern.transition(parameters, std::move(prior.fillExtrusionPattern)),
        fillExtrusionHeight.transition(parameters, std::move(prior.fillExtrusionHeight)),
        fillExtrusionBase.transition(parameters, std::move(prior.fillExtrusionBase)),
    };
}

FillExtrusionPaintProperties::Unevaluated FillExtrusionPaintProperties::Transitionable::untransitioned() const {
    return Unevaluated {
        Transitioning<PropertyValue<float>>(fillExtrusionOpacity.value),
        Transitioning<PropertyValue<Color>>(fillExtrusionColor.value),
        Transitioning<PropertyValue<std::array<float, 2>>>(fillExtrusionTranslate.value),
        Transitioning<PropertyValue<TranslateAnchorType>>(fillExtrusionTranslateAnchor.value),
        Transitioning<PropertyValue<std::string>>(fillExtrusionPattern.value),
        Transitioning<PropertyValue<float>>(fillExtrusionHeight.value),
        Transitioning<PropertyValue<float>>(fillExtrusionBase.value),
    };
}

bool FillExtrusionPaintProperties::Transitionable::hasDataDrivenPropertyDifference(const Transitionable& other) const {
    return false
        || fillExtrusionOpacity.value.hasDataDrivenPropertyDifference(other.fillExtrusionOpacity.value)
        || fillExtrusionColor.value.hasDataDrivenPropertyDifference(other.fillExtrusionColor.value)
        || fillExtrusionTranslate.value.hasDataDrivenPropertyDifference(other.fillExtrusionTranslate.value)
        || fillExtrusionTranslateAnchor.value.hasDataDrivenPropertyDifference(other.fillExtrusionTranslateAnchor.value)
        || fillExtrusionPattern.value.hasDataDrivenPropertyDifference(other.fillExtrusionPattern.value)
        || fillExtrusionHeight.value.hasDataDrivenPropertyDifference(other.fillExtrusionHeight.value)
        || fillExtrusionBase.value.hasDataDrivenPropertyDifference(other.fillExtrusionBase.value)
        ;
}

bool FillExtrusionPaintProperties::Unevaluated::hasTransition() const {
    return false
        || fillExtrusionOpacity.hasTransition()
        || fillExtrusionColor.hasTransition()
        || fillExtrusionTranslate.hasTransition()
        || fillExtrusionTranslateAnchor.hasTransition()
        || fillExtrusionPattern.hasTransition()
        || fillExtrusionHeight.hasTransition()
        || fillExtrusionBase.hasTransition()
        ;
}

FillExtrusionPaintProperties::PossiblyEvaluated FillExtrusionPaintProperties::Unevaluated::evaluate(const PropertyEvaluationParameters& parameters) const {
    return PossiblyEvaluated {
        fillExtrusionOpacity.evaluate(typename FillExtrusionOpacity::EvaluatorType(parameters, FillExtrusionOpacity::defaultValue()), parameters.now),
        fillExtrusionColor.evaluate(typename FillExtrusionColor::EvaluatorType(parameters, FillExtrusionColor::defaultValue()), parameters.now),
        fillExtrusionTranslate.evaluate(typename FillExtrusionTranslate::EvaluatorType(parameters, FillExtrusionTranslate::defaultValue()), parameters.now),
        fillExtrusionTranslateAnchor.evaluate(typename FillExtrusionTranslateAnchor::EvaluatorType(parameters, FillExtrusionTranslateAnchor::defaultValue()), parameters.now),
        fillExtrusionPattern.evaluate(typename FillExtrusionPattern::EvaluatorType(parameters, FillExtrusionPattern::defaultValue()), parameters.now),
        fillExtrusionHeight.evaluate(typename FillExtrusionHeight::EvaluatorType(parameters, FillExtrusionHeight::defaultValue()), parameters.now),
        fillExtrusionBase.evaluate(typename FillExtrusionBase::EvaluatorType(parameters, FillExtrusionBase::defaultValue()), parameters.now),
    };
}

FillExtrusionPaintProperties::Binders FillExtrusionPaintProperties::PossiblyEvaluated::createBinders(float z) const {
    return Binders {
        PaintPropertyBinder<Color, typename attributes::a_color::Type>::create(fillExtrusionColor, z, FillExtrusionColor::defaultValue()),
        PaintPropertyBinder<float, typename attributes::a_height::Type>::create(fillExtrusionHeight, z, FillExtrusionHeight::defaultValue()),
        PaintPropertyBinder<float, typename attributes::a_base::Type>::create(fillExtrusionBase, z, FillExtrusionBase::defaultValue()),
    };
}

std::bitset<8> FillExtrusionPaintProperties::PossiblyEvaluated::constants() const {
    std::bitset<8> result;
    result.set(0, fillExtrusionColor.isConstant());
    result.set(1, fillExtrusionHeight.isConstant());
    result.set(2, fillExtrusionBase.isConstant());
    return result;
}

std::vector<std::string> FillExtrusionPaintProperties::PossiblyEvaluated::defines() const {
    std::vector<std::string> result;
    result.push_back(fillExtrusionColor.isConstant()
        ? std::string("#define HAS_UNIFORM_") + FillExtrusionColor::Uniform::name()
        : std::string());
    result.push_back(fillExtrusionHeight.isConstant()
        ? std::string("#define HAS_UNIFORM_") + FillExtrusionHeight::Uniform::name()
        : std::string());
    result.push_back(fillExtrusionBase.isConstant()
        ? std::string("#define HAS_UNIFORM_") + FillExtrusionBase::Uniform::name()
        : std::string());
    return result;
}

void FillExtrusionPaintProperties::Binders::populateVertexVectors(const GeometryTileFeature& feature, std::size_t length) {
    fillExtrusionColor->populateVertexVector(feature, length);
    fillExtrusionHeight->populateVertexVector(feature, length);
    fillExtrusionBase->populateVertexVector(feature, length);
}

void FillExtrusionPaintProperties::Binders::upload(gl::Context& context) {
    fillExtrusionColor->upload(context);
    fillExtrusionHeight->upload(context);
    fillExtrusionBase->upload(context);
}

FillExtrusionPaintProperties::Binders::AttributeBindings FillExtrusionPaintProperties::Binders::attributeBindings(const PossiblyEvaluated& currentProperties) const {
    return AttributeBindings {
        fillExtrusionColor->attributeBinding(currentProperties.fillExtrusionColor),
        fillExtrusionHeight->attributeBinding(currentProperties.fillExtrusionHeight),
        fillExtrusionBase->attributeBinding(currentProperties.fillExtrusionBase),
    };
}

FillExtrusionPaintProperties::Binders::UniformValues FillExtrusionPaintProperties::Binders::uniformValues(float currentZoom, const PossiblyEvaluated& currentProperties) const {
    return UniformValues {
        typename InterpolationUniform<attributes::a_color>::Value {
            fillExtrusionColor->interpolationFactor(currentZoom)
        },
        typename InterpolationUniform<attributes::a_height>::Value {
            fillExtrusionHeight->interpolationFactor(currentZoom)
        },
        typename InterpolationUniform<attributes::a_base>::Value {
            fillExtrusionBase->interpolationFactor(currentZoom)
        },
        typename uniforms::u_color::Value {
            fillExtrusionColor->uniformValue(currentProperties.fillExtrusionColor)
        },
        typename uniforms::u_height::Value {
            fillExtrusionHeight->uniformValue(currentProperties.fillExtrusionHeight)
        },
        typename uniforms::u_base::Value {
            fillExtrusionBase->uniformValue(currentProperties.fillExtrusionBase)
        },
    };
}


} // namespace style
} // namespace mbgl
