// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>

#include <mbgl/util/color.hpp>

#include <vector>

namespace mbgl {
namespace style {

class SymbolLayer : public Layer {
public:
    SymbolLayer(const std::string& layerID);
    ~SymbolLayer() final;

    // Source

    void setSource(const std::string& sourceID, const std::string& sourceLayer);
    const std::string& getSourceID() const;
    const std::string& getSourceLayer() const;

    void setFilter(const Filter&);
    const Filter& getFilter() const;

    // Layout properties

    PropertyValue<SymbolPlacementType> getSymbolPlacement() const;
    void setSymbolPlacement(PropertyValue<SymbolPlacementType>);

    PropertyValue<float> getSymbolSpacing() const;
    void setSymbolSpacing(PropertyValue<float>);

    PropertyValue<bool> getSymbolAvoidEdges() const;
    void setSymbolAvoidEdges(PropertyValue<bool>);

    PropertyValue<bool> getIconAllowOverlap() const;
    void setIconAllowOverlap(PropertyValue<bool>);

    PropertyValue<bool> getIconIgnorePlacement() const;
    void setIconIgnorePlacement(PropertyValue<bool>);

    PropertyValue<bool> getIconOptional() const;
    void setIconOptional(PropertyValue<bool>);

    PropertyValue<AlignmentType> getIconRotationAlignment() const;
    void setIconRotationAlignment(PropertyValue<AlignmentType>);

    PropertyValue<float> getIconSize() const;
    void setIconSize(PropertyValue<float>);

    PropertyValue<IconTextFitType> getIconTextFit() const;
    void setIconTextFit(PropertyValue<IconTextFitType>);

    PropertyValue<std::array<float, 2>> getIconTextFitPadding() const;
    void setIconTextFitPadding(PropertyValue<std::array<float, 2>>);

    PropertyValue<std::string> getIconImage() const;
    void setIconImage(PropertyValue<std::string>);

    PropertyValue<float> getIconRotate() const;
    void setIconRotate(PropertyValue<float>);

    PropertyValue<float> getIconPadding() const;
    void setIconPadding(PropertyValue<float>);

    PropertyValue<bool> getIconKeepUpright() const;
    void setIconKeepUpright(PropertyValue<bool>);

    PropertyValue<std::array<float, 2>> getIconOffset() const;
    void setIconOffset(PropertyValue<std::array<float, 2>>);

    PropertyValue<AlignmentType> getTextPitchAlignment() const;
    void setTextPitchAlignment(PropertyValue<AlignmentType>);

    PropertyValue<AlignmentType> getTextRotationAlignment() const;
    void setTextRotationAlignment(PropertyValue<AlignmentType>);

    PropertyValue<std::string> getTextField() const;
    void setTextField(PropertyValue<std::string>);

    PropertyValue<std::vector<std::string>> getTextFont() const;
    void setTextFont(PropertyValue<std::vector<std::string>>);

    PropertyValue<float> getTextSize() const;
    void setTextSize(PropertyValue<float>);

    PropertyValue<float> getTextMaxWidth() const;
    void setTextMaxWidth(PropertyValue<float>);

    PropertyValue<float> getTextLineHeight() const;
    void setTextLineHeight(PropertyValue<float>);

    PropertyValue<float> getTextLetterSpacing() const;
    void setTextLetterSpacing(PropertyValue<float>);

    PropertyValue<TextJustifyType> getTextJustify() const;
    void setTextJustify(PropertyValue<TextJustifyType>);

    PropertyValue<TextAnchorType> getTextAnchor() const;
    void setTextAnchor(PropertyValue<TextAnchorType>);

    PropertyValue<float> getTextMaxAngle() const;
    void setTextMaxAngle(PropertyValue<float>);

    PropertyValue<float> getTextRotate() const;
    void setTextRotate(PropertyValue<float>);

    PropertyValue<float> getTextPadding() const;
    void setTextPadding(PropertyValue<float>);

    PropertyValue<bool> getTextKeepUpright() const;
    void setTextKeepUpright(PropertyValue<bool>);

    PropertyValue<TextTransformType> getTextTransform() const;
    void setTextTransform(PropertyValue<TextTransformType>);

    PropertyValue<std::array<float, 2>> getTextOffset() const;
    void setTextOffset(PropertyValue<std::array<float, 2>>);

    PropertyValue<bool> getTextAllowOverlap() const;
    void setTextAllowOverlap(PropertyValue<bool>);

    PropertyValue<bool> getTextIgnorePlacement() const;
    void setTextIgnorePlacement(PropertyValue<bool>);

    PropertyValue<bool> getTextOptional() const;
    void setTextOptional(PropertyValue<bool>);

    // Paint properties

    PropertyValue<float> getIconOpacity() const;
    void setIconOpacity(PropertyValue<float>);

    PropertyValue<Color> getIconColor() const;
    void setIconColor(PropertyValue<Color>);

    PropertyValue<Color> getIconHaloColor() const;
    void setIconHaloColor(PropertyValue<Color>);

    PropertyValue<float> getIconHaloWidth() const;
    void setIconHaloWidth(PropertyValue<float>);

    PropertyValue<float> getIconHaloBlur() const;
    void setIconHaloBlur(PropertyValue<float>);

    PropertyValue<std::array<float, 2>> getIconTranslate() const;
    void setIconTranslate(PropertyValue<std::array<float, 2>>);

    PropertyValue<TranslateAnchorType> getIconTranslateAnchor() const;
    void setIconTranslateAnchor(PropertyValue<TranslateAnchorType>);

    PropertyValue<float> getTextOpacity() const;
    void setTextOpacity(PropertyValue<float>);

    PropertyValue<Color> getTextColor() const;
    void setTextColor(PropertyValue<Color>);

    PropertyValue<Color> getTextHaloColor() const;
    void setTextHaloColor(PropertyValue<Color>);

    PropertyValue<float> getTextHaloWidth() const;
    void setTextHaloWidth(PropertyValue<float>);

    PropertyValue<float> getTextHaloBlur() const;
    void setTextHaloBlur(PropertyValue<float>);

    PropertyValue<std::array<float, 2>> getTextTranslate() const;
    void setTextTranslate(PropertyValue<std::array<float, 2>>);

    PropertyValue<TranslateAnchorType> getTextTranslateAnchor() const;
    void setTextTranslateAnchor(PropertyValue<TranslateAnchorType>);

    // Private implementation

    class Impl;
    Impl* const impl;

    SymbolLayer(const Impl&);
    SymbolLayer(const SymbolLayer&) = delete;
};

template <>
inline bool Layer::is<SymbolLayer>() const {
    return type == Type::Symbol;
}

} // namespace style
} // namespace mbgl
