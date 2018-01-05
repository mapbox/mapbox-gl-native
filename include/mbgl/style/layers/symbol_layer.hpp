// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/data_driven_property_value.hpp>

#include <mbgl/util/color.hpp>

#include <vector>

namespace mbgl {
namespace style {

class TransitionOptions;

class SymbolLayer : public Layer {
public:
    SymbolLayer(const std::string& layerID, const std::string& sourceID);
    ~SymbolLayer() final;

    // Source
    const std::string& getSourceID() const;
    const std::string& getSourceLayer() const;
    void setSourceLayer(const std::string& sourceLayer);

    void setFilter(const Filter&);
    const Filter& getFilter() const;

    // Visibility
    void setVisibility(VisibilityType) final;

    // Zoom range
    void setMinZoom(float) final;
    void setMaxZoom(float) final;

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

    static DataDrivenPropertyValue<float> getDefaultIconSize();
    DataDrivenPropertyValue<float> getIconSize() const;
    void setIconSize(DataDrivenPropertyValue<float>);

    static PropertyValue<IconTextFitType> getDefaultIconTextFit();
    PropertyValue<IconTextFitType> getIconTextFit() const;
    void setIconTextFit(PropertyValue<IconTextFitType>);

    static PropertyValue<std::array<float, 4>> getDefaultIconTextFitPadding();
    PropertyValue<std::array<float, 4>> getIconTextFitPadding() const;
    void setIconTextFitPadding(PropertyValue<std::array<float, 4>>);

    static DataDrivenPropertyValue<std::string> getDefaultIconImage();
    DataDrivenPropertyValue<std::string> getIconImage() const;
    void setIconImage(DataDrivenPropertyValue<std::string>);

    static DataDrivenPropertyValue<float> getDefaultIconRotate();
    DataDrivenPropertyValue<float> getIconRotate() const;
    void setIconRotate(DataDrivenPropertyValue<float>);

    static PropertyValue<float> getDefaultIconPadding();
    PropertyValue<float> getIconPadding() const;
    void setIconPadding(PropertyValue<float>);

    static PropertyValue<bool> getDefaultIconKeepUpright();
    PropertyValue<bool> getIconKeepUpright() const;
    void setIconKeepUpright(PropertyValue<bool>);

    static DataDrivenPropertyValue<std::array<float, 2>> getDefaultIconOffset();
    DataDrivenPropertyValue<std::array<float, 2>> getIconOffset() const;
    void setIconOffset(DataDrivenPropertyValue<std::array<float, 2>>);

    static DataDrivenPropertyValue<SymbolAnchorType> getDefaultIconAnchor();
    DataDrivenPropertyValue<SymbolAnchorType> getIconAnchor() const;
    void setIconAnchor(DataDrivenPropertyValue<SymbolAnchorType>);

    static PropertyValue<AlignmentType> getDefaultIconPitchAlignment();
    PropertyValue<AlignmentType> getIconPitchAlignment() const;
    void setIconPitchAlignment(PropertyValue<AlignmentType>);

    static PropertyValue<AlignmentType> getDefaultTextPitchAlignment();
    PropertyValue<AlignmentType> getTextPitchAlignment() const;
    void setTextPitchAlignment(PropertyValue<AlignmentType>);

    static PropertyValue<AlignmentType> getDefaultTextRotationAlignment();
    PropertyValue<AlignmentType> getTextRotationAlignment() const;
    void setTextRotationAlignment(PropertyValue<AlignmentType>);

    static DataDrivenPropertyValue<std::string> getDefaultTextField();
    DataDrivenPropertyValue<std::string> getTextField() const;
    void setTextField(DataDrivenPropertyValue<std::string>);

    static DataDrivenPropertyValue<std::vector<std::string>> getDefaultTextFont();
    DataDrivenPropertyValue<std::vector<std::string>> getTextFont() const;
    void setTextFont(DataDrivenPropertyValue<std::vector<std::string>>);

    static DataDrivenPropertyValue<float> getDefaultTextSize();
    DataDrivenPropertyValue<float> getTextSize() const;
    void setTextSize(DataDrivenPropertyValue<float>);

    static DataDrivenPropertyValue<float> getDefaultTextMaxWidth();
    DataDrivenPropertyValue<float> getTextMaxWidth() const;
    void setTextMaxWidth(DataDrivenPropertyValue<float>);

    static PropertyValue<float> getDefaultTextLineHeight();
    PropertyValue<float> getTextLineHeight() const;
    void setTextLineHeight(PropertyValue<float>);

    static DataDrivenPropertyValue<float> getDefaultTextLetterSpacing();
    DataDrivenPropertyValue<float> getTextLetterSpacing() const;
    void setTextLetterSpacing(DataDrivenPropertyValue<float>);

    static DataDrivenPropertyValue<TextJustifyType> getDefaultTextJustify();
    DataDrivenPropertyValue<TextJustifyType> getTextJustify() const;
    void setTextJustify(DataDrivenPropertyValue<TextJustifyType>);

