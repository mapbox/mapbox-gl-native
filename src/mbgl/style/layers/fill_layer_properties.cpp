// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/fill_layer_properties.hpp>

namespace mbgl {
namespace style {

FillPaintProperties::Unevaluated FillPaintProperties::Transitionable::transitioned(const TransitionParameters& parameters, Unevaluated&& prior) const {
    return Unevaluated {
        fillAntialias.transition(parameters, std::move(prior.fillAntialias)),
        fillOpacity.transition(parameters, std::move(prior.fillOpacity)),
        fillColor.transition(parameters, std::move(prior.fillColor)),
        fillOutlineColor.transition(parameters, std::move(prior.fillOutlineColor)),
        fillTranslate.transition(parameters, std::move(prior.fillTranslate)),
        fillTranslateAnchor.transition(parameters, std::move(prior.fillTranslateAnchor)),
        fillPattern.transition(parameters, std::move(prior.fillPattern)),
    };
}

FillPaintProperties::Unevaluated FillPaintProperties::Transitionable::untransitioned() const {
    return Unevaluated {
        Transitioning<PropertyValue<bool>>(fillAntialias.value),
        Transitioning<PropertyValue<float>>(fillOpacity.value),
        Transitioning<PropertyValue<Color>>(fillColor.value),
        Transitioning<PropertyValue<Color>>(fillOutlineColor.value),
        Transitioning<PropertyValue<std::array<float, 2>>>(fillTranslate.value),
        Transitioning<PropertyValue<TranslateAnchorType>>(fillTranslateAnchor.value),
        Transitioning<PropertyValue<std::string>>(fillPattern.value),
    };
}

bool FillPaintProperties::Transitionable::hasDataDrivenPropertyDifference(const Transitionable& other) const {
    return false
        || fillAntialias.value.hasDataDrivenPropertyDifference(other.fillAntialias.value)
        || fillOpacity.value.hasDataDrivenPropertyDifference(other.fillOpacity.value)
        || fillColor.value.hasDataDrivenPropertyDifference(other.fillColor.value)
        || fillOutlineColor.value.hasDataDrivenPropertyDifference(other.fillOutlineColor.value)
        || fillTranslate.value.hasDataDrivenPropertyDifference(other.fillTranslate.value)
        || fillTranslateAnchor.value.hasDataDrivenPropertyDifference(other.fillTranslateAnchor.value)
        || fillPattern.value.hasDataDrivenPropertyDifference(other.fillPattern.value)
        ;
}

bool FillPaintProperties::Unevaluated::hasTransition() const {
    return false
        || fillAntialias.hasTransition()
        || fillOpacity.hasTransition()
        || fillColor.hasTransition()
        || fillOutlineColor.hasTransition()
        || fillTranslate.hasTransition()
        || fillTranslateAnchor.hasTransition()
        || fillPattern.hasTransition()
        ;
}

FillPaintProperties::PossiblyEvaluated FillPaintProperties::Unevaluated::evaluate(const PropertyEvaluationParameters& parameters) const {
    return PossiblyEvaluated {
        fillAntialias.evaluate(typename FillAntialias::EvaluatorType(parameters, FillAntialias::defaultValue()), parameters.now),
        fillOpacity.evaluate(typename FillOpacity::EvaluatorType(parameters, FillOpacity::defaultValue()), parameters.now),
        fillColor.evaluate(typename FillColor::EvaluatorType(parameters, FillColor::defaultValue()), parameters.now),
        fillOutlineColor.evaluate(typename FillOutlineColor::EvaluatorType(parameters, FillOutlineColor::defaultValue()), parameters.now),
        fillTranslate.evaluate(typename FillTranslate::EvaluatorType(parameters, FillTranslate::defaultValue()), parameters.now),
        fillTranslateAnchor.evaluate(typename FillTranslateAnchor::EvaluatorType(parameters, FillTranslateAnchor::defaultValue()), parameters.now),
        fillPattern.evaluate(typename FillPattern::EvaluatorType(parameters, FillPattern::defaultValue()), parameters.now),
    };
}

FillPaintProperties::Binders FillPaintProperties::PossiblyEvaluated::createBinders(float z) const {
    return Binders {
        PaintPropertyBinder<float, typename attributes::a_opacity::Type>::create(fillOpacity, z, FillOpacity::defaultValue()),
        PaintPropertyBinder<Color, typename attributes::a_color::Type>::create(fillColor, z, FillColor::defaultValue()),
        PaintPropertyBinder<Color, typename attributes::a_outline_color::Type>::create(fillOutlineColor, z, FillOutlineColor::defaultValue()),
    };
}

std::bitset<8> FillPaintProperties::PossiblyEvaluated::constants() const {
    std::bitset<8> result;
    result.set(0, fillOpacity.isConstant());
    result.set(1, fillColor.isConstant());
    result.set(2, fillOutlineColor.isConstant());
    return result;
}

std::vector<std::string> FillPaintProperties::PossiblyEvaluated::defines() const {
    std::vector<std::string> result;
    result.push_back(fillOpacity.isConstant()
        ? std::string("#define HAS_UNIFORM_") + FillOpacity::Uniform::name()
        : std::string());
    result.push_back(fillColor.isConstant()
        ? std::string("#define HAS_UNIFORM_") + FillColor::Uniform::name()
        : std::string());
    result.push_back(fillOutlineColor.isConstant()
        ? std::string("#define HAS_UNIFORM_") + FillOutlineColor::Uniform::name()
        : std::string());
    return result;
}

void FillPaintProperties::Binders::populateVertexVectors(const GeometryTileFeature& feature, std::size_t length) {
    fillOpacity->populateVertexVector(feature, length);
    fillColor->populateVertexVector(feature, length);
    fillOutlineColor->populateVertexVector(feature, length);
}

void FillPaintProperties::Binders::upload(gl::Context& context) {
    fillOpacity->upload(context);
    fillColor->upload(context);
    fillOutlineColor->upload(context);
}

FillPaintProperties::Binders::AttributeBindings FillPaintProperties::Binders::attributeBindings(const PossiblyEvaluated& currentProperties) const {
    return AttributeBindings {
        fillOpacity->attributeBinding(currentProperties.fillOpacity),
        fillColor->attributeBinding(currentProperties.fillColor),
        fillOutlineColor->attributeBinding(currentProperties.fillOutlineColor),
    };
}

FillPaintProperties::Binders::UniformValues FillPaintProperties::Binders::uniformValues(float currentZoom, const PossiblyEvaluated& currentProperties) const {
    return UniformValues {
        typename InterpolationUniform<attributes::a_opacity>::Value {
            fillOpacity->interpolationFactor(currentZoom)
        },
        typename InterpolationUniform<attributes::a_color>::Value {
            fillColor->interpolationFactor(currentZoom)
        },
        typename InterpolationUniform<attributes::a_outline_color>::Value {
            fillOutlineColor->interpolationFactor(currentZoom)
        },
        typename uniforms::u_opacity::Value {
            fillOpacity->uniformValue(currentProperties.fillOpacity)
        },
        typename uniforms::u_color::Value {
            fillColor->uniformValue(currentProperties.fillColor)
        },
        typename uniforms::u_outline_color::Value {
            fillOutlineColor->uniformValue(currentProperties.fillOutlineColor)
        },
    };
}


} // namespace style
} // namespace mbgl
