// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/expression/formatted.hpp>
#include <mbgl/util/color.hpp>

#include <vector>

namespace mbgl {
namespace style {

class TransitionOptions;

class SymbolLayer : public Layer {
public:
    SymbolLayer(const std::string& layerID, const std::string& sourceID);
    ~SymbolLayer() final;

    // Dynamic properties
    optional<conversion::Error> setLayoutProperty(const std::string& name, const conversion::Convertible& value) final;
    optional<conversion::Error> setPaintProperty(const std::string& name, const conversion::Convertible& value) final;

    // Layout properties

    static PropertyValue<SymbolPlacementType> getDefaultSymbolPlacement();
    PropertyValue<SymbolPlacementType> getSymbolPlacement() const;
    void setSymbolPlacement(PropertyValue<SymbolPlacementType>);

    static PropertyValue<float> getDefaultSymbolSpacing();
    PropertyValue<float> getSymbolSpacing() const;
    void setSymbolSpacing(PropertyValue<float>);

    static PropertyValue<bool> getDefaultSymbolAvoidEdges();
    PropertyValue<bool> getSymbolAvoidEdges() const;
    void setSymbolAvoidEdges(PropertyValue<bool>);

    static PropertyValue<SymbolZOrderType> getDefaultSymbolZOrder();
    PropertyValue<SymbolZOrderType> getSymbolZOrder() const;
    void setSymbolZOrder(PropertyValue<SymbolZOrderType>);

    static PropertyValue<bool> getDefaultIconAllowOverlap();
    PropertyValue<bool> getIconAllowOverlap() const;
    void setIconAllowOverlap(PropertyValue<bool>);

    static PropertyValue<bool> getDefaultIconIgnorePlacement();
    PropertyValue<bool> getIconIgnorePlacement() const;
    void setIconIgnorePlacement(PropertyValue<bool>);

    static PropertyValue<bool> getDefaultIconOptional();
    PropertyValue<bool> getIconOptional() const;
    void setIconOptional(PropertyValue<bool>);

    static PropertyValue<AlignmentType> getDefaultIconRotationAlignment();
    PropertyValue<AlignmentType> getIconRotationAlignment() const;
    void setIconRotationAlignment(PropertyValue<AlignmentType>);

    static PropertyValue<float> getDefaultIconSize();
    PropertyValue<float> getIconSize() const;
    void setIconSize(PropertyValue<float>);

    static PropertyValue<IconTextFitType> getDefaultIconTextFit();
    PropertyValue<IconTextFitType> getIconTextFit() const;
    void setIconTextFit(PropertyValue<IconTextFitType>);

    static PropertyValue<std::array<float, 4>> getDefaultIconTextFitPadding();
    PropertyValue<std::array<float, 4>> getIconTextFitPadding() const;
    void setIconTextFitPadding(PropertyValue<std::array<float, 4>>);

    static PropertyValue<std::string> getDefaultIconImage();
    PropertyValue<std::string> getIconImage() const;
    void setIconImage(PropertyValue<std::string>);

    static PropertyValue<float> getDefaultIconRotate();
    PropertyValue<float> getIconRotate() const;
    void setIconRotate(PropertyValue<float>);

    static PropertyValue<float> getDefaultIconPadding();
    PropertyValue<float> getIconPadding() const;
    void setIconPadding(PropertyValue<float>);

    static PropertyValue<bool> getDefaultIconKeepUpright();
    PropertyValue<bool> getIconKeepUpright() const;
    void setIconKeepUpright(PropertyValue<bool>);

    static PropertyValue<std::array<float, 2>> getDefaultIconOffset();
    PropertyValue<std::array<float, 2>> getIconOffset() const;
    void setIconOffset(PropertyValue<std::array<float, 2>>);

    static PropertyValue<SymbolAnchorType> getDefaultIconAnchor();
    PropertyValue<SymbolAnchorType> getIconAnchor() const;
    void setIconAnchor(PropertyValue<SymbolAnchorType>);

    static PropertyValue<AlignmentType> getDefaultIconPitchAlignment();
    PropertyValue<AlignmentType> getIconPitchAlignment() const;
    void setIconPitchAlignment(PropertyValue<AlignmentType>);

    static PropertyValue<AlignmentType> getDefaultTextPitchAlignment();
    PropertyValue<AlignmentType> getTextPitchAlignment() const;
    void setTextPitchAlignment(PropertyValue<AlignmentType>);