    static DataDrivenPropertyValue<SymbolAnchorType> getDefaultTextAnchor();
    DataDrivenPropertyValue<SymbolAnchorType> getTextAnchor() const;
    void setTextAnchor(DataDrivenPropertyValue<SymbolAnchorType>);

    static PropertyValue<float> getDefaultTextMaxAngle();
    PropertyValue<float> getTextMaxAngle() const;
    void setTextMaxAngle(PropertyValue<float>);

    static DataDrivenPropertyValue<float> getDefaultTextRotate();
    DataDrivenPropertyValue<float> getTextRotate() const;
    void setTextRotate(DataDrivenPropertyValue<float>);

    static PropertyValue<float> getDefaultTextPadding();
    PropertyValue<float> getTextPadding() const;
    void setTextPadding(PropertyValue<float>);

    static PropertyValue<bool> getDefaultTextKeepUpright();
    PropertyValue<bool> getTextKeepUpright() const;
    void setTextKeepUpright(PropertyValue<bool>);

    static DataDrivenPropertyValue<TextTransformType> getDefaultTextTransform();
    DataDrivenPropertyValue<TextTransformType> getTextTransform() const;
    void setTextTransform(DataDrivenPropertyValue<TextTransformType>);

    static DataDrivenPropertyValue<std::array<float, 2>> getDefaultTextOffset();
    DataDrivenPropertyValue<std::array<float, 2>> getTextOffset() const;
    void setTextOffset(DataDrivenPropertyValue<std::array<float, 2>>);

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

    static DataDrivenPropertyValue<float> getDefaultIconOpacity();
    DataDrivenPropertyValue<float> getIconOpacity() const;
    void setIconOpacity(DataDrivenPropertyValue<float>);
    void setIconOpacityTransition(const TransitionOptions&);
    TransitionOptions getIconOpacityTransition() const;

    static DataDrivenPropertyValue<Color> getDefaultIconColor();
    DataDrivenPropertyValue<Color> getIconColor() const;
    void setIconColor(DataDrivenPropertyValue<Color>);
    void setIconColorTransition(const TransitionOptions&);
    TransitionOptions getIconColorTransition() const;

    static DataDrivenPropertyValue<Color> getDefaultIconHaloColor();
    DataDrivenPropertyValue<Color> getIconHaloColor() const;
    void setIconHaloColor(DataDrivenPropertyValue<Color>);
    void setIconHaloColorTransition(const TransitionOptions&);
    TransitionOptions getIconHaloColorTransition() const;

    static DataDrivenPropertyValue<float> getDefaultIconHaloWidth();
    DataDrivenPropertyValue<float> getIconHaloWidth() const;
    void setIconHaloWidth(DataDrivenPropertyValue<float>);
    void setIconHaloWidthTransition(const TransitionOptions&);
    TransitionOptions getIconHaloWidthTransition() const;

    static DataDrivenPropertyValue<float> getDefaultIconHaloBlur();
    DataDrivenPropertyValue<float> getIconHaloBlur() const;
    void setIconHaloBlur(DataDrivenPropertyValue<float>);
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

    static DataDrivenPropertyValue<float> getDefaultTextOpacity();
    DataDrivenPropertyValue<float> getTextOpacity() const;
    void setTextOpacity(DataDrivenPropertyValue<float>);
    void setTextOpacityTransition(const TransitionOptions&);
    TransitionOptions getTextOpacityTransition() const;

    static DataDrivenPropertyValue<Color> getDefaultTextColor();
    DataDrivenPropertyValue<Color> getTextColor() const;
    void setTextColor(DataDrivenPropertyValue<Color>);
    void setTextColorTransition(const TransitionOptions&);
    TransitionOptions getTextColorTransition() const;

    static DataDrivenPropertyValue<Color> getDefaultTextHaloColor();
    DataDrivenPropertyValue<Color> getTextHaloColor() const;
    void setTextHaloColor(DataDrivenPropertyValue<Color>);
    void setTextHaloColorTransition(const TransitionOptions&);
    TransitionOptions getTextHaloColorTransition() const;

    static DataDrivenPropertyValue<float> getDefaultTextHaloWidth();
    DataDrivenPropertyValue<float> getTextHaloWidth() const;
    void setTextHaloWidth(DataDrivenPropertyValue<float>);
    void setTextHaloWidthTransition(const TransitionOptions&);
    TransitionOptions getTextHaloWidthTransition() const;

    static DataDrivenPropertyValue<float> getDefaultTextHaloBlur();
    DataDrivenPropertyValue<float> getTextHaloBlur() const;
    void setTextHaloBlur(DataDrivenPropertyValue<float>);
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
};

template <>
inline bool Layer::is<SymbolLayer>() const {
    return getType() == LayerType::Symbol;
}

} // namespace style
} // namespace mbgl
