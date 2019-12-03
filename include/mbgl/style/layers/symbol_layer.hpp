// clang-format off

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
    optional<conversion::Error> setProperty(const std::string& name, const conversion::Convertible& value) final;

    StyleProperty getProperty(const std::string& name) const final;

    // Layout properties

    static PropertyValue<bool> getDefaultIconAllowOverlap();
    const PropertyValue<bool>& getIconAllowOverlap() const;
    void setIconAllowOverlap(const PropertyValue<bool>&);

    static PropertyValue<SymbolAnchorType> getDefaultIconAnchor();
    const PropertyValue<SymbolAnchorType>& getIconAnchor() const;
    void setIconAnchor(const PropertyValue<SymbolAnchorType>&);

    static PropertyValue<bool> getDefaultIconIgnorePlacement();
    const PropertyValue<bool>& getIconIgnorePlacement() const;
    void setIconIgnorePlacement(const PropertyValue<bool>&);

    static PropertyValue<expression::Image> getDefaultIconImage();
    const PropertyValue<expression::Image>& getIconImage() const;
    void setIconImage(const PropertyValue<expression::Image>&);

    static PropertyValue<bool> getDefaultIconKeepUpright();
    const PropertyValue<bool>& getIconKeepUpright() const;
    void setIconKeepUpright(const PropertyValue<bool>&);

    static PropertyValue<std::array<float, 2>> getDefaultIconOffset();
    const PropertyValue<std::array<float, 2>>& getIconOffset() const;
    void setIconOffset(const PropertyValue<std::array<float, 2>>&);

    static PropertyValue<bool> getDefaultIconOptional();
    const PropertyValue<bool>& getIconOptional() const;
    void setIconOptional(const PropertyValue<bool>&);

    static PropertyValue<float> getDefaultIconPadding();
    const PropertyValue<float>& getIconPadding() const;
    void setIconPadding(const PropertyValue<float>&);

    static PropertyValue<AlignmentType> getDefaultIconPitchAlignment();
    const PropertyValue<AlignmentType>& getIconPitchAlignment() const;
    void setIconPitchAlignment(const PropertyValue<AlignmentType>&);

    static PropertyValue<float> getDefaultIconRotate();
    const PropertyValue<float>& getIconRotate() const;
    void setIconRotate(const PropertyValue<float>&);

    static PropertyValue<AlignmentType> getDefaultIconRotationAlignment();
    const PropertyValue<AlignmentType>& getIconRotationAlignment() const;
    void setIconRotationAlignment(const PropertyValue<AlignmentType>&);

    static PropertyValue<float> getDefaultIconSize();
    const PropertyValue<float>& getIconSize() const;
    void setIconSize(const PropertyValue<float>&);

    static PropertyValue<IconTextFitType> getDefaultIconTextFit();
    const PropertyValue<IconTextFitType>& getIconTextFit() const;
    void setIconTextFit(const PropertyValue<IconTextFitType>&);

    static PropertyValue<std::array<float, 4>> getDefaultIconTextFitPadding();
    const PropertyValue<std::array<float, 4>>& getIconTextFitPadding() const;
    void setIconTextFitPadding(const PropertyValue<std::array<float, 4>>&);

    static PropertyValue<bool> getDefaultSymbolAvoidEdges();
    const PropertyValue<bool>& getSymbolAvoidEdges() const;
    void setSymbolAvoidEdges(const PropertyValue<bool>&);

    static PropertyValue<SymbolPlacementType> getDefaultSymbolPlacement();
    const PropertyValue<SymbolPlacementType>& getSymbolPlacement() const;
    void setSymbolPlacement(const PropertyValue<SymbolPlacementType>&);

    static PropertyValue<float> getDefaultSymbolSortKey();
    const PropertyValue<float>& getSymbolSortKey() const;
    void setSymbolSortKey(const PropertyValue<float>&);

    static PropertyValue<float> getDefaultSymbolSpacing();
    const PropertyValue<float>& getSymbolSpacing() const;
    void setSymbolSpacing(const PropertyValue<float>&);

    static PropertyValue<SymbolZOrderType> getDefaultSymbolZOrder();
    const PropertyValue<SymbolZOrderType>& getSymbolZOrder() const;
    void setSymbolZOrder(const PropertyValue<SymbolZOrderType>&);

    static PropertyValue<bool> getDefaultTextAllowOverlap();
    const PropertyValue<bool>& getTextAllowOverlap() const;
    void setTextAllowOverlap(const PropertyValue<bool>&);

    static PropertyValue<SymbolAnchorType> getDefaultTextAnchor();
    const PropertyValue<SymbolAnchorType>& getTextAnchor() const;
    void setTextAnchor(const PropertyValue<SymbolAnchorType>&);

    static PropertyValue<expression::Formatted> getDefaultTextField();
    const PropertyValue<expression::Formatted>& getTextField() const;
    void setTextField(const PropertyValue<expression::Formatted>&);

    static PropertyValue<std::vector<std::string>> getDefaultTextFont();
    const PropertyValue<std::vector<std::string>>& getTextFont() const;
    void setTextFont(const PropertyValue<std::vector<std::string>>&);

    static PropertyValue<bool> getDefaultTextIgnorePlacement();
    const PropertyValue<bool>& getTextIgnorePlacement() const;
    void setTextIgnorePlacement(const PropertyValue<bool>&);

    static PropertyValue<TextJustifyType> getDefaultTextJustify();
    const PropertyValue<TextJustifyType>& getTextJustify() const;
    void setTextJustify(const PropertyValue<TextJustifyType>&);

    static PropertyValue<bool> getDefaultTextKeepUpright();
    const PropertyValue<bool>& getTextKeepUpright() const;
    void setTextKeepUpright(const PropertyValue<bool>&);

    static PropertyValue<float> getDefaultTextLetterSpacing();
    const PropertyValue<float>& getTextLetterSpacing() const;
    void setTextLetterSpacing(const PropertyValue<float>&);

    static PropertyValue<float> getDefaultTextLineHeight();
    const PropertyValue<float>& getTextLineHeight() const;
    void setTextLineHeight(const PropertyValue<float>&);

    static PropertyValue<float> getDefaultTextMaxAngle();
    const PropertyValue<float>& getTextMaxAngle() const;
    void setTextMaxAngle(const PropertyValue<float>&);

    static PropertyValue<float> getDefaultTextMaxWidth();
    const PropertyValue<float>& getTextMaxWidth() const;
    void setTextMaxWidth(const PropertyValue<float>&);

    static PropertyValue<std::array<float, 2>> getDefaultTextOffset();
    const PropertyValue<std::array<float, 2>>& getTextOffset() const;
    void setTextOffset(const PropertyValue<std::array<float, 2>>&);

    static PropertyValue<bool> getDefaultTextOptional();
    const PropertyValue<bool>& getTextOptional() const;
    void setTextOptional(const PropertyValue<bool>&);

    static PropertyValue<float> getDefaultTextPadding();
    const PropertyValue<float>& getTextPadding() const;
    void setTextPadding(const PropertyValue<float>&);

    static PropertyValue<AlignmentType> getDefaultTextPitchAlignment();
    const PropertyValue<AlignmentType>& getTextPitchAlignment() const;
    void setTextPitchAlignment(const PropertyValue<AlignmentType>&);

    static PropertyValue<float> getDefaultTextRadialOffset();
    const PropertyValue<float>& getTextRadialOffset() const;
    void setTextRadialOffset(const PropertyValue<float>&);

    static PropertyValue<float> getDefaultTextRotate();
    const PropertyValue<float>& getTextRotate() const;
    void setTextRotate(const PropertyValue<float>&);

    static PropertyValue<AlignmentType> getDefaultTextRotationAlignment();
    const PropertyValue<AlignmentType>& getTextRotationAlignment() const;
    void setTextRotationAlignment(const PropertyValue<AlignmentType>&);

    static PropertyValue<float> getDefaultTextSize();
    const PropertyValue<float>& getTextSize() const;
    void setTextSize(const PropertyValue<float>&);

    static PropertyValue<TextTransformType> getDefaultTextTransform();
    const PropertyValue<TextTransformType>& getTextTransform() const;
    void setTextTransform(const PropertyValue<TextTransformType>&);

    static PropertyValue<std::vector<TextVariableAnchorType>> getDefaultTextVariableAnchor();
    const PropertyValue<std::vector<TextVariableAnchorType>>& getTextVariableAnchor() const;
    void setTextVariableAnchor(const PropertyValue<std::vector<TextVariableAnchorType>>&);

    static PropertyValue<std::vector<TextWritingModeType>> getDefaultTextWritingMode();
    const PropertyValue<std::vector<TextWritingModeType>>& getTextWritingMode() const;
    void setTextWritingMode(const PropertyValue<std::vector<TextWritingModeType>>&);

    // Paint properties

    static PropertyValue<Color> getDefaultIconColor();
    const PropertyValue<Color>& getIconColor() const;
    void setIconColor(const PropertyValue<Color>&);
    void setIconColorTransition(const TransitionOptions&);
    TransitionOptions getIconColorTransition() const;

    static PropertyValue<float> getDefaultIconHaloBlur();
    const PropertyValue<float>& getIconHaloBlur() const;
    void setIconHaloBlur(const PropertyValue<float>&);
    void setIconHaloBlurTransition(const TransitionOptions&);
    TransitionOptions getIconHaloBlurTransition() const;

    static PropertyValue<Color> getDefaultIconHaloColor();
    const PropertyValue<Color>& getIconHaloColor() const;
    void setIconHaloColor(const PropertyValue<Color>&);
    void setIconHaloColorTransition(const TransitionOptions&);
    TransitionOptions getIconHaloColorTransition() const;

    static PropertyValue<float> getDefaultIconHaloWidth();
    const PropertyValue<float>& getIconHaloWidth() const;
    void setIconHaloWidth(const PropertyValue<float>&);
    void setIconHaloWidthTransition(const TransitionOptions&);
    TransitionOptions getIconHaloWidthTransition() const;

    static PropertyValue<float> getDefaultIconOpacity();
    const PropertyValue<float>& getIconOpacity() const;
    void setIconOpacity(const PropertyValue<float>&);
    void setIconOpacityTransition(const TransitionOptions&);
    TransitionOptions getIconOpacityTransition() const;

    static PropertyValue<std::array<float, 2>> getDefaultIconTranslate();
    const PropertyValue<std::array<float, 2>>& getIconTranslate() const;
    void setIconTranslate(const PropertyValue<std::array<float, 2>>&);
    void setIconTranslateTransition(const TransitionOptions&);
    TransitionOptions getIconTranslateTransition() const;

    static PropertyValue<TranslateAnchorType> getDefaultIconTranslateAnchor();
    const PropertyValue<TranslateAnchorType>& getIconTranslateAnchor() const;
    void setIconTranslateAnchor(const PropertyValue<TranslateAnchorType>&);
    void setIconTranslateAnchorTransition(const TransitionOptions&);
    TransitionOptions getIconTranslateAnchorTransition() const;

    static PropertyValue<Color> getDefaultTextColor();
    const PropertyValue<Color>& getTextColor() const;
    void setTextColor(const PropertyValue<Color>&);
    void setTextColorTransition(const TransitionOptions&);
    TransitionOptions getTextColorTransition() const;

    static PropertyValue<float> getDefaultTextHaloBlur();
    const PropertyValue<float>& getTextHaloBlur() const;
    void setTextHaloBlur(const PropertyValue<float>&);
    void setTextHaloBlurTransition(const TransitionOptions&);
    TransitionOptions getTextHaloBlurTransition() const;

    static PropertyValue<Color> getDefaultTextHaloColor();
    const PropertyValue<Color>& getTextHaloColor() const;
    void setTextHaloColor(const PropertyValue<Color>&);
    void setTextHaloColorTransition(const TransitionOptions&);
    TransitionOptions getTextHaloColorTransition() const;

    static PropertyValue<float> getDefaultTextHaloWidth();
    const PropertyValue<float>& getTextHaloWidth() const;
    void setTextHaloWidth(const PropertyValue<float>&);
    void setTextHaloWidthTransition(const TransitionOptions&);
    TransitionOptions getTextHaloWidthTransition() const;

    static PropertyValue<float> getDefaultTextOpacity();
    const PropertyValue<float>& getTextOpacity() const;
    void setTextOpacity(const PropertyValue<float>&);
    void setTextOpacityTransition(const TransitionOptions&);
    TransitionOptions getTextOpacityTransition() const;

    static PropertyValue<std::array<float, 2>> getDefaultTextTranslate();
    const PropertyValue<std::array<float, 2>>& getTextTranslate() const;
    void setTextTranslate(const PropertyValue<std::array<float, 2>>&);
    void setTextTranslateTransition(const TransitionOptions&);
    TransitionOptions getTextTranslateTransition() const;

    static PropertyValue<TranslateAnchorType> getDefaultTextTranslateAnchor();
    const PropertyValue<TranslateAnchorType>& getTextTranslateAnchor() const;
    void setTextTranslateAnchor(const PropertyValue<TranslateAnchorType>&);
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

// clang-format on