    static PropertyValue<AlignmentType> getDefaultTextRotationAlignment();
    PropertyValue<AlignmentType> getTextRotationAlignment() const;
    void setTextRotationAlignment(PropertyValue<AlignmentType>);

    static PropertyValue<expression::Formatted> getDefaultTextField();
    PropertyValue<expression::Formatted> getTextField() const;
    void setTextField(PropertyValue<expression::Formatted>);

    static PropertyValue<std::vector<std::string>> getDefaultTextFont();
    PropertyValue<std::vector<std::string>> getTextFont() const;
    void setTextFont(PropertyValue<std::vector<std::string>>);

    static PropertyValue<float> getDefaultTextSize();
    PropertyValue<float> getTextSize() const;
    void setTextSize(PropertyValue<float>);

    static PropertyValue<float> getDefaultTextMaxWidth();
    PropertyValue<float> getTextMaxWidth() const;
    void setTextMaxWidth(PropertyValue<float>);

    static PropertyValue<float> getDefaultTextLineHeight();
    PropertyValue<float> getTextLineHeight() const;
    void setTextLineHeight(PropertyValue<float>);

    static PropertyValue<float> getDefaultTextLetterSpacing();
    PropertyValue<float> getTextLetterSpacing() const;
    void setTextLetterSpacing(PropertyValue<float>);

    static PropertyValue<TextJustifyType> getDefaultTextJustify();
    PropertyValue<TextJustifyType> getTextJustify() const;
    void setTextJustify(PropertyValue<TextJustifyType>);

    static PropertyValue<std::vector<TextVariableAnchorType>> getDefaultTextVariableAnchor();
    PropertyValue<std::vector<TextVariableAnchorType>> getTextVariableAnchor() const;
    void setTextVariableAnchor(PropertyValue<std::vector<TextVariableAnchorType>>);

    static PropertyValue<SymbolAnchorType> getDefaultTextAnchor();
    PropertyValue<SymbolAnchorType> getTextAnchor() const;
    void setTextAnchor(PropertyValue<SymbolAnchorType>);

    static PropertyValue<float> getDefaultTextMaxAngle();
    PropertyValue<float> getTextMaxAngle() const;
    void setTextMaxAngle(PropertyValue<float>);

    static PropertyValue<float> getDefaultTextRotate();
    PropertyValue<float> getTextRotate() const;
    void setTextRotate(PropertyValue<float>);

    static PropertyValue<float> getDefaultTextPadding();
    PropertyValue<float> getTextPadding() const;
    void setTextPadding(PropertyValue<float>);

    static PropertyValue<bool> getDefaultTextKeepUpright();
    PropertyValue<bool> getTextKeepUpright() const;
    void setTextKeepUpright(PropertyValue<bool>);

    static PropertyValue<TextTransformType> getDefaultTextTransform();
    PropertyValue<TextTransformType> getTextTransform() const;
    void setTextTransform(PropertyValue<TextTransformType>);

    static PropertyValue<std::array<float, 2>> getDefaultTextOffset();
    PropertyValue<std::array<float, 2>> getTextOffset() const;
    void setTextOffset(PropertyValue<std::array<float, 2>>);

    static PropertyValue<bool> getDefaultTextAllowOverlap();
    PropertyValue<bool> getTextAllowOverlap() const;
    void setTextAllowOverlap(PropertyValue<bool>);

    static PropertyValue<bool> getDefaultTextIgnorePlacement();
    PropertyValue<bool> getTextIgnorePlacement() const;
    void setTextIgnorePlacement(PropertyValue<bool>);

    static PropertyValue<bool> getDefaultTextOptional();
    PropertyValue<bool> getTextOptional() const;
    void setTextOptional(PropertyValue<bool>);

    // Paint properties

    static PropertyValue<float> getDefaultIconOpacity();
    PropertyValue<float> getIconOpacity() const;
    void setIconOpacity(PropertyValue<float>);
    void setIconOpacityTransition(const TransitionOptions&);
    TransitionOptions getIconOpacityTransition() const;

    static PropertyValue<Color> getDefaultIconColor();
    PropertyValue<Color> getIconColor() const;
    void setIconColor(PropertyValue<Color>);
    void setIconColorTransition(const TransitionOptions&);
    TransitionOptions getIconColorTransition() const;

