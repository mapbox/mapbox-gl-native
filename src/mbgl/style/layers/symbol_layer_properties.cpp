// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/symbol_layer_properties.hpp>

namespace mbgl {
namespace style {

bool operator==(const SymbolLayoutProperties::Unevaluated& lhs, const SymbolLayoutProperties::Unevaluated& rhs) {
    return true
        && lhs.symbolPlacement == rhs.symbolPlacement
        && lhs.symbolSpacing == rhs.symbolSpacing
        && lhs.symbolAvoidEdges == rhs.symbolAvoidEdges
        && lhs.iconAllowOverlap == rhs.iconAllowOverlap
        && lhs.iconIgnorePlacement == rhs.iconIgnorePlacement
        && lhs.iconOptional == rhs.iconOptional
        && lhs.iconRotationAlignment == rhs.iconRotationAlignment
        && lhs.iconSize == rhs.iconSize
        && lhs.iconTextFit == rhs.iconTextFit
        && lhs.iconTextFitPadding == rhs.iconTextFitPadding
        && lhs.iconImage == rhs.iconImage
        && lhs.iconRotate == rhs.iconRotate
        && lhs.iconPadding == rhs.iconPadding
        && lhs.iconKeepUpright == rhs.iconKeepUpright
        && lhs.iconOffset == rhs.iconOffset
        && lhs.iconAnchor == rhs.iconAnchor
        && lhs.iconPitchAlignment == rhs.iconPitchAlignment
        && lhs.textPitchAlignment == rhs.textPitchAlignment
        && lhs.textRotationAlignment == rhs.textRotationAlignment
        && lhs.textField == rhs.textField
        && lhs.textFont == rhs.textFont
        && lhs.textSize == rhs.textSize
        && lhs.textMaxWidth == rhs.textMaxWidth
        && lhs.textLineHeight == rhs.textLineHeight
        && lhs.textLetterSpacing == rhs.textLetterSpacing
        && lhs.textJustify == rhs.textJustify
        && lhs.textAnchor == rhs.textAnchor
        && lhs.textMaxAngle == rhs.textMaxAngle
        && lhs.textRotate == rhs.textRotate
        && lhs.textPadding == rhs.textPadding
        && lhs.textKeepUpright == rhs.textKeepUpright
        && lhs.textTransform == rhs.textTransform
        && lhs.textOffset == rhs.textOffset
        && lhs.textAllowOverlap == rhs.textAllowOverlap
        && lhs.textIgnorePlacement == rhs.textIgnorePlacement
        && lhs.textOptional == rhs.textOptional
        ;
}

SymbolLayoutProperties::PossiblyEvaluated SymbolLayoutProperties::Unevaluated::evaluate(const PropertyEvaluationParameters& parameters) const {
    return PossiblyEvaluated {
        symbolPlacement.evaluate(typename SymbolPlacement::EvaluatorType(parameters, SymbolPlacement::defaultValue()), parameters.now),
        symbolSpacing.evaluate(typename SymbolSpacing::EvaluatorType(parameters, SymbolSpacing::defaultValue()), parameters.now),
        symbolAvoidEdges.evaluate(typename SymbolAvoidEdges::EvaluatorType(parameters, SymbolAvoidEdges::defaultValue()), parameters.now),
        iconAllowOverlap.evaluate(typename IconAllowOverlap::EvaluatorType(parameters, IconAllowOverlap::defaultValue()), parameters.now),
        iconIgnorePlacement.evaluate(typename IconIgnorePlacement::EvaluatorType(parameters, IconIgnorePlacement::defaultValue()), parameters.now),
        iconOptional.evaluate(typename IconOptional::EvaluatorType(parameters, IconOptional::defaultValue()), parameters.now),
        iconRotationAlignment.evaluate(typename IconRotationAlignment::EvaluatorType(parameters, IconRotationAlignment::defaultValue()), parameters.now),
        iconSize.evaluate(typename IconSize::EvaluatorType(parameters, IconSize::defaultValue()), parameters.now),
        iconTextFit.evaluate(typename IconTextFit::EvaluatorType(parameters, IconTextFit::defaultValue()), parameters.now),
        iconTextFitPadding.evaluate(typename IconTextFitPadding::EvaluatorType(parameters, IconTextFitPadding::defaultValue()), parameters.now),
        iconImage.evaluate(typename IconImage::EvaluatorType(parameters, IconImage::defaultValue()), parameters.now),
        iconRotate.evaluate(typename IconRotate::EvaluatorType(parameters, IconRotate::defaultValue()), parameters.now),
        iconPadding.evaluate(typename IconPadding::EvaluatorType(parameters, IconPadding::defaultValue()), parameters.now),
        iconKeepUpright.evaluate(typename IconKeepUpright::EvaluatorType(parameters, IconKeepUpright::defaultValue()), parameters.now),
        iconOffset.evaluate(typename IconOffset::EvaluatorType(parameters, IconOffset::defaultValue()), parameters.now),
        iconAnchor.evaluate(typename IconAnchor::EvaluatorType(parameters, IconAnchor::defaultValue()), parameters.now),
        iconPitchAlignment.evaluate(typename IconPitchAlignment::EvaluatorType(parameters, IconPitchAlignment::defaultValue()), parameters.now),
        textPitchAlignment.evaluate(typename TextPitchAlignment::EvaluatorType(parameters, TextPitchAlignment::defaultValue()), parameters.now),
        textRotationAlignment.evaluate(typename TextRotationAlignment::EvaluatorType(parameters, TextRotationAlignment::defaultValue()), parameters.now),
        textField.evaluate(typename TextField::EvaluatorType(parameters, TextField::defaultValue()), parameters.now),
        textFont.evaluate(typename TextFont::EvaluatorType(parameters, TextFont::defaultValue()), parameters.now),
        textSize.evaluate(typename TextSize::EvaluatorType(parameters, TextSize::defaultValue()), parameters.now),
        textMaxWidth.evaluate(typename TextMaxWidth::EvaluatorType(parameters, TextMaxWidth::defaultValue()), parameters.now),
        textLineHeight.evaluate(typename TextLineHeight::EvaluatorType(parameters, TextLineHeight::defaultValue()), parameters.now),
        textLetterSpacing.evaluate(typename TextLetterSpacing::EvaluatorType(parameters, TextLetterSpacing::defaultValue()), parameters.now),
        textJustify.evaluate(typename TextJustify::EvaluatorType(parameters, TextJustify::defaultValue()), parameters.now),
        textAnchor.evaluate(typename TextAnchor::EvaluatorType(parameters, TextAnchor::defaultValue()), parameters.now),
        textMaxAngle.evaluate(typename TextMaxAngle::EvaluatorType(parameters, TextMaxAngle::defaultValue()), parameters.now),
        textRotate.evaluate(typename TextRotate::EvaluatorType(parameters, TextRotate::defaultValue()), parameters.now),
        textPadding.evaluate(typename TextPadding::EvaluatorType(parameters, TextPadding::defaultValue()), parameters.now),
        textKeepUpright.evaluate(typename TextKeepUpright::EvaluatorType(parameters, TextKeepUpright::defaultValue()), parameters.now),
        textTransform.evaluate(typename TextTransform::EvaluatorType(parameters, TextTransform::defaultValue()), parameters.now),
        textOffset.evaluate(typename TextOffset::EvaluatorType(parameters, TextOffset::defaultValue()), parameters.now),
        textAllowOverlap.evaluate(typename TextAllowOverlap::EvaluatorType(parameters, TextAllowOverlap::defaultValue()), parameters.now),
        textIgnorePlacement.evaluate(typename TextIgnorePlacement::EvaluatorType(parameters, TextIgnorePlacement::defaultValue()), parameters.now),
        textOptional.evaluate(typename TextOptional::EvaluatorType(parameters, TextOptional::defaultValue()), parameters.now),
    };
}

SymbolLayoutProperties::Evaluated SymbolLayoutProperties::PossiblyEvaluated::evaluate(float z, const GeometryTileFeature& feature) const {
    return Evaluated {
        symbolPlacement,
        symbolSpacing,
        symbolAvoidEdges,
        iconAllowOverlap,
        iconIgnorePlacement,
        iconOptional,
        iconRotationAlignment,
        iconSize.evaluate(feature, z, IconSize::defaultValue()),
        iconTextFit,
        iconTextFitPadding,
        iconImage.evaluate(feature, z, IconImage::defaultValue()),
        iconRotate.evaluate(feature, z, IconRotate::defaultValue()),
        iconPadding,
        iconKeepUpright,
        iconOffset.evaluate(feature, z, IconOffset::defaultValue()),
        iconAnchor.evaluate(feature, z, IconAnchor::defaultValue()),
        iconPitchAlignment,
        textPitchAlignment,
        textRotationAlignment,
        textField.evaluate(feature, z, TextField::defaultValue()),
        textFont.evaluate(feature, z, TextFont::defaultValue()),
        textSize.evaluate(feature, z, TextSize::defaultValue()),
        textMaxWidth.evaluate(feature, z, TextMaxWidth::defaultValue()),
        textLineHeight,
        textLetterSpacing.evaluate(feature, z, TextLetterSpacing::defaultValue()),
        textJustify.evaluate(feature, z, TextJustify::defaultValue()),
        textAnchor.evaluate(feature, z, TextAnchor::defaultValue()),
        textMaxAngle,
        textRotate.evaluate(feature, z, TextRotate::defaultValue()),
        textPadding,
        textKeepUpright,
        textTransform.evaluate(feature, z, TextTransform::defaultValue()),
        textOffset.evaluate(feature, z, TextOffset::defaultValue()),
        textAllowOverlap,
        textIgnorePlacement,
        textOptional,
    };
}

SymbolPaintProperties::Unevaluated SymbolPaintProperties::Transitionable::transitioned(const TransitionParameters& parameters, Unevaluated&& prior) const {
    return Unevaluated {
        iconOpacity.transition(parameters, std::move(prior.iconOpacity)),
        iconColor.transition(parameters, std::move(prior.iconColor)),
        iconHaloColor.transition(parameters, std::move(prior.iconHaloColor)),
        iconHaloWidth.transition(parameters, std::move(prior.iconHaloWidth)),
        iconHaloBlur.transition(parameters, std::move(prior.iconHaloBlur)),
        iconTranslate.transition(parameters, std::move(prior.iconTranslate)),
        iconTranslateAnchor.transition(parameters, std::move(prior.iconTranslateAnchor)),
        textOpacity.transition(parameters, std::move(prior.textOpacity)),
        textColor.transition(parameters, std::move(prior.textColor)),
        textHaloColor.transition(parameters, std::move(prior.textHaloColor)),
        textHaloWidth.transition(parameters, std::move(prior.textHaloWidth)),
        textHaloBlur.transition(parameters, std::move(prior.textHaloBlur)),
        textTranslate.transition(parameters, std::move(prior.textTranslate)),
        textTranslateAnchor.transition(parameters, std::move(prior.textTranslateAnchor)),
    };
}

SymbolPaintProperties::Unevaluated SymbolPaintProperties::Transitionable::untransitioned() const {
    return Unevaluated {
        Transitioning<PropertyValue<float>>(iconOpacity.value),
        Transitioning<PropertyValue<Color>>(iconColor.value),
        Transitioning<PropertyValue<Color>>(iconHaloColor.value),
        Transitioning<PropertyValue<float>>(iconHaloWidth.value),
        Transitioning<PropertyValue<float>>(iconHaloBlur.value),
        Transitioning<PropertyValue<std::array<float, 2>>>(iconTranslate.value),
        Transitioning<PropertyValue<TranslateAnchorType>>(iconTranslateAnchor.value),
        Transitioning<PropertyValue<float>>(textOpacity.value),
        Transitioning<PropertyValue<Color>>(textColor.value),
        Transitioning<PropertyValue<Color>>(textHaloColor.value),
        Transitioning<PropertyValue<float>>(textHaloWidth.value),
        Transitioning<PropertyValue<float>>(textHaloBlur.value),
        Transitioning<PropertyValue<std::array<float, 2>>>(textTranslate.value),
        Transitioning<PropertyValue<TranslateAnchorType>>(textTranslateAnchor.value),
    };
}

bool SymbolPaintProperties::Transitionable::hasDataDrivenPropertyDifference(const Transitionable& other) const {
    return false
        || iconOpacity.value.hasDataDrivenPropertyDifference(other.iconOpacity.value)
        || iconColor.value.hasDataDrivenPropertyDifference(other.iconColor.value)
        || iconHaloColor.value.hasDataDrivenPropertyDifference(other.iconHaloColor.value)
        || iconHaloWidth.value.hasDataDrivenPropertyDifference(other.iconHaloWidth.value)
        || iconHaloBlur.value.hasDataDrivenPropertyDifference(other.iconHaloBlur.value)
        || iconTranslate.value.hasDataDrivenPropertyDifference(other.iconTranslate.value)
        || iconTranslateAnchor.value.hasDataDrivenPropertyDifference(other.iconTranslateAnchor.value)
        || textOpacity.value.hasDataDrivenPropertyDifference(other.textOpacity.value)
        || textColor.value.hasDataDrivenPropertyDifference(other.textColor.value)
        || textHaloColor.value.hasDataDrivenPropertyDifference(other.textHaloColor.value)
        || textHaloWidth.value.hasDataDrivenPropertyDifference(other.textHaloWidth.value)
        || textHaloBlur.value.hasDataDrivenPropertyDifference(other.textHaloBlur.value)
        || textTranslate.value.hasDataDrivenPropertyDifference(other.textTranslate.value)
        || textTranslateAnchor.value.hasDataDrivenPropertyDifference(other.textTranslateAnchor.value)
        ;
}

bool SymbolPaintProperties::Unevaluated::hasTransition() const {
    return false
        || iconOpacity.hasTransition()
        || iconColor.hasTransition()
        || iconHaloColor.hasTransition()
        || iconHaloWidth.hasTransition()
        || iconHaloBlur.hasTransition()
        || iconTranslate.hasTransition()
        || iconTranslateAnchor.hasTransition()
        || textOpacity.hasTransition()
        || textColor.hasTransition()
        || textHaloColor.hasTransition()
        || textHaloWidth.hasTransition()
        || textHaloBlur.hasTransition()
        || textTranslate.hasTransition()
        || textTranslateAnchor.hasTransition()
        ;
}

SymbolPaintProperties::PossiblyEvaluated SymbolPaintProperties::Unevaluated::evaluate(const PropertyEvaluationParameters& parameters) const {
    return PossiblyEvaluated {
        iconOpacity.evaluate(typename IconOpacity::EvaluatorType(parameters, IconOpacity::defaultValue()), parameters.now),
        iconColor.evaluate(typename IconColor::EvaluatorType(parameters, IconColor::defaultValue()), parameters.now),
        iconHaloColor.evaluate(typename IconHaloColor::EvaluatorType(parameters, IconHaloColor::defaultValue()), parameters.now),
        iconHaloWidth.evaluate(typename IconHaloWidth::EvaluatorType(parameters, IconHaloWidth::defaultValue()), parameters.now),
        iconHaloBlur.evaluate(typename IconHaloBlur::EvaluatorType(parameters, IconHaloBlur::defaultValue()), parameters.now),
        iconTranslate.evaluate(typename IconTranslate::EvaluatorType(parameters, IconTranslate::defaultValue()), parameters.now),
        iconTranslateAnchor.evaluate(typename IconTranslateAnchor::EvaluatorType(parameters, IconTranslateAnchor::defaultValue()), parameters.now),
        textOpacity.evaluate(typename TextOpacity::EvaluatorType(parameters, TextOpacity::defaultValue()), parameters.now),
        textColor.evaluate(typename TextColor::EvaluatorType(parameters, TextColor::defaultValue()), parameters.now),
        textHaloColor.evaluate(typename TextHaloColor::EvaluatorType(parameters, TextHaloColor::defaultValue()), parameters.now),
        textHaloWidth.evaluate(typename TextHaloWidth::EvaluatorType(parameters, TextHaloWidth::defaultValue()), parameters.now),
        textHaloBlur.evaluate(typename TextHaloBlur::EvaluatorType(parameters, TextHaloBlur::defaultValue()), parameters.now),
        textTranslate.evaluate(typename TextTranslate::EvaluatorType(parameters, TextTranslate::defaultValue()), parameters.now),
        textTranslateAnchor.evaluate(typename TextTranslateAnchor::EvaluatorType(parameters, TextTranslateAnchor::defaultValue()), parameters.now),
    };
}




IconPaintProperties::Binders IconPaintProperties::PossiblyEvaluated::createBinders(float z) const {
    return Binders {
        PaintPropertyBinder<float, typename attributes::a_opacity::Type>::create(iconOpacity, z, IconOpacity::defaultValue()),
        PaintPropertyBinder<Color, typename attributes::a_fill_color::Type>::create(iconColor, z, IconColor::defaultValue()),
        PaintPropertyBinder<Color, typename attributes::a_halo_color::Type>::create(iconHaloColor, z, IconHaloColor::defaultValue()),
        PaintPropertyBinder<float, typename attributes::a_halo_width::Type>::create(iconHaloWidth, z, IconHaloWidth::defaultValue()),
        PaintPropertyBinder<float, typename attributes::a_halo_blur::Type>::create(iconHaloBlur, z, IconHaloBlur::defaultValue()),
    };
}

std::bitset<8> IconPaintProperties::PossiblyEvaluated::constants() const {
    std::bitset<8> result;
    result.set(0, iconOpacity.isConstant());
    result.set(1, iconColor.isConstant());
    result.set(2, iconHaloColor.isConstant());
    result.set(3, iconHaloWidth.isConstant());
    result.set(4, iconHaloBlur.isConstant());
    return result;
}

std::vector<std::string> IconPaintProperties::PossiblyEvaluated::defines() const {
    std::vector<std::string> result;
    result.push_back(iconOpacity.isConstant()
        ? std::string("#define HAS_UNIFORM_") + IconOpacity::Uniform::name()
        : std::string());
    result.push_back(iconColor.isConstant()
        ? std::string("#define HAS_UNIFORM_") + IconColor::Uniform::name()
        : std::string());
    result.push_back(iconHaloColor.isConstant()
        ? std::string("#define HAS_UNIFORM_") + IconHaloColor::Uniform::name()
        : std::string());
    result.push_back(iconHaloWidth.isConstant()
        ? std::string("#define HAS_UNIFORM_") + IconHaloWidth::Uniform::name()
        : std::string());
    result.push_back(iconHaloBlur.isConstant()
        ? std::string("#define HAS_UNIFORM_") + IconHaloBlur::Uniform::name()
        : std::string());
    return result;
}

void IconPaintProperties::Binders::populateVertexVectors(const GeometryTileFeature& feature, std::size_t length) {
    iconOpacity->populateVertexVector(feature, length);
    iconColor->populateVertexVector(feature, length);
    iconHaloColor->populateVertexVector(feature, length);
    iconHaloWidth->populateVertexVector(feature, length);
    iconHaloBlur->populateVertexVector(feature, length);
}

void IconPaintProperties::Binders::upload(gl::Context& context) {
    iconOpacity->upload(context);
    iconColor->upload(context);
    iconHaloColor->upload(context);
    iconHaloWidth->upload(context);
    iconHaloBlur->upload(context);
}

IconPaintProperties::Binders::AttributeBindings IconPaintProperties::Binders::attributeBindings(const PossiblyEvaluated& currentProperties) const {
    return AttributeBindings {
        iconOpacity->attributeBinding(currentProperties.iconOpacity),
        iconColor->attributeBinding(currentProperties.iconColor),
        iconHaloColor->attributeBinding(currentProperties.iconHaloColor),
        iconHaloWidth->attributeBinding(currentProperties.iconHaloWidth),
        iconHaloBlur->attributeBinding(currentProperties.iconHaloBlur),
    };
}

IconPaintProperties::Binders::UniformValues IconPaintProperties::Binders::uniformValues(float currentZoom, const PossiblyEvaluated& currentProperties) const {
    return UniformValues {
        typename InterpolationUniform<attributes::a_opacity>::Value {
            iconOpacity->interpolationFactor(currentZoom)
        },
        typename InterpolationUniform<attributes::a_fill_color>::Value {
            iconColor->interpolationFactor(currentZoom)
        },
        typename InterpolationUniform<attributes::a_halo_color>::Value {
            iconHaloColor->interpolationFactor(currentZoom)
        },
        typename InterpolationUniform<attributes::a_halo_width>::Value {
            iconHaloWidth->interpolationFactor(currentZoom)
        },
        typename InterpolationUniform<attributes::a_halo_blur>::Value {
            iconHaloBlur->interpolationFactor(currentZoom)
        },
        typename uniforms::u_opacity::Value {
            iconOpacity->uniformValue(currentProperties.iconOpacity)
        },
        typename uniforms::u_fill_color::Value {
            iconColor->uniformValue(currentProperties.iconColor)
        },
        typename uniforms::u_halo_color::Value {
            iconHaloColor->uniformValue(currentProperties.iconHaloColor)
        },
        typename uniforms::u_halo_width::Value {
            iconHaloWidth->uniformValue(currentProperties.iconHaloWidth)
        },
        typename uniforms::u_halo_blur::Value {
            iconHaloBlur->uniformValue(currentProperties.iconHaloBlur)
        },
    };
}



TextPaintProperties::Binders TextPaintProperties::PossiblyEvaluated::createBinders(float z) const {
    return Binders {
        PaintPropertyBinder<float, typename attributes::a_opacity::Type>::create(textOpacity, z, TextOpacity::defaultValue()),
        PaintPropertyBinder<Color, typename attributes::a_fill_color::Type>::create(textColor, z, TextColor::defaultValue()),
        PaintPropertyBinder<Color, typename attributes::a_halo_color::Type>::create(textHaloColor, z, TextHaloColor::defaultValue()),
        PaintPropertyBinder<float, typename attributes::a_halo_width::Type>::create(textHaloWidth, z, TextHaloWidth::defaultValue()),
        PaintPropertyBinder<float, typename attributes::a_halo_blur::Type>::create(textHaloBlur, z, TextHaloBlur::defaultValue()),
    };
}

std::bitset<8> TextPaintProperties::PossiblyEvaluated::constants() const {
    std::bitset<8> result;
    result.set(0, textOpacity.isConstant());
    result.set(1, textColor.isConstant());
    result.set(2, textHaloColor.isConstant());
    result.set(3, textHaloWidth.isConstant());
    result.set(4, textHaloBlur.isConstant());
    return result;
}

std::vector<std::string> TextPaintProperties::PossiblyEvaluated::defines() const {
    std::vector<std::string> result;
    result.push_back(textOpacity.isConstant()
        ? std::string("#define HAS_UNIFORM_") + TextOpacity::Uniform::name()
        : std::string());
    result.push_back(textColor.isConstant()
        ? std::string("#define HAS_UNIFORM_") + TextColor::Uniform::name()
        : std::string());
    result.push_back(textHaloColor.isConstant()
        ? std::string("#define HAS_UNIFORM_") + TextHaloColor::Uniform::name()
        : std::string());
    result.push_back(textHaloWidth.isConstant()
        ? std::string("#define HAS_UNIFORM_") + TextHaloWidth::Uniform::name()
        : std::string());
    result.push_back(textHaloBlur.isConstant()
        ? std::string("#define HAS_UNIFORM_") + TextHaloBlur::Uniform::name()
        : std::string());
    return result;
}

void TextPaintProperties::Binders::populateVertexVectors(const GeometryTileFeature& feature, std::size_t length) {
    textOpacity->populateVertexVector(feature, length);
    textColor->populateVertexVector(feature, length);
    textHaloColor->populateVertexVector(feature, length);
    textHaloWidth->populateVertexVector(feature, length);
    textHaloBlur->populateVertexVector(feature, length);
}

void TextPaintProperties::Binders::upload(gl::Context& context) {
    textOpacity->upload(context);
    textColor->upload(context);
    textHaloColor->upload(context);
    textHaloWidth->upload(context);
    textHaloBlur->upload(context);
}

TextPaintProperties::Binders::AttributeBindings TextPaintProperties::Binders::attributeBindings(const PossiblyEvaluated& currentProperties) const {
    return AttributeBindings {
        textOpacity->attributeBinding(currentProperties.textOpacity),
        textColor->attributeBinding(currentProperties.textColor),
        textHaloColor->attributeBinding(currentProperties.textHaloColor),
        textHaloWidth->attributeBinding(currentProperties.textHaloWidth),
        textHaloBlur->attributeBinding(currentProperties.textHaloBlur),
    };
}

TextPaintProperties::Binders::UniformValues TextPaintProperties::Binders::uniformValues(float currentZoom, const PossiblyEvaluated& currentProperties) const {
    return UniformValues {
        typename InterpolationUniform<attributes::a_opacity>::Value {
            textOpacity->interpolationFactor(currentZoom)
        },
        typename InterpolationUniform<attributes::a_fill_color>::Value {
            textColor->interpolationFactor(currentZoom)
        },
        typename InterpolationUniform<attributes::a_halo_color>::Value {
            textHaloColor->interpolationFactor(currentZoom)
        },
        typename InterpolationUniform<attributes::a_halo_width>::Value {
            textHaloWidth->interpolationFactor(currentZoom)
        },
        typename InterpolationUniform<attributes::a_halo_blur>::Value {
            textHaloBlur->interpolationFactor(currentZoom)
        },
        typename uniforms::u_opacity::Value {
            textOpacity->uniformValue(currentProperties.textOpacity)
        },
        typename uniforms::u_fill_color::Value {
            textColor->uniformValue(currentProperties.textColor)
        },
        typename uniforms::u_halo_color::Value {
            textHaloColor->uniformValue(currentProperties.textHaloColor)
        },
        typename uniforms::u_halo_width::Value {
            textHaloWidth->uniformValue(currentProperties.textHaloWidth)
        },
        typename uniforms::u_halo_blur::Value {
            textHaloBlur->uniformValue(currentProperties.textHaloBlur)
        },
    };
}


} // namespace style
} // namespace mbgl
