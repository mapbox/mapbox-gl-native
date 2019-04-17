// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layer_properties.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/style/properties.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>

namespace mbgl {
namespace style {

struct SymbolPlacement : LayoutProperty<SymbolPlacementType> {
    static constexpr const char *name() { return "symbol-placement"; }
    static SymbolPlacementType defaultValue() { return SymbolPlacementType::Point; }
};

struct SymbolSpacing : LayoutProperty<float> {
    static constexpr const char *name() { return "symbol-spacing"; }
    static float defaultValue() { return 250; }
};

struct SymbolAvoidEdges : LayoutProperty<bool> {
    static constexpr const char *name() { return "symbol-avoid-edges"; }
    static bool defaultValue() { return false; }
};

struct SymbolSortKey : DataDrivenLayoutProperty<float> {
    static constexpr const char *name() { return "symbol-sort-key"; }
    static float defaultValue() { return 0; }
};

struct SymbolZOrder : LayoutProperty<SymbolZOrderType> {
    static constexpr const char *name() { return "symbol-z-order"; }
    static SymbolZOrderType defaultValue() { return SymbolZOrderType::Auto; }
};

struct IconAllowOverlap : LayoutProperty<bool> {
    static constexpr const char *name() { return "icon-allow-overlap"; }
    static bool defaultValue() { return false; }
};

struct IconIgnorePlacement : LayoutProperty<bool> {
    static constexpr const char *name() { return "icon-ignore-placement"; }
    static bool defaultValue() { return false; }
};

struct IconOptional : LayoutProperty<bool> {
    static constexpr const char *name() { return "icon-optional"; }
    static bool defaultValue() { return false; }
};

struct IconRotationAlignment : LayoutProperty<AlignmentType> {
    static constexpr const char *name() { return "icon-rotation-alignment"; }
    static AlignmentType defaultValue() { return AlignmentType::Auto; }
};

struct IconSize : DataDrivenLayoutProperty<float> {
    static constexpr const char *name() { return "icon-size"; }
    static float defaultValue() { return 1; }
};

struct IconTextFit : LayoutProperty<IconTextFitType> {
    static constexpr const char *name() { return "icon-text-fit"; }
    static IconTextFitType defaultValue() { return IconTextFitType::None; }
};

struct IconTextFitPadding : LayoutProperty<std::array<float, 4>> {
    static constexpr const char *name() { return "icon-text-fit-padding"; }
    static std::array<float, 4> defaultValue() { return {{ 0, 0, 0, 0 }}; }
};

struct IconImage : DataDrivenLayoutProperty<std::string> {
    static constexpr const char *name() { return "icon-image"; }
    static std::string defaultValue() { return ""; }
};

struct IconRotate : DataDrivenLayoutProperty<float> {
    static constexpr const char *name() { return "icon-rotate"; }
    static float defaultValue() { return 0; }
};

struct IconPadding : LayoutProperty<float> {
    static constexpr const char *name() { return "icon-padding"; }
    static float defaultValue() { return 2; }
};

struct IconKeepUpright : LayoutProperty<bool> {
    static constexpr const char *name() { return "icon-keep-upright"; }
    static bool defaultValue() { return false; }
};

struct IconOffset : DataDrivenLayoutProperty<std::array<float, 2>> {
    static constexpr const char *name() { return "icon-offset"; }
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct IconAnchor : DataDrivenLayoutProperty<SymbolAnchorType> {
    static constexpr const char *name() { return "icon-anchor"; }
    static SymbolAnchorType defaultValue() { return SymbolAnchorType::Center; }
};

struct IconPitchAlignment : LayoutProperty<AlignmentType> {
    static constexpr const char *name() { return "icon-pitch-alignment"; }
    static AlignmentType defaultValue() { return AlignmentType::Auto; }
};

struct TextPitchAlignment : LayoutProperty<AlignmentType> {
    static constexpr const char *name() { return "text-pitch-alignment"; }
    static AlignmentType defaultValue() { return AlignmentType::Auto; }
};

struct TextRotationAlignment : LayoutProperty<AlignmentType> {
    static constexpr const char *name() { return "text-rotation-alignment"; }
    static AlignmentType defaultValue() { return AlignmentType::Auto; }
};

struct TextField : DataDrivenLayoutProperty<expression::Formatted> {
    static constexpr const char *name() { return "text-field"; }
    static expression::Formatted defaultValue() { return ""; }
};

struct TextFont : DataDrivenLayoutProperty<std::vector<std::string>> {
    static constexpr const char *name() { return "text-font"; }
    static std::vector<std::string> defaultValue() { return { "Open Sans Regular", "Arial Unicode MS Regular" }; }
};

struct TextSize : DataDrivenLayoutProperty<float> {
    static constexpr const char *name() { return "text-size"; }
    static float defaultValue() { return 16; }
};

struct TextMaxWidth : DataDrivenLayoutProperty<float> {
    static constexpr const char *name() { return "text-max-width"; }
    static float defaultValue() { return 10; }
};

struct TextLineHeight : LayoutProperty<float> {
    static constexpr const char *name() { return "text-line-height"; }
    static float defaultValue() { return 1.2; }
};

struct TextLetterSpacing : DataDrivenLayoutProperty<float> {
    static constexpr const char *name() { return "text-letter-spacing"; }
    static float defaultValue() { return 0; }
};

struct TextJustify : DataDrivenLayoutProperty<TextJustifyType> {
    static constexpr const char *name() { return "text-justify"; }
    static TextJustifyType defaultValue() { return TextJustifyType::Center; }
};

struct TextRadialOffset : DataDrivenLayoutProperty<float> {
    static constexpr const char *name() { return "text-radial-offset"; }
    static float defaultValue() { return 0; }
};

struct TextVariableAnchor : LayoutProperty<std::vector<TextVariableAnchorType>> {
    static constexpr const char *name() { return "text-variable-anchor"; }
    static std::vector<TextVariableAnchorType> defaultValue() { return {  }; }
};

struct TextAnchor : DataDrivenLayoutProperty<SymbolAnchorType> {
    static constexpr const char *name() { return "text-anchor"; }
    static SymbolAnchorType defaultValue() { return SymbolAnchorType::Center; }
};

struct TextMaxAngle : LayoutProperty<float> {
    static constexpr const char *name() { return "text-max-angle"; }
    static float defaultValue() { return 45; }
};

struct TextRotate : DataDrivenLayoutProperty<float> {
    static constexpr const char *name() { return "text-rotate"; }
    static float defaultValue() { return 0; }
};

struct TextPadding : LayoutProperty<float> {
    static constexpr const char *name() { return "text-padding"; }
    static float defaultValue() { return 2; }
};

struct TextKeepUpright : LayoutProperty<bool> {
    static constexpr const char *name() { return "text-keep-upright"; }
    static bool defaultValue() { return true; }
};

struct TextTransform : DataDrivenLayoutProperty<TextTransformType> {
    static constexpr const char *name() { return "text-transform"; }
    static TextTransformType defaultValue() { return TextTransformType::None; }
};

struct TextOffset : DataDrivenLayoutProperty<std::array<float, 2>> {
    static constexpr const char *name() { return "text-offset"; }
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct TextAllowOverlap : LayoutProperty<bool> {
    static constexpr const char *name() { return "text-allow-overlap"; }
    static bool defaultValue() { return false; }
};

struct TextIgnorePlacement : LayoutProperty<bool> {
    static constexpr const char *name() { return "text-ignore-placement"; }
    static bool defaultValue() { return false; }
};

struct TextOptional : LayoutProperty<bool> {
    static constexpr const char *name() { return "text-optional"; }
    static bool defaultValue() { return false; }
};

struct IconOpacity : DataDrivenPaintProperty<float, attributes::opacity, uniforms::opacity> {
    static float defaultValue() { return 1; }
};

struct IconColor : DataDrivenPaintProperty<Color, attributes::fill_color, uniforms::fill_color> {
    static Color defaultValue() { return Color::black(); }
};

struct IconHaloColor : DataDrivenPaintProperty<Color, attributes::halo_color, uniforms::halo_color> {
    static Color defaultValue() { return {}; }
};

struct IconHaloWidth : DataDrivenPaintProperty<float, attributes::halo_width, uniforms::halo_width> {
    static float defaultValue() { return 0; }
};

struct IconHaloBlur : DataDrivenPaintProperty<float, attributes::halo_blur, uniforms::halo_blur> {
    static float defaultValue() { return 0; }
};

struct IconTranslate : PaintProperty<std::array<float, 2>> {
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct IconTranslateAnchor : PaintProperty<TranslateAnchorType> {
    static TranslateAnchorType defaultValue() { return TranslateAnchorType::Map; }
};

struct TextOpacity : DataDrivenPaintProperty<float, attributes::opacity, uniforms::opacity> {
    static float defaultValue() { return 1; }
};

struct TextColor : DataDrivenPaintProperty<Color, attributes::fill_color, uniforms::fill_color, true> {
    static Color defaultValue() { return Color::black(); }
    static constexpr const char *name() { return "text-color"; }
    static constexpr auto expressionType() { return expression::type::ColorType{}; };
    template<typename T> static bool hasOverride(const T& t) { return !!t.textColor; };
};

struct TextHaloColor : DataDrivenPaintProperty<Color, attributes::halo_color, uniforms::halo_color> {
    static Color defaultValue() { return {}; }
};

struct TextHaloWidth : DataDrivenPaintProperty<float, attributes::halo_width, uniforms::halo_width> {
    static float defaultValue() { return 0; }
};

struct TextHaloBlur : DataDrivenPaintProperty<float, attributes::halo_blur, uniforms::halo_blur> {
    static float defaultValue() { return 0; }
};

struct TextTranslate : PaintProperty<std::array<float, 2>> {
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct TextTranslateAnchor : PaintProperty<TranslateAnchorType> {
    static TranslateAnchorType defaultValue() { return TranslateAnchorType::Map; }
};

class SymbolLayoutProperties : public Properties<
    SymbolPlacement,
    SymbolSpacing,
    SymbolAvoidEdges,
    SymbolSortKey,
    SymbolZOrder,
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
    IconAnchor,
    IconPitchAlignment,
    TextPitchAlignment,
    TextRotationAlignment,
    TextField,
    TextFont,
    TextSize,
    TextMaxWidth,
    TextLineHeight,
    TextLetterSpacing,
    TextJustify,
    TextRadialOffset,
    TextVariableAnchor,
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

class SymbolPaintProperties : public Properties<
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

class SymbolLayerProperties final : public LayerProperties {
public:
    explicit SymbolLayerProperties(Immutable<SymbolLayer::Impl>);
    SymbolLayerProperties(
        Immutable<SymbolLayer::Impl>,
        SymbolPaintProperties::PossiblyEvaluated);
    ~SymbolLayerProperties() override;

    const SymbolLayer::Impl& layerImpl() const;
    // Data members.
    SymbolPaintProperties::PossiblyEvaluated evaluated;
};

} // namespace style
} // namespace mbgl
