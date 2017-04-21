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

    static PropertyValue<AlignmentType> getDefaultTextPitchAlignment();
    PropertyValue<AlignmentType> getTextPitchAlignment() const;
    void setTextPitchAlignment(PropertyValue<AlignmentType>);

    static PropertyValue<AlignmentType> getDefaultTextRotationAlignment();
    PropertyValue<AlignmentType> getTextRotationAlignment() const;
    void setTextRotationAlignment(PropertyValue<AlignmentType>);

    static DataDrivenPropertyValue<std::string> getDefaultTextField();
    DataDrivenPropertyValue<std::string> getTextField() const;
    void setTextField(DataDrivenPropertyValue<std::string>);

    static PropertyValue<std::vector<std::string>> getDefaultTextFont();
    PropertyValue<std::vector<std::string>> getTextFont() const;
    void setTextFont(PropertyValue<std::vector<std::string>>);

    static DataDrivenPropertyValue<float> getDefaultTextSize();
    DataDrivenPropertyValue<float> getTextSize() const;
    void setTextSize(DataDrivenPropertyValue<float>);

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

    static PropertyValue<TextAnchorType> getDefaultTextAnchor();
    PropertyValue<TextAnchorType> getTextAnchor() const;
    void setTextAnchor(PropertyValue<TextAnchorType>);

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
    DataDrivenPropertyValue<float> getIconOpacity(const optional<std::string>& klass = {}) const;
    void setIconOpacity(DataDrivenPropertyValue<float>, const optional<std::string>& klass = {});
    void setIconOpacityTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getIconOpacityTransition(const optional<std::string>& klass = {}) const;

    static DataDrivenPropertyValue<Color> getDefaultIconColor();
    DataDrivenPropertyValue<Color> getIconColor(const optional<std::string>& klass = {}) const;
    void setIconColor(DataDrivenPropertyValue<Color>, const optional<std::string>& klass = {});
    void setIconColorTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getIconColorTransition(const optional<std::string>& klass = {}) const;

    static DataDrivenPropertyValue<Color> getDefaultIconHaloColor();
    DataDrivenPropertyValue<Color> getIconHaloColor(const optional<std::string>& klass = {}) const;
    void setIconHaloColor(DataDrivenPropertyValue<Color>, const optional<std::string>& klass = {});
    void setIconHaloColorTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getIconHaloColorTransition(const optional<std::string>& klass = {}) const;

    static DataDrivenPropertyValue<float> getDefaultIconHaloWidth();
    DataDrivenPropertyValue<float> getIconHaloWidth(const optional<std::string>& klass = {}) const;
    void setIconHaloWidth(DataDrivenPropertyValue<float>, const optional<std::string>& klass = {});
    void setIconHaloWidthTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getIconHaloWidthTransition(const optional<std::string>& klass = {}) const;

    static DataDrivenPropertyValue<float> getDefaultIconHaloBlur();
    DataDrivenPropertyValue<float> getIconHaloBlur(const optional<std::string>& klass = {}) const;
    void setIconHaloBlur(DataDrivenPropertyValue<float>, const optional<std::string>& klass = {});
    void setIconHaloBlurTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getIconHaloBlurTransition(const optional<std::string>& klass = {}) const;

    static PropertyValue<std::array<float, 2>> getDefaultIconTranslate();
    PropertyValue<std::array<float, 2>> getIconTranslate(const optional<std::string>& klass = {}) const;
    void setIconTranslate(PropertyValue<std::array<float, 2>>, const optional<std::string>& klass = {});
    void setIconTranslateTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getIconTranslateTransition(const optional<std::string>& klass = {}) const;

    static PropertyValue<TranslateAnchorType> getDefaultIconTranslateAnchor();
    PropertyValue<TranslateAnchorType> getIconTranslateAnchor(const optional<std::string>& klass = {}) const;
    void setIconTranslateAnchor(PropertyValue<TranslateAnchorType>, const optional<std::string>& klass = {});
    void setIconTranslateAnchorTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getIconTranslateAnchorTransition(const optional<std::string>& klass = {}) const;

    static DataDrivenPropertyValue<float> getDefaultTextOpacity();
    DataDrivenPropertyValue<float> getTextOpacity(const optional<std::string>& klass = {}) const;
    void setTextOpacity(DataDrivenPropertyValue<float>, const optional<std::string>& klass = {});
    void setTextOpacityTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getTextOpacityTransition(const optional<std::string>& klass = {}) const;

    static DataDrivenPropertyValue<Color> getDefaultTextColor();
    DataDrivenPropertyValue<Color> getTextColor(const optional<std::string>& klass = {}) const;
    void setTextColor(DataDrivenPropertyValue<Color>, const optional<std::string>& klass = {});
    void setTextColorTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getTextColorTransition(const optional<std::string>& klass = {}) const;

    static DataDrivenPropertyValue<Color> getDefaultTextHaloColor();
    DataDrivenPropertyValue<Color> getTextHaloColor(const optional<std::string>& klass = {}) const;
    void setTextHaloColor(DataDrivenPropertyValue<Color>, const optional<std::string>& klass = {});
    void setTextHaloColorTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getTextHaloColorTransition(const optional<std::string>& klass = {}) const;

    static DataDrivenPropertyValue<float> getDefaultTextHaloWidth();
    DataDrivenPropertyValue<float> getTextHaloWidth(const optional<std::string>& klass = {}) const;
    void setTextHaloWidth(DataDrivenPropertyValue<float>, const optional<std::string>& klass = {});
    void setTextHaloWidthTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getTextHaloWidthTransition(const optional<std::string>& klass = {}) const;

    static DataDrivenPropertyValue<float> getDefaultTextHaloBlur();
    DataDrivenPropertyValue<float> getTextHaloBlur(const optional<std::string>& klass = {}) const;
    void setTextHaloBlur(DataDrivenPropertyValue<float>, const optional<std::string>& klass = {});
    void setTextHaloBlurTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getTextHaloBlurTransition(const optional<std::string>& klass = {}) const;

    static PropertyValue<std::array<float, 2>> getDefaultTextTranslate();
    PropertyValue<std::array<float, 2>> getTextTranslate(const optional<std::string>& klass = {}) const;
    void setTextTranslate(PropertyValue<std::array<float, 2>>, const optional<std::string>& klass = {});
    void setTextTranslateTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getTextTranslateTransition(const optional<std::string>& klass = {}) const;

    static PropertyValue<TranslateAnchorType> getDefaultTextTranslateAnchor();
    PropertyValue<TranslateAnchorType> getTextTranslateAnchor(const optional<std::string>& klass = {}) const;
    void setTextTranslateAnchor(PropertyValue<TranslateAnchorType>, const optional<std::string>& klass = {});
    void setTextTranslateAnchorTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getTextTranslateAnchorTransition(const optional<std::string>& klass = {}) const;

    // Private implementation

    class Impl;
    Impl* const impl;

    SymbolLayer(const Impl&);
    SymbolLayer(const SymbolLayer&) = delete;
};

template <>
inline bool Layer::is<SymbolLayer>() const {
    return type == LayerType::Symbol;
}

} // namespace style
} // namespace mbgl
