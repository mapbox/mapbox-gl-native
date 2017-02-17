// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/programs/attributes.hpp>

namespace mbgl {
namespace style {

struct SymbolPlacement : LayoutProperty<SymbolPlacementType> {
    static constexpr const char * key = "symbol-placement";
    static SymbolPlacementType defaultValue() { return SymbolPlacementType::Point; }
};

struct SymbolSpacing : LayoutProperty<float> {
    static constexpr const char * key = "symbol-spacing";
    static float defaultValue() { return 250; }
};

struct SymbolAvoidEdges : LayoutProperty<bool> {
    static constexpr const char * key = "symbol-avoid-edges";
    static bool defaultValue() { return false; }
};

struct IconAllowOverlap : LayoutProperty<bool> {
    static constexpr const char * key = "icon-allow-overlap";
    static bool defaultValue() { return false; }
};

struct IconIgnorePlacement : LayoutProperty<bool> {
    static constexpr const char * key = "icon-ignore-placement";
    static bool defaultValue() { return false; }
};

struct IconOptional : LayoutProperty<bool> {
    static constexpr const char * key = "icon-optional";
    static bool defaultValue() { return false; }
};

struct IconRotationAlignment : LayoutProperty<AlignmentType> {
    static constexpr const char * key = "icon-rotation-alignment";
    static AlignmentType defaultValue() { return AlignmentType::Auto; }
};

struct IconSize : LayoutProperty<float> {
    static constexpr const char * key = "icon-size";
    static float defaultValue() { return 1; }
};

struct IconTextFit : LayoutProperty<IconTextFitType> {
    static constexpr const char * key = "icon-text-fit";
    static IconTextFitType defaultValue() { return IconTextFitType::None; }
};

struct IconTextFitPadding : LayoutProperty<std::array<float, 4>> {
    static constexpr const char * key = "icon-text-fit-padding";
    static std::array<float, 4> defaultValue() { return {{ 0, 0, 0, 0 }}; }
};

struct IconImage : LayoutProperty<std::string> {
    static constexpr const char * key = "icon-image";
    static std::string defaultValue() { return ""; }
};

struct IconRotate : DataDrivenLayoutProperty<float> {
    static constexpr const char * key = "icon-rotate";
    static float defaultValue() { return 0; }
};

struct IconPadding : LayoutProperty<float> {
    static constexpr const char * key = "icon-padding";
    static float defaultValue() { return 2; }
};

struct IconKeepUpright : LayoutProperty<bool> {
    static constexpr const char * key = "icon-keep-upright";
    static bool defaultValue() { return false; }
};

struct IconOffset : DataDrivenLayoutProperty<std::array<float, 2>> {
    static constexpr const char * key = "icon-offset";
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct TextPitchAlignment : LayoutProperty<AlignmentType> {
    static constexpr const char * key = "text-pitch-alignment";
    static AlignmentType defaultValue() { return AlignmentType::Auto; }
};

struct TextRotationAlignment : LayoutProperty<AlignmentType> {
    static constexpr const char * key = "text-rotation-alignment";
    static AlignmentType defaultValue() { return AlignmentType::Auto; }
};

struct TextField : DataDrivenLayoutProperty<std::string> {
    static constexpr const char * key = "text-field";
    static std::string defaultValue() { return ""; }
};

struct TextFont : LayoutProperty<std::vector<std::string>> {
    static constexpr const char * key = "text-font";
    static std::vector<std::string> defaultValue() { return { "Open Sans Regular", "Arial Unicode MS Regular" }; }
};

struct TextSize : LayoutProperty<float> {
    static constexpr const char * key = "text-size";
    static float defaultValue() { return 16; }
};

struct TextMaxWidth : LayoutProperty<float> {
    static constexpr const char * key = "text-max-width";
    static float defaultValue() { return 10; }
};

struct TextLineHeight : LayoutProperty<float> {
    static constexpr const char * key = "text-line-height";
    static float defaultValue() { return 1.2; }
};

struct TextLetterSpacing : LayoutProperty<float> {
    static constexpr const char * key = "text-letter-spacing";
    static float defaultValue() { return 0; }
};

struct TextJustify : LayoutProperty<TextJustifyType> {
    static constexpr const char * key = "text-justify";
    static TextJustifyType defaultValue() { return TextJustifyType::Center; }
};

struct TextAnchor : LayoutProperty<TextAnchorType> {
    static constexpr const char * key = "text-anchor";
    static TextAnchorType defaultValue() { return TextAnchorType::Center; }
};

struct TextMaxAngle : LayoutProperty<float> {
    static constexpr const char * key = "text-max-angle";
    static float defaultValue() { return 45; }
};

struct TextRotate : LayoutProperty<float> {
    static constexpr const char * key = "text-rotate";
    static float defaultValue() { return 0; }
};

struct TextPadding : LayoutProperty<float> {
    static constexpr const char * key = "text-padding";
    static float defaultValue() { return 2; }
};

struct TextKeepUpright : LayoutProperty<bool> {
    static constexpr const char * key = "text-keep-upright";
    static bool defaultValue() { return true; }
};

struct TextTransform : DataDrivenLayoutProperty<TextTransformType> {
    static constexpr const char * key = "text-transform";
    static TextTransformType defaultValue() { return TextTransformType::None; }
};

struct TextOffset : LayoutProperty<std::array<float, 2>> {
    static constexpr const char * key = "text-offset";
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct TextAllowOverlap : LayoutProperty<bool> {
    static constexpr const char * key = "text-allow-overlap";
    static bool defaultValue() { return false; }
};

struct TextIgnorePlacement : LayoutProperty<bool> {
    static constexpr const char * key = "text-ignore-placement";
    static bool defaultValue() { return false; }
};

struct TextOptional : LayoutProperty<bool> {
    static constexpr const char * key = "text-optional";
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
