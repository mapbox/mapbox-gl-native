// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/line_layer_properties.hpp>

namespace mbgl {
namespace style {

bool operator==(const LineLayoutProperties::Unevaluated& lhs, const LineLayoutProperties::Unevaluated& rhs) {
    return true
        && lhs.lineCap == rhs.lineCap
        && lhs.lineJoin == rhs.lineJoin
        && lhs.lineMiterLimit == rhs.lineMiterLimit
        && lhs.lineRoundLimit == rhs.lineRoundLimit
        ;
}

LineLayoutProperties::PossiblyEvaluated LineLayoutProperties::Unevaluated::evaluate(const PropertyEvaluationParameters& parameters) const {
    return PossiblyEvaluated {
        lineCap.evaluate(typename LineCap::EvaluatorType(parameters, LineCap::defaultValue()), parameters.now),
        lineJoin.evaluate(typename LineJoin::EvaluatorType(parameters, LineJoin::defaultValue()), parameters.now),
        lineMiterLimit.evaluate(typename LineMiterLimit::EvaluatorType(parameters, LineMiterLimit::defaultValue()), parameters.now),
        lineRoundLimit.evaluate(typename LineRoundLimit::EvaluatorType(parameters, LineRoundLimit::defaultValue()), parameters.now),
    };
}

LineLayoutProperties::Evaluated LineLayoutProperties::PossiblyEvaluated::evaluate(float z, const GeometryTileFeature& feature) const {
    return Evaluated {
        lineCap,
        lineJoin.evaluate(feature, z, LineJoin::defaultValue()),
        lineMiterLimit,
        lineRoundLimit,
    };
}

LinePaintProperties::Unevaluated LinePaintProperties::Transitionable::transitioned(const TransitionParameters& parameters, Unevaluated&& prior) const {
    return Unevaluated {
        lineOpacity.transition(parameters, std::move(prior.lineOpacity)),
        lineColor.transition(parameters, std::move(prior.lineColor)),
        lineTranslate.transition(parameters, std::move(prior.lineTranslate)),
        lineTranslateAnchor.transition(parameters, std::move(prior.lineTranslateAnchor)),
        lineWidth.transition(parameters, std::move(prior.lineWidth)),
        lineGapWidth.transition(parameters, std::move(prior.lineGapWidth)),
        lineOffset.transition(parameters, std::move(prior.lineOffset)),
        lineBlur.transition(parameters, std::move(prior.lineBlur)),
        lineDasharray.transition(parameters, std::move(prior.lineDasharray)),
        linePattern.transition(parameters, std::move(prior.linePattern)),
    };
}

LinePaintProperties::Unevaluated LinePaintProperties::Transitionable::untransitioned() const {
    return Unevaluated {
        Transitioning<PropertyValue<float>>(lineOpacity.value),
        Transitioning<PropertyValue<Color>>(lineColor.value),
        Transitioning<PropertyValue<std::array<float, 2>>>(lineTranslate.value),
        Transitioning<PropertyValue<TranslateAnchorType>>(lineTranslateAnchor.value),
        Transitioning<PropertyValue<float>>(lineWidth.value),
        Transitioning<PropertyValue<float>>(lineGapWidth.value),
        Transitioning<PropertyValue<float>>(lineOffset.value),
        Transitioning<PropertyValue<float>>(lineBlur.value),
        Transitioning<PropertyValue<std::vector<float>>>(lineDasharray.value),
        Transitioning<PropertyValue<std::string>>(linePattern.value),
    };
}

bool LinePaintProperties::Transitionable::hasDataDrivenPropertyDifference(const Transitionable& other) const {
    return false
        || lineOpacity.value.hasDataDrivenPropertyDifference(other.lineOpacity.value)
        || lineColor.value.hasDataDrivenPropertyDifference(other.lineColor.value)
        || lineTranslate.value.hasDataDrivenPropertyDifference(other.lineTranslate.value)
        || lineTranslateAnchor.value.hasDataDrivenPropertyDifference(other.lineTranslateAnchor.value)
        || lineWidth.value.hasDataDrivenPropertyDifference(other.lineWidth.value)
        || lineGapWidth.value.hasDataDrivenPropertyDifference(other.lineGapWidth.value)
        || lineOffset.value.hasDataDrivenPropertyDifference(other.lineOffset.value)
        || lineBlur.value.hasDataDrivenPropertyDifference(other.lineBlur.value)
        || lineDasharray.value.hasDataDrivenPropertyDifference(other.lineDasharray.value)
        || linePattern.value.hasDataDrivenPropertyDifference(other.linePattern.value)
        ;
}

bool LinePaintProperties::Unevaluated::hasTransition() const {
    return false
        || lineOpacity.hasTransition()
        || lineColor.hasTransition()
        || lineTranslate.hasTransition()
        || lineTranslateAnchor.hasTransition()
        || lineWidth.hasTransition()
        || lineGapWidth.hasTransition()
        || lineOffset.hasTransition()
        || lineBlur.hasTransition()
        || lineDasharray.hasTransition()
        || linePattern.hasTransition()
        ;
}

LinePaintProperties::PossiblyEvaluated LinePaintProperties::Unevaluated::evaluate(const PropertyEvaluationParameters& parameters) const {
    return PossiblyEvaluated {
        lineOpacity.evaluate(typename LineOpacity::EvaluatorType(parameters, LineOpacity::defaultValue()), parameters.now),
        lineColor.evaluate(typename LineColor::EvaluatorType(parameters, LineColor::defaultValue()), parameters.now),
        lineTranslate.evaluate(typename LineTranslate::EvaluatorType(parameters, LineTranslate::defaultValue()), parameters.now),
        lineTranslateAnchor.evaluate(typename LineTranslateAnchor::EvaluatorType(parameters, LineTranslateAnchor::defaultValue()), parameters.now),
        lineWidth.evaluate(typename LineWidth::EvaluatorType(parameters, LineWidth::defaultValue()), parameters.now),
        lineGapWidth.evaluate(typename LineGapWidth::EvaluatorType(parameters, LineGapWidth::defaultValue()), parameters.now),
        lineOffset.evaluate(typename LineOffset::EvaluatorType(parameters, LineOffset::defaultValue()), parameters.now),
        lineBlur.evaluate(typename LineBlur::EvaluatorType(parameters, LineBlur::defaultValue()), parameters.now),
        lineDasharray.evaluate(typename LineDasharray::EvaluatorType(parameters, LineDasharray::defaultValue()), parameters.now),
        linePattern.evaluate(typename LinePattern::EvaluatorType(parameters, LinePattern::defaultValue()), parameters.now),
    };
}

LinePaintProperties::Binders LinePaintProperties::PossiblyEvaluated::createBinders(float z) const {
    return Binders {
        PaintPropertyBinder<float, typename attributes::a_opacity::Type>::create(lineOpacity, z, LineOpacity::defaultValue()),
        PaintPropertyBinder<Color, typename attributes::a_color::Type>::create(lineColor, z, LineColor::defaultValue()),
        PaintPropertyBinder<float, typename attributes::a_width::Type>::create(lineWidth, z, LineWidth::defaultValue()),
        PaintPropertyBinder<float, typename attributes::a_gapwidth::Type>::create(lineGapWidth, z, LineGapWidth::defaultValue()),
        PaintPropertyBinder<float, typename attributes::a_offset<1>::Type>::create(lineOffset, z, LineOffset::defaultValue()),
        PaintPropertyBinder<float, typename attributes::a_blur::Type>::create(lineBlur, z, LineBlur::defaultValue()),
    };
}

std::bitset<8> LinePaintProperties::PossiblyEvaluated::constants() const {
    std::bitset<8> result;
    result.set(0, lineOpacity.isConstant());
    result.set(1, lineColor.isConstant());
    result.set(2, lineWidth.isConstant());
    result.set(3, lineGapWidth.isConstant());
    result.set(4, lineOffset.isConstant());
    result.set(5, lineBlur.isConstant());
    return result;
}

std::vector<std::string> LinePaintProperties::PossiblyEvaluated::defines() const {
    std::vector<std::string> result;
    result.push_back(lineOpacity.isConstant()
        ? std::string("#define HAS_UNIFORM_") + LineOpacity::Uniform::name()
        : std::string());
    result.push_back(lineColor.isConstant()
        ? std::string("#define HAS_UNIFORM_") + LineColor::Uniform::name()
        : std::string());
    result.push_back(lineWidth.isConstant()
        ? std::string("#define HAS_UNIFORM_") + LineWidth::Uniform::name()
        : std::string());
    result.push_back(lineGapWidth.isConstant()
        ? std::string("#define HAS_UNIFORM_") + LineGapWidth::Uniform::name()
        : std::string());
    result.push_back(lineOffset.isConstant()
        ? std::string("#define HAS_UNIFORM_") + LineOffset::Uniform::name()
        : std::string());
    result.push_back(lineBlur.isConstant()
        ? std::string("#define HAS_UNIFORM_") + LineBlur::Uniform::name()
        : std::string());
    return result;
}

void LinePaintProperties::Binders::populateVertexVectors(const GeometryTileFeature& feature, std::size_t length) {
    lineOpacity->populateVertexVector(feature, length);
    lineColor->populateVertexVector(feature, length);
    lineWidth->populateVertexVector(feature, length);
    lineGapWidth->populateVertexVector(feature, length);
    lineOffset->populateVertexVector(feature, length);
    lineBlur->populateVertexVector(feature, length);
}

void LinePaintProperties::Binders::upload(gl::Context& context) {
    lineOpacity->upload(context);
    lineColor->upload(context);
    lineWidth->upload(context);
    lineGapWidth->upload(context);
    lineOffset->upload(context);
    lineBlur->upload(context);
}

LinePaintProperties::Binders::AttributeBindings LinePaintProperties::Binders::attributeBindings(const PossiblyEvaluated& currentProperties) const {
    return AttributeBindings {
        lineOpacity->attributeBinding(currentProperties.lineOpacity),
        lineColor->attributeBinding(currentProperties.lineColor),
        lineWidth->attributeBinding(currentProperties.lineWidth),
        lineGapWidth->attributeBinding(currentProperties.lineGapWidth),
        lineOffset->attributeBinding(currentProperties.lineOffset),
        lineBlur->attributeBinding(currentProperties.lineBlur),
    };
}

LinePaintProperties::Binders::UniformValues LinePaintProperties::Binders::uniformValues(float currentZoom, const PossiblyEvaluated& currentProperties) const {
    return UniformValues {
        typename InterpolationUniform<attributes::a_opacity>::Value {
            lineOpacity->interpolationFactor(currentZoom)
        },
        typename InterpolationUniform<attributes::a_color>::Value {
            lineColor->interpolationFactor(currentZoom)
        },
        typename InterpolationUniform<attributes::a_width>::Value {
            lineWidth->interpolationFactor(currentZoom)
        },
        typename InterpolationUniform<attributes::a_gapwidth>::Value {
            lineGapWidth->interpolationFactor(currentZoom)
        },
        typename InterpolationUniform<attributes::a_offset<1>>::Value {
            lineOffset->interpolationFactor(currentZoom)
        },
        typename InterpolationUniform<attributes::a_blur>::Value {
            lineBlur->interpolationFactor(currentZoom)
        },
        typename uniforms::u_opacity::Value {
            lineOpacity->uniformValue(currentProperties.lineOpacity)
        },
        typename uniforms::u_color::Value {
            lineColor->uniformValue(currentProperties.lineColor)
        },
        typename uniforms::u_width::Value {
            lineWidth->uniformValue(currentProperties.lineWidth)
        },
        typename uniforms::u_gapwidth::Value {
            lineGapWidth->uniformValue(currentProperties.lineGapWidth)
        },
        typename uniforms::u_offset::Value {
            lineOffset->uniformValue(currentProperties.lineOffset)
        },
        typename uniforms::u_blur::Value {
            lineBlur->uniformValue(currentProperties.lineBlur)
        },
    };
}


} // namespace style
} // namespace mbgl
