// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/util/rapidjson.hpp>

namespace mbgl {
namespace style {

class CascadeParameters;
class CalculationParameters;

class SymbolLayoutProperties {
public:
    void parse(const JSValue&);
    void recalculate(const CalculationParameters&);

    LayoutProperty<SymbolPlacementType> symbolPlacement { SymbolPlacementType::Point };
    LayoutProperty<float> symbolSpacing { 250 };
    LayoutProperty<bool> symbolAvoidEdges { false };
    LayoutProperty<bool> iconAllowOverlap { false };
    LayoutProperty<bool> iconIgnorePlacement { false };
    LayoutProperty<bool> iconOptional { false };
    LayoutProperty<AlignmentType> iconRotationAlignment { AlignmentType::Viewport };
    LayoutProperty<float> iconSize { 1 };
    LayoutProperty<std::string> iconImage { "" };
    LayoutProperty<float> iconRotate { 0 };
    LayoutProperty<float> iconPadding { 2 };
    LayoutProperty<bool> iconKeepUpright { false };
    LayoutProperty<std::array<float, 2>> iconOffset { {{ 0, 0 }} };
    LayoutProperty<AlignmentType> textPitchAlignment { AlignmentType::Undefined };
    LayoutProperty<AlignmentType> textRotationAlignment { AlignmentType::Viewport };
    LayoutProperty<std::string> textField { "" };
    LayoutProperty<std::vector<std::string>> textFont { { "Open Sans Regular", "Arial Unicode MS Regular" } };
    LayoutProperty<float> textSize { 16 };
    LayoutProperty<float> textMaxWidth { 10 };
    LayoutProperty<float> textLineHeight { 1.2 };
    LayoutProperty<float> textLetterSpacing { 0 };
    LayoutProperty<TextJustifyType> textJustify { TextJustifyType::Center };
    LayoutProperty<TextAnchorType> textAnchor { TextAnchorType::Center };
    LayoutProperty<float> textMaxAngle { 45 };
    LayoutProperty<float> textRotate { 0 };
    LayoutProperty<float> textPadding { 2 };
    LayoutProperty<bool> textKeepUpright { true };
    LayoutProperty<TextTransformType> textTransform { TextTransformType::None };
    LayoutProperty<std::array<float, 2>> textOffset { {{ 0, 0 }} };
    LayoutProperty<bool> textAllowOverlap { false };
    LayoutProperty<bool> textIgnorePlacement { false };
    LayoutProperty<bool> textOptional { false };
};

class SymbolPaintProperties {
public:
    void parse(const JSValue&);
    void cascade(const CascadeParameters&);
    bool recalculate(const CalculationParameters&);

    PaintProperty<float> iconOpacity { 1 };
    PaintProperty<Color> iconColor { { 0, 0, 0, 1 } };
    PaintProperty<Color> iconHaloColor { { 0, 0, 0, 0 } };
    PaintProperty<float> iconHaloWidth { 0 };
    PaintProperty<float> iconHaloBlur { 0 };
    PaintProperty<std::array<float, 2>> iconTranslate { {{ 0, 0 }} };
    PaintProperty<TranslateAnchorType> iconTranslateAnchor { TranslateAnchorType::Map };
    PaintProperty<float> textOpacity { 1 };
    PaintProperty<Color> textColor { { 0, 0, 0, 1 } };
    PaintProperty<Color> textHaloColor { { 0, 0, 0, 0 } };
    PaintProperty<float> textHaloWidth { 0 };
    PaintProperty<float> textHaloBlur { 0 };
    PaintProperty<std::array<float, 2>> textTranslate { {{ 0, 0 }} };
    PaintProperty<TranslateAnchorType> textTranslateAnchor { TranslateAnchorType::Map };
};

} // namespace style
} // namespace mbgl
