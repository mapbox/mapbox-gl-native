// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/circle_layer_properties.hpp>

namespace mbgl {
namespace style {

CirclePaintProperties::Unevaluated CirclePaintProperties::Transitionable::transitioned(const TransitionParameters& parameters, Unevaluated&& prior) const {
    return Unevaluated {
        circleRadius.transition(parameters, std::move(prior.circleRadius)),
        circleColor.transition(parameters, std::move(prior.circleColor)),
        circleBlur.transition(parameters, std::move(prior.circleBlur)),
        circleOpacity.transition(parameters, std::move(prior.circleOpacity)),
        circleTranslate.transition(parameters, std::move(prior.circleTranslate)),
        circleTranslateAnchor.transition(parameters, std::move(prior.circleTranslateAnchor)),
        circlePitchScale.transition(parameters, std::move(prior.circlePitchScale)),
        circlePitchAlignment.transition(parameters, std::move(prior.circlePitchAlignment)),
        circleStrokeWidth.transition(parameters, std::move(prior.circleStrokeWidth)),
        circleStrokeColor.transition(parameters, std::move(prior.circleStrokeColor)),
        circleStrokeOpacity.transition(parameters, std::move(prior.circleStrokeOpacity)),
    };
}

CirclePaintProperties::Unevaluated CirclePaintProperties::Transitionable::untransitioned() const {
    return Unevaluated {
        Transitioning<PropertyValue<float>>(circleRadius.value),
        Transitioning<PropertyValue<Color>>(circleColor.value),
        Transitioning<PropertyValue<float>>(circleBlur.value),
        Transitioning<PropertyValue<float>>(circleOpacity.value),
        Transitioning<PropertyValue<std::array<float, 2>>>(circleTranslate.value),
        Transitioning<PropertyValue<TranslateAnchorType>>(circleTranslateAnchor.value),
        Transitioning<PropertyValue<CirclePitchScaleType>>(circlePitchScale.value),
        Transitioning<PropertyValue<AlignmentType>>(circlePitchAlignment.value),
        Transitioning<PropertyValue<float>>(circleStrokeWidth.value),
        Transitioning<PropertyValue<Color>>(circleStrokeColor.value),
        Transitioning<PropertyValue<float>>(circleStrokeOpacity.value),
    };
}

bool CirclePaintProperties::Transitionable::hasDataDrivenPropertyDifference(const Transitionable& other) const {
    return false
        || circleRadius.value.hasDataDrivenPropertyDifference(other.circleRadius.value)
        || circleColor.value.hasDataDrivenPropertyDifference(other.circleColor.value)
        || circleBlur.value.hasDataDrivenPropertyDifference(other.circleBlur.value)
        || circleOpacity.value.hasDataDrivenPropertyDifference(other.circleOpacity.value)
        || circleTranslate.value.hasDataDrivenPropertyDifference(other.circleTranslate.value)
        || circleTranslateAnchor.value.hasDataDrivenPropertyDifference(other.circleTranslateAnchor.value)
        || circlePitchScale.value.hasDataDrivenPropertyDifference(other.circlePitchScale.value)
        || circlePitchAlignment.value.hasDataDrivenPropertyDifference(other.circlePitchAlignment.value)
        || circleStrokeWidth.value.hasDataDrivenPropertyDifference(other.circleStrokeWidth.value)
        || circleStrokeColor.value.hasDataDrivenPropertyDifference(other.circleStrokeColor.value)
        || circleStrokeOpacity.value.hasDataDrivenPropertyDifference(other.circleStrokeOpacity.value)
        ;
}

bool CirclePaintProperties::Unevaluated::hasTransition() const {
    return false
        || circleRadius.hasTransition()
        || circleColor.hasTransition()
        || circleBlur.hasTransition()
        || circleOpacity.hasTransition()
        || circleTranslate.hasTransition()
        || circleTranslateAnchor.hasTransition()
        || circlePitchScale.hasTransition()
        || circlePitchAlignment.hasTransition()
        || circleStrokeWidth.hasTransition()
        || circleStrokeColor.hasTransition()
        || circleStrokeOpacity.hasTransition()
        ;
}

CirclePaintProperties::PossiblyEvaluated CirclePaintProperties::Unevaluated::evaluate(const PropertyEvaluationParameters& parameters) const {
    return PossiblyEvaluated {
        circleRadius.evaluate(typename CircleRadius::EvaluatorType(parameters, CircleRadius::defaultValue()), parameters.now),
        circleColor.evaluate(typename CircleColor::EvaluatorType(parameters, CircleColor::defaultValue()), parameters.now),
        circleBlur.evaluate(typename CircleBlur::EvaluatorType(parameters, CircleBlur::defaultValue()), parameters.now),
        circleOpacity.evaluate(typename CircleOpacity::EvaluatorType(parameters, CircleOpacity::defaultValue()), parameters.now),
        circleTranslate.evaluate(typename CircleTranslate::EvaluatorType(parameters, CircleTranslate::defaultValue()), parameters.now),
        circleTranslateAnchor.evaluate(typename CircleTranslateAnchor::EvaluatorType(parameters, CircleTranslateAnchor::defaultValue()), parameters.now),
        circlePitchScale.evaluate(typename CirclePitchScale::EvaluatorType(parameters, CirclePitchScale::defaultValue()), parameters.now),
        circlePitchAlignment.evaluate(typename CirclePitchAlignment::EvaluatorType(parameters, CirclePitchAlignment::defaultValue()), parameters.now),
        circleStrokeWidth.evaluate(typename CircleStrokeWidth::EvaluatorType(parameters, CircleStrokeWidth::defaultValue()), parameters.now),
        circleStrokeColor.evaluate(typename CircleStrokeColor::EvaluatorType(parameters, CircleStrokeColor::defaultValue()), parameters.now),
        circleStrokeOpacity.evaluate(typename CircleStrokeOpacity::EvaluatorType(parameters, CircleStrokeOpacity::defaultValue()), parameters.now),
    };
}

CirclePaintProperties::Binders CirclePaintProperties::PossiblyEvaluated::createBinders(float z) const {
    return Binders {
        PaintPropertyBinder<float, typename attributes::a_radius::Type>::create(circleRadius, z, CircleRadius::defaultValue()),
        PaintPropertyBinder<Color, typename attributes::a_color::Type>::create(circleColor, z, CircleColor::defaultValue()),
        PaintPropertyBinder<float, typename attributes::a_blur::Type>::create(circleBlur, z, CircleBlur::defaultValue()),
        PaintPropertyBinder<float, typename attributes::a_opacity::Type>::create(circleOpacity, z, CircleOpacity::defaultValue()),
        PaintPropertyBinder<float, typename attributes::a_stroke_width::Type>::create(circleStrokeWidth, z, CircleStrokeWidth::defaultValue()),
        PaintPropertyBinder<Color, typename attributes::a_stroke_color::Type>::create(circleStrokeColor, z, CircleStrokeColor::defaultValue()),
        PaintPropertyBinder<float, typename attributes::a_stroke_opacity::Type>::create(circleStrokeOpacity, z, CircleStrokeOpacity::defaultValue()),
    };
}

std::bitset<8> CirclePaintProperties::PossiblyEvaluated::constants() const {
    std::bitset<8> result;
    result.set(0, circleRadius.isConstant());
    result.set(1, circleColor.isConstant());
    result.set(2, circleBlur.isConstant());
    result.set(3, circleOpacity.isConstant());
    result.set(4, circleStrokeWidth.isConstant());
    result.set(5, circleStrokeColor.isConstant());
    result.set(6, circleStrokeOpacity.isConstant());
    return result;
}

std::vector<std::string> CirclePaintProperties::PossiblyEvaluated::defines() const {
    std::vector<std::string> result;
    result.push_back(circleRadius.isConstant()
        ? std::string("#define HAS_UNIFORM_") + CircleRadius::Uniform::name()
        : std::string());
    result.push_back(circleColor.isConstant()
        ? std::string("#define HAS_UNIFORM_") + CircleColor::Uniform::name()
        : std::string());
    result.push_back(circleBlur.isConstant()
        ? std::string("#define HAS_UNIFORM_") + CircleBlur::Uniform::name()
        : std::string());
    result.push_back(circleOpacity.isConstant()
        ? std::string("#define HAS_UNIFORM_") + CircleOpacity::Uniform::name()
        : std::string());
    result.push_back(circleStrokeWidth.isConstant()
        ? std::string("#define HAS_UNIFORM_") + CircleStrokeWidth::Uniform::name()
        : std::string());
    result.push_back(circleStrokeColor.isConstant()
        ? std::string("#define HAS_UNIFORM_") + CircleStrokeColor::Uniform::name()
        : std::string());
    result.push_back(circleStrokeOpacity.isConstant()
        ? std::string("#define HAS_UNIFORM_") + CircleStrokeOpacity::Uniform::name()
        : std::string());
    return result;
}

void CirclePaintProperties::Binders::populateVertexVectors(const GeometryTileFeature& feature, std::size_t length) {
    circleRadius->populateVertexVector(feature, length);
    circleColor->populateVertexVector(feature, length);
    circleBlur->populateVertexVector(feature, length);
    circleOpacity->populateVertexVector(feature, length);
    circleStrokeWidth->populateVertexVector(feature, length);
    circleStrokeColor->populateVertexVector(feature, length);
    circleStrokeOpacity->populateVertexVector(feature, length);
}

void CirclePaintProperties::Binders::upload(gl::Context& context) {
    circleRadius->upload(context);
    circleColor->upload(context);
    circleBlur->upload(context);
    circleOpacity->upload(context);
    circleStrokeWidth->upload(context);
    circleStrokeColor->upload(context);
    circleStrokeOpacity->upload(context);
}

CirclePaintProperties::Binders::AttributeBindings CirclePaintProperties::Binders::attributeBindings(const PossiblyEvaluated& currentProperties) const {
    return AttributeBindings {
        circleRadius->attributeBinding(currentProperties.circleRadius),
        circleColor->attributeBinding(currentProperties.circleColor),
        circleBlur->attributeBinding(currentProperties.circleBlur),
        circleOpacity->attributeBinding(currentProperties.circleOpacity),
        circleStrokeWidth->attributeBinding(currentProperties.circleStrokeWidth),
        circleStrokeColor->attributeBinding(currentProperties.circleStrokeColor),
        circleStrokeOpacity->attributeBinding(currentProperties.circleStrokeOpacity),
    };
}

CirclePaintProperties::Binders::UniformValues CirclePaintProperties::Binders::uniformValues(float currentZoom, const PossiblyEvaluated& currentProperties) const {
    return UniformValues {
        typename InterpolationUniform<attributes::a_radius>::Value {
            circleRadius->interpolationFactor(currentZoom)
        },
        typename InterpolationUniform<attributes::a_color>::Value {
            circleColor->interpolationFactor(currentZoom)
        },
        typename InterpolationUniform<attributes::a_blur>::Value {
            circleBlur->interpolationFactor(currentZoom)
        },
        typename InterpolationUniform<attributes::a_opacity>::Value {
            circleOpacity->interpolationFactor(currentZoom)
        },
        typename InterpolationUniform<attributes::a_stroke_width>::Value {
            circleStrokeWidth->interpolationFactor(currentZoom)
        },
        typename InterpolationUniform<attributes::a_stroke_color>::Value {
            circleStrokeColor->interpolationFactor(currentZoom)
        },
        typename InterpolationUniform<attributes::a_stroke_opacity>::Value {
            circleStrokeOpacity->interpolationFactor(currentZoom)
        },
        typename uniforms::u_radius::Value {
            circleRadius->uniformValue(currentProperties.circleRadius)
        },
        typename uniforms::u_color::Value {
            circleColor->uniformValue(currentProperties.circleColor)
        },
        typename uniforms::u_blur::Value {
            circleBlur->uniformValue(currentProperties.circleBlur)
        },
        typename uniforms::u_opacity::Value {
            circleOpacity->uniformValue(currentProperties.circleOpacity)
        },
        typename uniforms::u_stroke_width::Value {
            circleStrokeWidth->uniformValue(currentProperties.circleStrokeWidth)
        },
        typename uniforms::u_stroke_color::Value {
            circleStrokeColor->uniformValue(currentProperties.circleStrokeColor)
        },
        typename uniforms::u_stroke_opacity::Value {
            circleStrokeOpacity->uniformValue(currentProperties.circleStrokeOpacity)
        },
    };
}


} // namespace style
} // namespace mbgl
