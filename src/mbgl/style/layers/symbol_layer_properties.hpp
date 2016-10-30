// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>

namespace mbgl {
namespace style {

struct SymbolPlacement : LayoutProperty<SymbolPlacementType> {
    static SymbolPlacementType defaultValue() { return SymbolPlacementType::Point; }
};

struct SymbolSpacing : LayoutProperty<float> {
    static float defaultValue() { return 250; }
};

struct SymbolAvoidEdges : LayoutProperty<bool> {
    static bool defaultValue() { return false; }
};

struct IconAllowOverlap : LayoutProperty<bool> {
    static bool defaultValue() { return false; }
};

struct IconIgnorePlacement : LayoutProperty<bool> {
    static bool defaultValue() { return false; }
};

struct IconOptional : LayoutProperty<bool> {
    static bool defaultValue() { return false; }
};

struct IconRotationAlignment : LayoutProperty<AlignmentType> {
    static AlignmentType defaultValue() { return AlignmentType::Auto; }
};

struct IconSize : LayoutProperty<float> {
    static float defaultValue() { return 1; }
};

struct IconTextFit : LayoutProperty<IconTextFitType> {
    static IconTextFitType defaultValue() { return IconTextFitType::None; }
};

struct IconTextFitPadding : LayoutProperty<std::array<float, 4>> {
    static std::array<float, 4> defaultValue() { return {{ 0, 0, 0, 0 }}; }
};

struct IconImage : LayoutProperty<std::string> {
    static std::string defaultValue() { return ""; }
};

struct IconRotate : LayoutProperty<float> {
    static float defaultValue() { return 0; }
};

struct IconPadding : LayoutProperty<float> {
    static float defaultValue() { return 2; }
};

struct IconKeepUpright : LayoutProperty<bool> {
    static bool defaultValue() { return false; }
};

struct IconOffset : LayoutProperty<std::array<float, 2>> {
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct TextPitchAlignment : LayoutProperty<AlignmentType> {
    static AlignmentType defaultValue() { return AlignmentType::Auto; }
};

struct TextRotationAlignment : LayoutProperty<AlignmentType> {
    static AlignmentType defaultValue() { return AlignmentType::Auto; }
};

struct TextField : LayoutProperty<std::string> {
    static std::string defaultValue() { return ""; }
};

struct TextFont : LayoutProperty<std::vector<std::string>> {
    static std::vector<std::string> defaultValue() { return { "Open Sans Regular", "Arial Unicode MS Regular" }; }
};

struct TextSize : LayoutProperty<float> {
    static float defaultValue() { return 16; }
};

struct TextMaxWidth : LayoutProperty<float> {
    static float defaultValue() { return 10; }
};

struct TextLineHeight : LayoutProperty<float> {
    static float defaultValue() { return 1.2; }
};

struct TextLetterSpacing : LayoutProperty<float> {
    static float defaultValue() { return 0; }
};

struct TextJustify : LayoutProperty<TextJustifyType> {
    static TextJustifyType defaultValue() { return TextJustifyType::Center; }
};

struct TextAnchor : LayoutProperty<TextAnchorType> {
    static TextAnchorType defaultValue() { return TextAnchorType::Center; }
};

struct TextMaxAngle : LayoutProperty<float> {
    static float defaultValue() { return 45; }
};

struct TextRotate : LayoutProperty<float> {
    static float defaultValue() { return 0; }
};

struct TextPadding : LayoutProperty<float> {
    static float defaultValue() { return 2; }
};

struct TextKeepUpright : LayoutProperty<bool> {
    static bool defaultValue() { return true; }
};

struct TextTransform : LayoutProperty<TextTransformType> {
    static TextTransformType defaultValue() { return TextTransformType::None; }
};

struct TextOffset : LayoutProperty<std::array<float, 2>> {
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct TextAllowOverlap : LayoutProperty<bool> {
    static bool defaultValue() { return false; }
};

struct TextIgnorePlacement : LayoutProperty<bool> {
    static bool defaultValue() { return false; }
};

struct TextOptional : LayoutProperty<bool> {
    static bool defaultValue() { return false; }
};

struct IconOpacity : PaintProperty<float> {
    static float defaultValue() { return 1; }
};

struct IconColor : PaintProperty<Color> {
    static Color defaultValue() { return Color::black(); }
};

struct IconHaloColor : PaintProperty<Color> {
    static Color defaultValue() { return {}; }
};

struct IconHaloWidth : PaintProperty<float> {
    static float defaultValue() { return 0; }
};

struct IconHaloBlur : PaintProperty<float> {
    static float defaultValue() { return 0; }
};

struct IconTranslate : PaintProperty<std::array<float, 2>> {
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct IconTranslateAnchor : PaintProperty<TranslateAnchorType> {
    static TranslateAnchorType defaultValue() { return TranslateAnchorType::Map; }
};

struct TextOpacity : PaintProperty<float> {
    static float defaultValue() { return 1; }
};

struct TextColor : PaintProperty<Color> {
    static Color defaultValue() { return Color::black(); }
};

struct TextHaloColor : PaintProperty<Color> {
    static Color defaultValue() { return {}; }
};

struct TextHaloWidth : PaintProperty<float> {
    static float defaultValue() { return 0; }
};

struct TextHaloBlur : PaintProperty<float> {
    static float defaultValue() { return 0; }
};

struct TextTranslate : PaintProperty<std::array<float, 2>> {
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct TextTranslateAnchor : PaintProperty<TranslateAnchorType> {
    static TranslateAnchorType defaultValue() { return TranslateAnchorType::Map; }
};

class SymbolLayoutProperties : public LayoutProperties<
    SymbolPlacement,
    SymbolSpacing,
    SymbolAvoidEdges,
    IconAllowOverlap,
    IconIgnorePlacement,
    IconOptional,
    IconRotationAlignment,
    IconSize,
    IconTextFit,
    IconTextFitPadding,
    IconImage,
    IconRotate,
    IconPadding,
    IconKeepUpright,
    IconOffset,
    TextPitchAlignment,
    TextRotationAlignment,
    TextField,
    TextFont,
    TextSize,
    TextMaxWidth,
    TextLineHeight,
    TextLetterSpacing,
    TextJustify,
    TextAnchor,
    TextMaxAngle,
    TextRotate,
    TextPadding,
    TextKeepUpright,
    TextTransform,
    TextOffset,
    TextAllowOverlap,
    TextIgnorePlacement,
    TextOptional
> {};

class SymbolPaintProperties : public PaintProperties<
    IconOpacity,
    IconColor,
    IconHaloColor,
    IconHaloWidth,
    IconHaloBlur,
    IconTranslate,
    IconTranslateAnchor,
    TextOpacity,
    TextColor,
    TextHaloColor,
    TextHaloWidth,
    TextHaloBlur,
    TextTranslate,
    TextTranslateAnchor
> {};

} // namespace style
} // namespace mbgl
