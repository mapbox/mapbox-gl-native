// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>

#include <vector>

namespace mbgl {

class SymbolLayer : public Layer {
public:
    SymbolLayer(const std::string& layerID);
    ~SymbolLayer() final;

    // Source

    void setSource(const std::string& sourceID, const std::string& sourceLayer);
    const std::string& getSourceID() const;
    const std::string& getSourceLayer() const;

    // Layout properties

    Function<SymbolPlacementType> getSymbolPlacement() const;
    void setSymbolPlacement(Function<SymbolPlacementType>);

    Function<float> getSymbolSpacing() const;
    void setSymbolSpacing(Function<float>);

    Function<bool> getSymbolAvoidEdges() const;
    void setSymbolAvoidEdges(Function<bool>);

    Function<bool> getIconAllowOverlap() const;
    void setIconAllowOverlap(Function<bool>);

    Function<bool> getIconIgnorePlacement() const;
    void setIconIgnorePlacement(Function<bool>);

    Function<bool> getIconOptional() const;
    void setIconOptional(Function<bool>);

    Function<RotationAlignmentType> getIconRotationAlignment() const;
    void setIconRotationAlignment(Function<RotationAlignmentType>);

    Function<float> getIconSize() const;
    void setIconSize(Function<float>);

    Function<std::string> getIconImage() const;
    void setIconImage(Function<std::string>);

    Function<float> getIconRotate() const;
    void setIconRotate(Function<float>);

    Function<float> getIconPadding() const;
    void setIconPadding(Function<float>);

    Function<bool> getIconKeepUpright() const;
    void setIconKeepUpright(Function<bool>);

    Function<std::array<float, 2>> getIconOffset() const;
    void setIconOffset(Function<std::array<float, 2>>);

    Function<RotationAlignmentType> getTextRotationAlignment() const;
    void setTextRotationAlignment(Function<RotationAlignmentType>);

    Function<std::string> getTextField() const;
    void setTextField(Function<std::string>);

    Function<std::vector<std::string>> getTextFont() const;
    void setTextFont(Function<std::vector<std::string>>);

    Function<float> getTextSize() const;
    void setTextSize(Function<float>);

    Function<float> getTextMaxWidth() const;
    void setTextMaxWidth(Function<float>);

    Function<float> getTextLineHeight() const;
    void setTextLineHeight(Function<float>);

    Function<float> getTextLetterSpacing() const;
    void setTextLetterSpacing(Function<float>);

    Function<TextJustifyType> getTextJustify() const;
    void setTextJustify(Function<TextJustifyType>);

    Function<TextAnchorType> getTextAnchor() const;
    void setTextAnchor(Function<TextAnchorType>);

    Function<float> getTextMaxAngle() const;
    void setTextMaxAngle(Function<float>);

    Function<float> getTextRotate() const;
    void setTextRotate(Function<float>);

    Function<float> getTextPadding() const;
    void setTextPadding(Function<float>);

    Function<bool> getTextKeepUpright() const;
    void setTextKeepUpright(Function<bool>);

    Function<TextTransformType> getTextTransform() const;
    void setTextTransform(Function<TextTransformType>);

    Function<std::array<float, 2>> getTextOffset() const;
    void setTextOffset(Function<std::array<float, 2>>);

    Function<bool> getTextAllowOverlap() const;
    void setTextAllowOverlap(Function<bool>);

    Function<bool> getTextIgnorePlacement() const;
    void setTextIgnorePlacement(Function<bool>);

    Function<bool> getTextOptional() const;
    void setTextOptional(Function<bool>);

    // Paint properties

    Function<float> getIconOpacity() const;
    void setIconOpacity(Function<float>);

    Function<Color> getIconColor() const;
    void setIconColor(Function<Color>);

    Function<Color> getIconHaloColor() const;
    void setIconHaloColor(Function<Color>);

    Function<float> getIconHaloWidth() const;
    void setIconHaloWidth(Function<float>);

    Function<float> getIconHaloBlur() const;
    void setIconHaloBlur(Function<float>);

    Function<std::array<float, 2>> getIconTranslate() const;
    void setIconTranslate(Function<std::array<float, 2>>);

    Function<TranslateAnchorType> getIconTranslateAnchor() const;
    void setIconTranslateAnchor(Function<TranslateAnchorType>);

    Function<float> getTextOpacity() const;
    void setTextOpacity(Function<float>);

    Function<Color> getTextColor() const;
    void setTextColor(Function<Color>);

    Function<Color> getTextHaloColor() const;
    void setTextHaloColor(Function<Color>);

    Function<float> getTextHaloWidth() const;
    void setTextHaloWidth(Function<float>);

    Function<float> getTextHaloBlur() const;
    void setTextHaloBlur(Function<float>);

    Function<std::array<float, 2>> getTextTranslate() const;
    void setTextTranslate(Function<std::array<float, 2>>);

    Function<TranslateAnchorType> getTextTranslateAnchor() const;
    void setTextTranslateAnchor(Function<TranslateAnchorType>);

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

} // namespace mbgl