    static PropertyValue<Color> getDefaultIconHaloColor();
    PropertyValue<Color> getIconHaloColor() const;
    void setIconHaloColor(PropertyValue<Color>);
    void setIconHaloColorTransition(const TransitionOptions&);
    TransitionOptions getIconHaloColorTransition() const;

    static PropertyValue<float> getDefaultIconHaloWidth();
    PropertyValue<float> getIconHaloWidth() const;
    void setIconHaloWidth(PropertyValue<float>);
    void setIconHaloWidthTransition(const TransitionOptions&);
    TransitionOptions getIconHaloWidthTransition() const;

    static PropertyValue<float> getDefaultIconHaloBlur();
    PropertyValue<float> getIconHaloBlur() const;
    void setIconHaloBlur(PropertyValue<float>);
    void setIconHaloBlurTransition(const TransitionOptions&);
    TransitionOptions getIconHaloBlurTransition() const;

    static PropertyValue<std::array<float, 2>> getDefaultIconTranslate();
    PropertyValue<std::array<float, 2>> getIconTranslate() const;
    void setIconTranslate(PropertyValue<std::array<float, 2>>);
    void setIconTranslateTransition(const TransitionOptions&);
    TransitionOptions getIconTranslateTransition() const;

    static PropertyValue<TranslateAnchorType> getDefaultIconTranslateAnchor();
    PropertyValue<TranslateAnchorType> getIconTranslateAnchor() const;
    void setIconTranslateAnchor(PropertyValue<TranslateAnchorType>);
    void setIconTranslateAnchorTransition(const TransitionOptions&);
    TransitionOptions getIconTranslateAnchorTransition() const;

    static PropertyValue<float> getDefaultTextOpacity();
    PropertyValue<float> getTextOpacity() const;
    void setTextOpacity(PropertyValue<float>);
    void setTextOpacityTransition(const TransitionOptions&);
    TransitionOptions getTextOpacityTransition() const;

    static PropertyValue<Color> getDefaultTextColor();
    PropertyValue<Color> getTextColor() const;
    void setTextColor(PropertyValue<Color>);
    void setTextColorTransition(const TransitionOptions&);
    TransitionOptions getTextColorTransition() const;

    static PropertyValue<Color> getDefaultTextHaloColor();
    PropertyValue<Color> getTextHaloColor() const;
    void setTextHaloColor(PropertyValue<Color>);
    void setTextHaloColorTransition(const TransitionOptions&);
    TransitionOptions getTextHaloColorTransition() const;

    static PropertyValue<float> getDefaultTextHaloWidth();
    PropertyValue<float> getTextHaloWidth() const;
    void setTextHaloWidth(PropertyValue<float>);
    void setTextHaloWidthTransition(const TransitionOptions&);
    TransitionOptions getTextHaloWidthTransition() const;

    static PropertyValue<float> getDefaultTextHaloBlur();
    PropertyValue<float> getTextHaloBlur() const;
    void setTextHaloBlur(PropertyValue<float>);
    void setTextHaloBlurTransition(const TransitionOptions&);
    TransitionOptions getTextHaloBlurTransition() const;

    static PropertyValue<std::array<float, 2>> getDefaultTextTranslate();
    PropertyValue<std::array<float, 2>> getTextTranslate() const;
    void setTextTranslate(PropertyValue<std::array<float, 2>>);
    void setTextTranslateTransition(const TransitionOptions&);
    TransitionOptions getTextTranslateTransition() const;

    static PropertyValue<TranslateAnchorType> getDefaultTextTranslateAnchor();
    PropertyValue<TranslateAnchorType> getTextTranslateAnchor() const;
    void setTextTranslateAnchor(PropertyValue<TranslateAnchorType>);
    void setTextTranslateAnchorTransition(const TransitionOptions&);
    TransitionOptions getTextTranslateAnchorTransition() const;

    // Private implementation

    class Impl;
    const Impl& impl() const;

    Mutable<Impl> mutableImpl() const;
    SymbolLayer(Immutable<Impl>);
    std::unique_ptr<Layer> cloneRef(const std::string& id) const final;

protected:
    Mutable<Layer::Impl> mutableBaseImpl() const final;
};

} // namespace style
} // namespace mbgl
