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

    static PropertyValue<AlignmentType> getDefaultTextPitchAlignment();
    PropertyValue<AlignmentType> getTextPitchAlignment() const;
    void setTextPitchAlignment(PropertyValue<AlignmentType>);

    static PropertyValue<AlignmentType> getDefaultTextRotationAlignment();
    PropertyValue<AlignmentType> getTextRotationAlignment() const;
    void setTextRotationAlignment(PropertyValue<AlignmentType>);

    static PropertyValue<std::string> getDefaultTextField();
    PropertyValue<std::string> getTextField() const;
    void setTextField(PropertyValue<std::string>);

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

    static PropertyValue<TextAnchorType> getDefaultTextAnchor();
    PropertyValue<TextAnchorType> getTextAnchor() const;
    void setTextAnchor(PropertyValue<TextAnchorType>);

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
    PropertyValue<float> getIconOpacity(const optional<std::string>& klass = {}) const;
    void setIconOpacity(PropertyValue<float>, const optional<std::string>& klass = {});

    static PropertyValue<Color> getDefaultIconColor();
    PropertyValue<Color> getIconColor(const optional<std::string>& klass = {}) const;
    void setIconColor(PropertyValue<Color>, const optional<std::string>& klass = {});

    static PropertyValue<Color> getDefaultIconHaloColor();
    PropertyValue<Color> getIconHaloColor(const optional<std::string>& klass = {}) const;
    void setIconHaloColor(PropertyValue<Color>, const optional<std::string>& klass = {});

    static PropertyValue<float> getDefaultIconHaloWidth();
    PropertyValue<float> getIconHaloWidth(const optional<std::string>& klass = {}) const;
    void setIconHaloWidth(PropertyValue<float>, const optional<std::string>& klass = {});

    static PropertyValue<float> getDefaultIconHaloBlur();
    PropertyValue<float> getIconHaloBlur(const optional<std::string>& klass = {}) const;
    void setIconHaloBlur(PropertyValue<float>, const optional<std::string>& klass = {});

    static PropertyValue<std::array<float, 2>> getDefaultIconTranslate();
    PropertyValue<std::array<float, 2>> getIconTranslate(const optional<std::string>& klass = {}) const;
    void setIconTranslate(PropertyValue<std::array<float, 2>>, const optional<std::string>& klass = {});

    static PropertyValue<TranslateAnchorType> getDefaultIconTranslateAnchor();
    PropertyValue<TranslateAnchorType> getIconTranslateAnchor(const optional<std::string>& klass = {}) const;
    void setIconTranslateAnchor(PropertyValue<TranslateAnchorType>, const optional<std::string>& klass = {});

    static PropertyValue<float> getDefaultTextOpacity();
    PropertyValue<float> getTextOpacity(const optional<std::string>& klass = {}) const;
    void setTextOpacity(PropertyValue<float>, const optional<std::string>& klass = {});

    static PropertyValue<Color> getDefaultTextColor();
    PropertyValue<Color> getTextColor(const optional<std::string>& klass = {}) const;
    void setTextColor(PropertyValue<Color>, const optional<std::string>& klass = {});

    static PropertyValue<Color> getDefaultTextHaloColor();
    PropertyValue<Color> getTextHaloColor(const optional<std::string>& klass = {}) const;
    void setTextHaloColor(PropertyValue<Color>, const optional<std::string>& klass = {});

    static PropertyValue<float> getDefaultTextHaloWidth();
    PropertyValue<float> getTextHaloWidth(const optional<std::string>& klass = {}) const;
    void setTextHaloWidth(PropertyValue<float>, const optional<std::string>& klass = {});

    static PropertyValue<float> getDefaultTextHaloBlur();
    PropertyValue<float> getTextHaloBlur(const optional<std::string>& klass = {}) const;
    void setTextHaloBlur(PropertyValue<float>, const optional<std::string>& klass = {});

    static PropertyValue<std::array<float, 2>> getDefaultTextTranslate();
    PropertyValue<std::array<float, 2>> getTextTranslate(const optional<std::string>& klass = {}) const;
    void setTextTranslate(PropertyValue<std::array<float, 2>>, const optional<std::string>& klass = {});

    static PropertyValue<TranslateAnchorType> getDefaultTextTranslateAnchor();
    PropertyValue<TranslateAnchorType> getTextTranslateAnchor(const optional<std::string>& klass = {}) const;
    void setTextTranslateAnchor(PropertyValue<TranslateAnchorType>, const optional<std::string>& klass = {});

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
