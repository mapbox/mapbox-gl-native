#pragma once

// This file is generated. Edit make_property_setters.hpp.ejs, then run `make style-code`.

#include <mbgl/style/conversion/property_setter.hpp>

#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/background_layer.hpp>

#include <unordered_map>

namespace mbgl {
namespace style {
namespace conversion {

template <class V>
auto makeLayoutPropertySetters() {
    std::unordered_map<std::string, PropertySetter<V>> result;

    result["visibility"] = &setVisibility<V>;


    result["line-cap"] = &setProperty<V, LineLayer, PropertyValue<LineCapType>, &LineLayer::setLineCap>;
    result["line-join"] = &setProperty<V, LineLayer, DataDrivenPropertyValue<LineJoinType>, &LineLayer::setLineJoin>;
    result["line-miter-limit"] = &setProperty<V, LineLayer, PropertyValue<float>, &LineLayer::setLineMiterLimit>;
    result["line-round-limit"] = &setProperty<V, LineLayer, PropertyValue<float>, &LineLayer::setLineRoundLimit>;

    result["symbol-placement"] = &setProperty<V, SymbolLayer, PropertyValue<SymbolPlacementType>, &SymbolLayer::setSymbolPlacement>;
    result["symbol-spacing"] = &setProperty<V, SymbolLayer, PropertyValue<float>, &SymbolLayer::setSymbolSpacing>;
    result["symbol-avoid-edges"] = &setProperty<V, SymbolLayer, PropertyValue<bool>, &SymbolLayer::setSymbolAvoidEdges>;
    result["icon-allow-overlap"] = &setProperty<V, SymbolLayer, PropertyValue<bool>, &SymbolLayer::setIconAllowOverlap>;
    result["icon-ignore-placement"] = &setProperty<V, SymbolLayer, PropertyValue<bool>, &SymbolLayer::setIconIgnorePlacement>;
    result["icon-optional"] = &setProperty<V, SymbolLayer, PropertyValue<bool>, &SymbolLayer::setIconOptional>;
    result["icon-rotation-alignment"] = &setProperty<V, SymbolLayer, PropertyValue<AlignmentType>, &SymbolLayer::setIconRotationAlignment>;
    result["icon-size"] = &setProperty<V, SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setIconSize>;
    result["icon-text-fit"] = &setProperty<V, SymbolLayer, PropertyValue<IconTextFitType>, &SymbolLayer::setIconTextFit>;
    result["icon-text-fit-padding"] = &setProperty<V, SymbolLayer, PropertyValue<std::array<float, 4>>, &SymbolLayer::setIconTextFitPadding>;
    result["icon-image"] = &setProperty<V, SymbolLayer, DataDrivenPropertyValue<std::string>, &SymbolLayer::setIconImage>;
    result["icon-rotate"] = &setProperty<V, SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setIconRotate>;
    result["icon-padding"] = &setProperty<V, SymbolLayer, PropertyValue<float>, &SymbolLayer::setIconPadding>;
    result["icon-keep-upright"] = &setProperty<V, SymbolLayer, PropertyValue<bool>, &SymbolLayer::setIconKeepUpright>;
    result["icon-offset"] = &setProperty<V, SymbolLayer, DataDrivenPropertyValue<std::array<float, 2>>, &SymbolLayer::setIconOffset>;
    result["icon-pitch-alignment"] = &setProperty<V, SymbolLayer, PropertyValue<AlignmentType>, &SymbolLayer::setIconPitchAlignment>;
    result["text-pitch-alignment"] = &setProperty<V, SymbolLayer, PropertyValue<AlignmentType>, &SymbolLayer::setTextPitchAlignment>;
    result["text-rotation-alignment"] = &setProperty<V, SymbolLayer, PropertyValue<AlignmentType>, &SymbolLayer::setTextRotationAlignment>;
    result["text-field"] = &setProperty<V, SymbolLayer, DataDrivenPropertyValue<std::string>, &SymbolLayer::setTextField>;
    result["text-font"] = &setProperty<V, SymbolLayer, PropertyValue<std::vector<std::string>>, &SymbolLayer::setTextFont>;
    result["text-size"] = &setProperty<V, SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setTextSize>;
    result["text-max-width"] = &setProperty<V, SymbolLayer, PropertyValue<float>, &SymbolLayer::setTextMaxWidth>;
    result["text-line-height"] = &setProperty<V, SymbolLayer, PropertyValue<float>, &SymbolLayer::setTextLineHeight>;
    result["text-letter-spacing"] = &setProperty<V, SymbolLayer, PropertyValue<float>, &SymbolLayer::setTextLetterSpacing>;
    result["text-justify"] = &setProperty<V, SymbolLayer, DataDrivenPropertyValue<TextJustifyType>, &SymbolLayer::setTextJustify>;
    result["text-anchor"] = &setProperty<V, SymbolLayer, DataDrivenPropertyValue<TextAnchorType>, &SymbolLayer::setTextAnchor>;
    result["text-max-angle"] = &setProperty<V, SymbolLayer, PropertyValue<float>, &SymbolLayer::setTextMaxAngle>;
    result["text-rotate"] = &setProperty<V, SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setTextRotate>;
    result["text-padding"] = &setProperty<V, SymbolLayer, PropertyValue<float>, &SymbolLayer::setTextPadding>;
    result["text-keep-upright"] = &setProperty<V, SymbolLayer, PropertyValue<bool>, &SymbolLayer::setTextKeepUpright>;
    result["text-transform"] = &setProperty<V, SymbolLayer, DataDrivenPropertyValue<TextTransformType>, &SymbolLayer::setTextTransform>;
    result["text-offset"] = &setProperty<V, SymbolLayer, DataDrivenPropertyValue<std::array<float, 2>>, &SymbolLayer::setTextOffset>;
    result["text-allow-overlap"] = &setProperty<V, SymbolLayer, PropertyValue<bool>, &SymbolLayer::setTextAllowOverlap>;
    result["text-ignore-placement"] = &setProperty<V, SymbolLayer, PropertyValue<bool>, &SymbolLayer::setTextIgnorePlacement>;
    result["text-optional"] = &setProperty<V, SymbolLayer, PropertyValue<bool>, &SymbolLayer::setTextOptional>;





    return result;
}

template <class V>
auto makePaintPropertySetters() {
    std::unordered_map<std::string, PropertySetter<V>> result;

    result["fill-antialias"] = &setProperty<V, FillLayer, PropertyValue<bool>, &FillLayer::setFillAntialias>;
    result["fill-antialias-transition"] = &setTransition<V, FillLayer, &FillLayer::setFillAntialiasTransition>;
    result["fill-opacity"] = &setProperty<V, FillLayer, DataDrivenPropertyValue<float>, &FillLayer::setFillOpacity>;
    result["fill-opacity-transition"] = &setTransition<V, FillLayer, &FillLayer::setFillOpacityTransition>;
    result["fill-color"] = &setProperty<V, FillLayer, DataDrivenPropertyValue<Color>, &FillLayer::setFillColor>;
    result["fill-color-transition"] = &setTransition<V, FillLayer, &FillLayer::setFillColorTransition>;
    result["fill-outline-color"] = &setProperty<V, FillLayer, DataDrivenPropertyValue<Color>, &FillLayer::setFillOutlineColor>;
    result["fill-outline-color-transition"] = &setTransition<V, FillLayer, &FillLayer::setFillOutlineColorTransition>;
    result["fill-translate"] = &setProperty<V, FillLayer, PropertyValue<std::array<float, 2>>, &FillLayer::setFillTranslate>;
    result["fill-translate-transition"] = &setTransition<V, FillLayer, &FillLayer::setFillTranslateTransition>;
    result["fill-translate-anchor"] = &setProperty<V, FillLayer, PropertyValue<TranslateAnchorType>, &FillLayer::setFillTranslateAnchor>;
    result["fill-translate-anchor-transition"] = &setTransition<V, FillLayer, &FillLayer::setFillTranslateAnchorTransition>;
    result["fill-pattern"] = &setProperty<V, FillLayer, PropertyValue<std::string>, &FillLayer::setFillPattern>;
    result["fill-pattern-transition"] = &setTransition<V, FillLayer, &FillLayer::setFillPatternTransition>;

    result["line-opacity"] = &setProperty<V, LineLayer, DataDrivenPropertyValue<float>, &LineLayer::setLineOpacity>;
    result["line-opacity-transition"] = &setTransition<V, LineLayer, &LineLayer::setLineOpacityTransition>;
    result["line-color"] = &setProperty<V, LineLayer, DataDrivenPropertyValue<Color>, &LineLayer::setLineColor>;
    result["line-color-transition"] = &setTransition<V, LineLayer, &LineLayer::setLineColorTransition>;
    result["line-translate"] = &setProperty<V, LineLayer, PropertyValue<std::array<float, 2>>, &LineLayer::setLineTranslate>;
    result["line-translate-transition"] = &setTransition<V, LineLayer, &LineLayer::setLineTranslateTransition>;
    result["line-translate-anchor"] = &setProperty<V, LineLayer, PropertyValue<TranslateAnchorType>, &LineLayer::setLineTranslateAnchor>;
    result["line-translate-anchor-transition"] = &setTransition<V, LineLayer, &LineLayer::setLineTranslateAnchorTransition>;
    result["line-width"] = &setProperty<V, LineLayer, DataDrivenPropertyValue<float>, &LineLayer::setLineWidth>;
    result["line-width-transition"] = &setTransition<V, LineLayer, &LineLayer::setLineWidthTransition>;
    result["line-gap-width"] = &setProperty<V, LineLayer, DataDrivenPropertyValue<float>, &LineLayer::setLineGapWidth>;
    result["line-gap-width-transition"] = &setTransition<V, LineLayer, &LineLayer::setLineGapWidthTransition>;
    result["line-offset"] = &setProperty<V, LineLayer, DataDrivenPropertyValue<float>, &LineLayer::setLineOffset>;
    result["line-offset-transition"] = &setTransition<V, LineLayer, &LineLayer::setLineOffsetTransition>;
    result["line-blur"] = &setProperty<V, LineLayer, DataDrivenPropertyValue<float>, &LineLayer::setLineBlur>;
    result["line-blur-transition"] = &setTransition<V, LineLayer, &LineLayer::setLineBlurTransition>;
    result["line-dasharray"] = &setProperty<V, LineLayer, PropertyValue<std::vector<float>>, &LineLayer::setLineDasharray>;
    result["line-dasharray-transition"] = &setTransition<V, LineLayer, &LineLayer::setLineDasharrayTransition>;
    result["line-pattern"] = &setProperty<V, LineLayer, PropertyValue<std::string>, &LineLayer::setLinePattern>;
    result["line-pattern-transition"] = &setTransition<V, LineLayer, &LineLayer::setLinePatternTransition>;

    result["icon-opacity"] = &setProperty<V, SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setIconOpacity>;
    result["icon-opacity-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setIconOpacityTransition>;
    result["icon-color"] = &setProperty<V, SymbolLayer, DataDrivenPropertyValue<Color>, &SymbolLayer::setIconColor>;
    result["icon-color-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setIconColorTransition>;
    result["icon-halo-color"] = &setProperty<V, SymbolLayer, DataDrivenPropertyValue<Color>, &SymbolLayer::setIconHaloColor>;
    result["icon-halo-color-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setIconHaloColorTransition>;
    result["icon-halo-width"] = &setProperty<V, SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setIconHaloWidth>;
    result["icon-halo-width-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setIconHaloWidthTransition>;
    result["icon-halo-blur"] = &setProperty<V, SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setIconHaloBlur>;
    result["icon-halo-blur-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setIconHaloBlurTransition>;
    result["icon-translate"] = &setProperty<V, SymbolLayer, PropertyValue<std::array<float, 2>>, &SymbolLayer::setIconTranslate>;
    result["icon-translate-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setIconTranslateTransition>;
    result["icon-translate-anchor"] = &setProperty<V, SymbolLayer, PropertyValue<TranslateAnchorType>, &SymbolLayer::setIconTranslateAnchor>;
    result["icon-translate-anchor-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setIconTranslateAnchorTransition>;
    result["text-opacity"] = &setProperty<V, SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setTextOpacity>;
    result["text-opacity-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setTextOpacityTransition>;
    result["text-color"] = &setProperty<V, SymbolLayer, DataDrivenPropertyValue<Color>, &SymbolLayer::setTextColor>;
    result["text-color-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setTextColorTransition>;
    result["text-halo-color"] = &setProperty<V, SymbolLayer, DataDrivenPropertyValue<Color>, &SymbolLayer::setTextHaloColor>;
    result["text-halo-color-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setTextHaloColorTransition>;
    result["text-halo-width"] = &setProperty<V, SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setTextHaloWidth>;
    result["text-halo-width-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setTextHaloWidthTransition>;
    result["text-halo-blur"] = &setProperty<V, SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setTextHaloBlur>;
    result["text-halo-blur-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setTextHaloBlurTransition>;
    result["text-translate"] = &setProperty<V, SymbolLayer, PropertyValue<std::array<float, 2>>, &SymbolLayer::setTextTranslate>;
    result["text-translate-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setTextTranslateTransition>;
    result["text-translate-anchor"] = &setProperty<V, SymbolLayer, PropertyValue<TranslateAnchorType>, &SymbolLayer::setTextTranslateAnchor>;
    result["text-translate-anchor-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setTextTranslateAnchorTransition>;

    result["circle-radius"] = &setProperty<V, CircleLayer, DataDrivenPropertyValue<float>, &CircleLayer::setCircleRadius>;
    result["circle-radius-transition"] = &setTransition<V, CircleLayer, &CircleLayer::setCircleRadiusTransition>;
    result["circle-color"] = &setProperty<V, CircleLayer, DataDrivenPropertyValue<Color>, &CircleLayer::setCircleColor>;
    result["circle-color-transition"] = &setTransition<V, CircleLayer, &CircleLayer::setCircleColorTransition>;
    result["circle-blur"] = &setProperty<V, CircleLayer, DataDrivenPropertyValue<float>, &CircleLayer::setCircleBlur>;
    result["circle-blur-transition"] = &setTransition<V, CircleLayer, &CircleLayer::setCircleBlurTransition>;
    result["circle-opacity"] = &setProperty<V, CircleLayer, DataDrivenPropertyValue<float>, &CircleLayer::setCircleOpacity>;
    result["circle-opacity-transition"] = &setTransition<V, CircleLayer, &CircleLayer::setCircleOpacityTransition>;
    result["circle-translate"] = &setProperty<V, CircleLayer, PropertyValue<std::array<float, 2>>, &CircleLayer::setCircleTranslate>;
    result["circle-translate-transition"] = &setTransition<V, CircleLayer, &CircleLayer::setCircleTranslateTransition>;
    result["circle-translate-anchor"] = &setProperty<V, CircleLayer, PropertyValue<TranslateAnchorType>, &CircleLayer::setCircleTranslateAnchor>;
    result["circle-translate-anchor-transition"] = &setTransition<V, CircleLayer, &CircleLayer::setCircleTranslateAnchorTransition>;
    result["circle-pitch-scale"] = &setProperty<V, CircleLayer, PropertyValue<CirclePitchScaleType>, &CircleLayer::setCirclePitchScale>;
    result["circle-pitch-scale-transition"] = &setTransition<V, CircleLayer, &CircleLayer::setCirclePitchScaleTransition>;
    result["circle-pitch-alignment"] = &setProperty<V, CircleLayer, PropertyValue<AlignmentType>, &CircleLayer::setCirclePitchAlignment>;
    result["circle-pitch-alignment-transition"] = &setTransition<V, CircleLayer, &CircleLayer::setCirclePitchAlignmentTransition>;
    result["circle-stroke-width"] = &setProperty<V, CircleLayer, DataDrivenPropertyValue<float>, &CircleLayer::setCircleStrokeWidth>;
    result["circle-stroke-width-transition"] = &setTransition<V, CircleLayer, &CircleLayer::setCircleStrokeWidthTransition>;
    result["circle-stroke-color"] = &setProperty<V, CircleLayer, DataDrivenPropertyValue<Color>, &CircleLayer::setCircleStrokeColor>;
    result["circle-stroke-color-transition"] = &setTransition<V, CircleLayer, &CircleLayer::setCircleStrokeColorTransition>;
    result["circle-stroke-opacity"] = &setProperty<V, CircleLayer, DataDrivenPropertyValue<float>, &CircleLayer::setCircleStrokeOpacity>;
    result["circle-stroke-opacity-transition"] = &setTransition<V, CircleLayer, &CircleLayer::setCircleStrokeOpacityTransition>;

    result["fill-extrusion-opacity"] = &setProperty<V, FillExtrusionLayer, PropertyValue<float>, &FillExtrusionLayer::setFillExtrusionOpacity>;
    result["fill-extrusion-opacity-transition"] = &setTransition<V, FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionOpacityTransition>;
    result["fill-extrusion-color"] = &setProperty<V, FillExtrusionLayer, DataDrivenPropertyValue<Color>, &FillExtrusionLayer::setFillExtrusionColor>;
    result["fill-extrusion-color-transition"] = &setTransition<V, FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionColorTransition>;
    result["fill-extrusion-translate"] = &setProperty<V, FillExtrusionLayer, PropertyValue<std::array<float, 2>>, &FillExtrusionLayer::setFillExtrusionTranslate>;
    result["fill-extrusion-translate-transition"] = &setTransition<V, FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionTranslateTransition>;
    result["fill-extrusion-translate-anchor"] = &setProperty<V, FillExtrusionLayer, PropertyValue<TranslateAnchorType>, &FillExtrusionLayer::setFillExtrusionTranslateAnchor>;
    result["fill-extrusion-translate-anchor-transition"] = &setTransition<V, FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionTranslateAnchorTransition>;
    result["fill-extrusion-pattern"] = &setProperty<V, FillExtrusionLayer, PropertyValue<std::string>, &FillExtrusionLayer::setFillExtrusionPattern>;
    result["fill-extrusion-pattern-transition"] = &setTransition<V, FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionPatternTransition>;
    result["fill-extrusion-height"] = &setProperty<V, FillExtrusionLayer, DataDrivenPropertyValue<float>, &FillExtrusionLayer::setFillExtrusionHeight>;
    result["fill-extrusion-height-transition"] = &setTransition<V, FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionHeightTransition>;
    result["fill-extrusion-base"] = &setProperty<V, FillExtrusionLayer, DataDrivenPropertyValue<float>, &FillExtrusionLayer::setFillExtrusionBase>;
    result["fill-extrusion-base-transition"] = &setTransition<V, FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionBaseTransition>;

    result["raster-opacity"] = &setProperty<V, RasterLayer, PropertyValue<float>, &RasterLayer::setRasterOpacity>;
    result["raster-opacity-transition"] = &setTransition<V, RasterLayer, &RasterLayer::setRasterOpacityTransition>;
    result["raster-hue-rotate"] = &setProperty<V, RasterLayer, PropertyValue<float>, &RasterLayer::setRasterHueRotate>;
    result["raster-hue-rotate-transition"] = &setTransition<V, RasterLayer, &RasterLayer::setRasterHueRotateTransition>;
    result["raster-brightness-min"] = &setProperty<V, RasterLayer, PropertyValue<float>, &RasterLayer::setRasterBrightnessMin>;
    result["raster-brightness-min-transition"] = &setTransition<V, RasterLayer, &RasterLayer::setRasterBrightnessMinTransition>;
    result["raster-brightness-max"] = &setProperty<V, RasterLayer, PropertyValue<float>, &RasterLayer::setRasterBrightnessMax>;
    result["raster-brightness-max-transition"] = &setTransition<V, RasterLayer, &RasterLayer::setRasterBrightnessMaxTransition>;
    result["raster-saturation"] = &setProperty<V, RasterLayer, PropertyValue<float>, &RasterLayer::setRasterSaturation>;
    result["raster-saturation-transition"] = &setTransition<V, RasterLayer, &RasterLayer::setRasterSaturationTransition>;
    result["raster-contrast"] = &setProperty<V, RasterLayer, PropertyValue<float>, &RasterLayer::setRasterContrast>;
    result["raster-contrast-transition"] = &setTransition<V, RasterLayer, &RasterLayer::setRasterContrastTransition>;
    result["raster-fade-duration"] = &setProperty<V, RasterLayer, PropertyValue<float>, &RasterLayer::setRasterFadeDuration>;
    result["raster-fade-duration-transition"] = &setTransition<V, RasterLayer, &RasterLayer::setRasterFadeDurationTransition>;

    result["background-color"] = &setProperty<V, BackgroundLayer, PropertyValue<Color>, &BackgroundLayer::setBackgroundColor>;
    result["background-color-transition"] = &setTransition<V, BackgroundLayer, &BackgroundLayer::setBackgroundColorTransition>;
    result["background-pattern"] = &setProperty<V, BackgroundLayer, PropertyValue<std::string>, &BackgroundLayer::setBackgroundPattern>;
    result["background-pattern-transition"] = &setTransition<V, BackgroundLayer, &BackgroundLayer::setBackgroundPatternTransition>;
    result["background-opacity"] = &setProperty<V, BackgroundLayer, PropertyValue<float>, &BackgroundLayer::setBackgroundOpacity>;
    result["background-opacity-transition"] = &setTransition<V, BackgroundLayer, &BackgroundLayer::setBackgroundOpacityTransition>;

    return result;
}

} // namespace conversion
} // namespace style
} // namespace mbgl
