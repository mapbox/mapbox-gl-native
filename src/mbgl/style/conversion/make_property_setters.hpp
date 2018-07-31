#pragma once

// This file is generated. Edit make_property_setters.hpp.ejs, then run `make style-code`.

#include <mbgl/style/conversion/property_setter.hpp>

#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/heatmap_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/hillshade_layer.hpp>
#include <mbgl/style/layers/background_layer.hpp>

#include <unordered_map>

namespace mbgl {
namespace style {
namespace conversion {

inline auto makeLayoutPropertySetters() {
    std::unordered_map<std::string, PropertySetter> result;

    result["visibility"] = &setVisibility;


    result["line-cap"] = &setProperty<LineLayer, PropertyValue<LineCapType>, &LineLayer::setLineCap, false, false>;
    result["line-join"] = &setProperty<LineLayer, PropertyValue<LineJoinType>, &LineLayer::setLineJoin, true, false>;
    result["line-miter-limit"] = &setProperty<LineLayer, PropertyValue<float>, &LineLayer::setLineMiterLimit, false, false>;
    result["line-round-limit"] = &setProperty<LineLayer, PropertyValue<float>, &LineLayer::setLineRoundLimit, false, false>;

    result["symbol-placement"] = &setProperty<SymbolLayer, PropertyValue<SymbolPlacementType>, &SymbolLayer::setSymbolPlacement, false, false>;
    result["symbol-spacing"] = &setProperty<SymbolLayer, PropertyValue<float>, &SymbolLayer::setSymbolSpacing, false, false>;
    result["symbol-avoid-edges"] = &setProperty<SymbolLayer, PropertyValue<bool>, &SymbolLayer::setSymbolAvoidEdges, false, false>;
    result["icon-allow-overlap"] = &setProperty<SymbolLayer, PropertyValue<bool>, &SymbolLayer::setIconAllowOverlap, false, false>;
    result["icon-ignore-placement"] = &setProperty<SymbolLayer, PropertyValue<bool>, &SymbolLayer::setIconIgnorePlacement, false, false>;
    result["icon-optional"] = &setProperty<SymbolLayer, PropertyValue<bool>, &SymbolLayer::setIconOptional, false, false>;
    result["icon-rotation-alignment"] = &setProperty<SymbolLayer, PropertyValue<AlignmentType>, &SymbolLayer::setIconRotationAlignment, false, false>;
    result["icon-size"] = &setProperty<SymbolLayer, PropertyValue<float>, &SymbolLayer::setIconSize, true, false>;
    result["icon-text-fit"] = &setProperty<SymbolLayer, PropertyValue<IconTextFitType>, &SymbolLayer::setIconTextFit, false, false>;
    result["icon-text-fit-padding"] = &setProperty<SymbolLayer, PropertyValue<std::array<float, 4>>, &SymbolLayer::setIconTextFitPadding, false, false>;
    result["icon-image"] = &setProperty<SymbolLayer, PropertyValue<std::string>, &SymbolLayer::setIconImage, true, true>;
    result["icon-rotate"] = &setProperty<SymbolLayer, PropertyValue<float>, &SymbolLayer::setIconRotate, true, false>;
    result["icon-padding"] = &setProperty<SymbolLayer, PropertyValue<float>, &SymbolLayer::setIconPadding, false, false>;
    result["icon-keep-upright"] = &setProperty<SymbolLayer, PropertyValue<bool>, &SymbolLayer::setIconKeepUpright, false, false>;
    result["icon-offset"] = &setProperty<SymbolLayer, PropertyValue<std::array<float, 2>>, &SymbolLayer::setIconOffset, true, false>;
    result["icon-anchor"] = &setProperty<SymbolLayer, PropertyValue<SymbolAnchorType>, &SymbolLayer::setIconAnchor, true, false>;
    result["icon-pitch-alignment"] = &setProperty<SymbolLayer, PropertyValue<AlignmentType>, &SymbolLayer::setIconPitchAlignment, false, false>;
    result["text-pitch-alignment"] = &setProperty<SymbolLayer, PropertyValue<AlignmentType>, &SymbolLayer::setTextPitchAlignment, false, false>;
    result["text-rotation-alignment"] = &setProperty<SymbolLayer, PropertyValue<AlignmentType>, &SymbolLayer::setTextRotationAlignment, false, false>;
    result["text-field"] = &setProperty<SymbolLayer, PropertyValue<std::string>, &SymbolLayer::setTextField, true, true>;
    result["text-font"] = &setProperty<SymbolLayer, PropertyValue<std::vector<std::string>>, &SymbolLayer::setTextFont, true, false>;
    result["text-size"] = &setProperty<SymbolLayer, PropertyValue<float>, &SymbolLayer::setTextSize, true, false>;
    result["text-max-width"] = &setProperty<SymbolLayer, PropertyValue<float>, &SymbolLayer::setTextMaxWidth, true, false>;
    result["text-line-height"] = &setProperty<SymbolLayer, PropertyValue<float>, &SymbolLayer::setTextLineHeight, false, false>;
    result["text-letter-spacing"] = &setProperty<SymbolLayer, PropertyValue<float>, &SymbolLayer::setTextLetterSpacing, true, false>;
    result["text-justify"] = &setProperty<SymbolLayer, PropertyValue<TextJustifyType>, &SymbolLayer::setTextJustify, true, false>;
    result["text-anchor"] = &setProperty<SymbolLayer, PropertyValue<SymbolAnchorType>, &SymbolLayer::setTextAnchor, true, false>;
    result["text-max-angle"] = &setProperty<SymbolLayer, PropertyValue<float>, &SymbolLayer::setTextMaxAngle, false, false>;
    result["text-rotate"] = &setProperty<SymbolLayer, PropertyValue<float>, &SymbolLayer::setTextRotate, true, false>;
    result["text-padding"] = &setProperty<SymbolLayer, PropertyValue<float>, &SymbolLayer::setTextPadding, false, false>;
    result["text-keep-upright"] = &setProperty<SymbolLayer, PropertyValue<bool>, &SymbolLayer::setTextKeepUpright, false, false>;
    result["text-transform"] = &setProperty<SymbolLayer, PropertyValue<TextTransformType>, &SymbolLayer::setTextTransform, true, false>;
    result["text-offset"] = &setProperty<SymbolLayer, PropertyValue<std::array<float, 2>>, &SymbolLayer::setTextOffset, true, false>;
    result["text-allow-overlap"] = &setProperty<SymbolLayer, PropertyValue<bool>, &SymbolLayer::setTextAllowOverlap, false, false>;
    result["text-ignore-placement"] = &setProperty<SymbolLayer, PropertyValue<bool>, &SymbolLayer::setTextIgnorePlacement, false, false>;
    result["text-optional"] = &setProperty<SymbolLayer, PropertyValue<bool>, &SymbolLayer::setTextOptional, false, false>;







    return result;
}

inline auto makePaintPropertySetters() {
    std::unordered_map<std::string, PropertySetter> result;

    result["fill-antialias"] = &setProperty<FillLayer, PropertyValue<bool>, &FillLayer::setFillAntialias, false, false>;
    result["fill-antialias-transition"] = &setTransition<FillLayer, &FillLayer::setFillAntialiasTransition>;
    result["fill-opacity"] = &setProperty<FillLayer, PropertyValue<float>, &FillLayer::setFillOpacity, true, false>;
    result["fill-opacity-transition"] = &setTransition<FillLayer, &FillLayer::setFillOpacityTransition>;
    result["fill-color"] = &setProperty<FillLayer, PropertyValue<Color>, &FillLayer::setFillColor, true, false>;
    result["fill-color-transition"] = &setTransition<FillLayer, &FillLayer::setFillColorTransition>;
    result["fill-outline-color"] = &setProperty<FillLayer, PropertyValue<Color>, &FillLayer::setFillOutlineColor, true, false>;
    result["fill-outline-color-transition"] = &setTransition<FillLayer, &FillLayer::setFillOutlineColorTransition>;
    result["fill-translate"] = &setProperty<FillLayer, PropertyValue<std::array<float, 2>>, &FillLayer::setFillTranslate, false, false>;
    result["fill-translate-transition"] = &setTransition<FillLayer, &FillLayer::setFillTranslateTransition>;
    result["fill-translate-anchor"] = &setProperty<FillLayer, PropertyValue<TranslateAnchorType>, &FillLayer::setFillTranslateAnchor, false, false>;
    result["fill-translate-anchor-transition"] = &setTransition<FillLayer, &FillLayer::setFillTranslateAnchorTransition>;
    result["fill-pattern"] = &setProperty<FillLayer, PropertyValue<std::string>, &FillLayer::setFillPattern, false, false>;
    result["fill-pattern-transition"] = &setTransition<FillLayer, &FillLayer::setFillPatternTransition>;

    result["line-opacity"] = &setProperty<LineLayer, PropertyValue<float>, &LineLayer::setLineOpacity, true, false>;
    result["line-opacity-transition"] = &setTransition<LineLayer, &LineLayer::setLineOpacityTransition>;
    result["line-color"] = &setProperty<LineLayer, PropertyValue<Color>, &LineLayer::setLineColor, true, false>;
    result["line-color-transition"] = &setTransition<LineLayer, &LineLayer::setLineColorTransition>;
    result["line-translate"] = &setProperty<LineLayer, PropertyValue<std::array<float, 2>>, &LineLayer::setLineTranslate, false, false>;
    result["line-translate-transition"] = &setTransition<LineLayer, &LineLayer::setLineTranslateTransition>;
    result["line-translate-anchor"] = &setProperty<LineLayer, PropertyValue<TranslateAnchorType>, &LineLayer::setLineTranslateAnchor, false, false>;
    result["line-translate-anchor-transition"] = &setTransition<LineLayer, &LineLayer::setLineTranslateAnchorTransition>;
    result["line-width"] = &setProperty<LineLayer, PropertyValue<float>, &LineLayer::setLineWidth, true, false>;
    result["line-width-transition"] = &setTransition<LineLayer, &LineLayer::setLineWidthTransition>;
    result["line-gap-width"] = &setProperty<LineLayer, PropertyValue<float>, &LineLayer::setLineGapWidth, true, false>;
    result["line-gap-width-transition"] = &setTransition<LineLayer, &LineLayer::setLineGapWidthTransition>;
    result["line-offset"] = &setProperty<LineLayer, PropertyValue<float>, &LineLayer::setLineOffset, true, false>;
    result["line-offset-transition"] = &setTransition<LineLayer, &LineLayer::setLineOffsetTransition>;
    result["line-blur"] = &setProperty<LineLayer, PropertyValue<float>, &LineLayer::setLineBlur, true, false>;
    result["line-blur-transition"] = &setTransition<LineLayer, &LineLayer::setLineBlurTransition>;
    result["line-dasharray"] = &setProperty<LineLayer, PropertyValue<std::vector<float>>, &LineLayer::setLineDasharray, false, false>;
    result["line-dasharray-transition"] = &setTransition<LineLayer, &LineLayer::setLineDasharrayTransition>;
    result["line-pattern"] = &setProperty<LineLayer, PropertyValue<std::string>, &LineLayer::setLinePattern, false, false>;
    result["line-pattern-transition"] = &setTransition<LineLayer, &LineLayer::setLinePatternTransition>;

    result["icon-opacity"] = &setProperty<SymbolLayer, PropertyValue<float>, &SymbolLayer::setIconOpacity, true, false>;
    result["icon-opacity-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setIconOpacityTransition>;
    result["icon-color"] = &setProperty<SymbolLayer, PropertyValue<Color>, &SymbolLayer::setIconColor, true, false>;
    result["icon-color-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setIconColorTransition>;
    result["icon-halo-color"] = &setProperty<SymbolLayer, PropertyValue<Color>, &SymbolLayer::setIconHaloColor, true, false>;
    result["icon-halo-color-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setIconHaloColorTransition>;
    result["icon-halo-width"] = &setProperty<SymbolLayer, PropertyValue<float>, &SymbolLayer::setIconHaloWidth, true, false>;
    result["icon-halo-width-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setIconHaloWidthTransition>;
    result["icon-halo-blur"] = &setProperty<SymbolLayer, PropertyValue<float>, &SymbolLayer::setIconHaloBlur, true, false>;
    result["icon-halo-blur-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setIconHaloBlurTransition>;
    result["icon-translate"] = &setProperty<SymbolLayer, PropertyValue<std::array<float, 2>>, &SymbolLayer::setIconTranslate, false, false>;
    result["icon-translate-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setIconTranslateTransition>;
    result["icon-translate-anchor"] = &setProperty<SymbolLayer, PropertyValue<TranslateAnchorType>, &SymbolLayer::setIconTranslateAnchor, false, false>;
    result["icon-translate-anchor-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setIconTranslateAnchorTransition>;
    result["text-opacity"] = &setProperty<SymbolLayer, PropertyValue<float>, &SymbolLayer::setTextOpacity, true, false>;
    result["text-opacity-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setTextOpacityTransition>;
    result["text-color"] = &setProperty<SymbolLayer, PropertyValue<Color>, &SymbolLayer::setTextColor, true, false>;
    result["text-color-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setTextColorTransition>;
    result["text-halo-color"] = &setProperty<SymbolLayer, PropertyValue<Color>, &SymbolLayer::setTextHaloColor, true, false>;
    result["text-halo-color-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setTextHaloColorTransition>;
    result["text-halo-width"] = &setProperty<SymbolLayer, PropertyValue<float>, &SymbolLayer::setTextHaloWidth, true, false>;
    result["text-halo-width-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setTextHaloWidthTransition>;
    result["text-halo-blur"] = &setProperty<SymbolLayer, PropertyValue<float>, &SymbolLayer::setTextHaloBlur, true, false>;
    result["text-halo-blur-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setTextHaloBlurTransition>;
    result["text-translate"] = &setProperty<SymbolLayer, PropertyValue<std::array<float, 2>>, &SymbolLayer::setTextTranslate, false, false>;
    result["text-translate-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setTextTranslateTransition>;
    result["text-translate-anchor"] = &setProperty<SymbolLayer, PropertyValue<TranslateAnchorType>, &SymbolLayer::setTextTranslateAnchor, false, false>;
    result["text-translate-anchor-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setTextTranslateAnchorTransition>;

    result["circle-radius"] = &setProperty<CircleLayer, PropertyValue<float>, &CircleLayer::setCircleRadius, true, false>;
    result["circle-radius-transition"] = &setTransition<CircleLayer, &CircleLayer::setCircleRadiusTransition>;
    result["circle-color"] = &setProperty<CircleLayer, PropertyValue<Color>, &CircleLayer::setCircleColor, true, false>;
    result["circle-color-transition"] = &setTransition<CircleLayer, &CircleLayer::setCircleColorTransition>;
    result["circle-blur"] = &setProperty<CircleLayer, PropertyValue<float>, &CircleLayer::setCircleBlur, true, false>;
    result["circle-blur-transition"] = &setTransition<CircleLayer, &CircleLayer::setCircleBlurTransition>;
    result["circle-opacity"] = &setProperty<CircleLayer, PropertyValue<float>, &CircleLayer::setCircleOpacity, true, false>;
    result["circle-opacity-transition"] = &setTransition<CircleLayer, &CircleLayer::setCircleOpacityTransition>;
    result["circle-translate"] = &setProperty<CircleLayer, PropertyValue<std::array<float, 2>>, &CircleLayer::setCircleTranslate, false, false>;
    result["circle-translate-transition"] = &setTransition<CircleLayer, &CircleLayer::setCircleTranslateTransition>;
    result["circle-translate-anchor"] = &setProperty<CircleLayer, PropertyValue<TranslateAnchorType>, &CircleLayer::setCircleTranslateAnchor, false, false>;
    result["circle-translate-anchor-transition"] = &setTransition<CircleLayer, &CircleLayer::setCircleTranslateAnchorTransition>;
    result["circle-pitch-scale"] = &setProperty<CircleLayer, PropertyValue<CirclePitchScaleType>, &CircleLayer::setCirclePitchScale, false, false>;
    result["circle-pitch-scale-transition"] = &setTransition<CircleLayer, &CircleLayer::setCirclePitchScaleTransition>;
    result["circle-pitch-alignment"] = &setProperty<CircleLayer, PropertyValue<AlignmentType>, &CircleLayer::setCirclePitchAlignment, false, false>;
    result["circle-pitch-alignment-transition"] = &setTransition<CircleLayer, &CircleLayer::setCirclePitchAlignmentTransition>;
    result["circle-stroke-width"] = &setProperty<CircleLayer, PropertyValue<float>, &CircleLayer::setCircleStrokeWidth, true, false>;
    result["circle-stroke-width-transition"] = &setTransition<CircleLayer, &CircleLayer::setCircleStrokeWidthTransition>;
    result["circle-stroke-color"] = &setProperty<CircleLayer, PropertyValue<Color>, &CircleLayer::setCircleStrokeColor, true, false>;
    result["circle-stroke-color-transition"] = &setTransition<CircleLayer, &CircleLayer::setCircleStrokeColorTransition>;
    result["circle-stroke-opacity"] = &setProperty<CircleLayer, PropertyValue<float>, &CircleLayer::setCircleStrokeOpacity, true, false>;
    result["circle-stroke-opacity-transition"] = &setTransition<CircleLayer, &CircleLayer::setCircleStrokeOpacityTransition>;

    result["heatmap-radius"] = &setProperty<HeatmapLayer, PropertyValue<float>, &HeatmapLayer::setHeatmapRadius, true, false>;
    result["heatmap-radius-transition"] = &setTransition<HeatmapLayer, &HeatmapLayer::setHeatmapRadiusTransition>;
    result["heatmap-weight"] = &setProperty<HeatmapLayer, PropertyValue<float>, &HeatmapLayer::setHeatmapWeight, true, false>;
    result["heatmap-weight-transition"] = &setTransition<HeatmapLayer, &HeatmapLayer::setHeatmapWeightTransition>;
    result["heatmap-intensity"] = &setProperty<HeatmapLayer, PropertyValue<float>, &HeatmapLayer::setHeatmapIntensity, false, false>;
    result["heatmap-intensity-transition"] = &setTransition<HeatmapLayer, &HeatmapLayer::setHeatmapIntensityTransition>;
    result["heatmap-color"] = &setProperty<HeatmapLayer, ColorRampPropertyValue, &HeatmapLayer::setHeatmapColor, false, false>;
    result["heatmap-color-transition"] = &setTransition<HeatmapLayer, &HeatmapLayer::setHeatmapColorTransition>;
    result["heatmap-opacity"] = &setProperty<HeatmapLayer, PropertyValue<float>, &HeatmapLayer::setHeatmapOpacity, false, false>;
    result["heatmap-opacity-transition"] = &setTransition<HeatmapLayer, &HeatmapLayer::setHeatmapOpacityTransition>;

    result["fill-extrusion-opacity"] = &setProperty<FillExtrusionLayer, PropertyValue<float>, &FillExtrusionLayer::setFillExtrusionOpacity, false, false>;
    result["fill-extrusion-opacity-transition"] = &setTransition<FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionOpacityTransition>;
    result["fill-extrusion-color"] = &setProperty<FillExtrusionLayer, PropertyValue<Color>, &FillExtrusionLayer::setFillExtrusionColor, true, false>;
    result["fill-extrusion-color-transition"] = &setTransition<FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionColorTransition>;
    result["fill-extrusion-translate"] = &setProperty<FillExtrusionLayer, PropertyValue<std::array<float, 2>>, &FillExtrusionLayer::setFillExtrusionTranslate, false, false>;
    result["fill-extrusion-translate-transition"] = &setTransition<FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionTranslateTransition>;
    result["fill-extrusion-translate-anchor"] = &setProperty<FillExtrusionLayer, PropertyValue<TranslateAnchorType>, &FillExtrusionLayer::setFillExtrusionTranslateAnchor, false, false>;
    result["fill-extrusion-translate-anchor-transition"] = &setTransition<FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionTranslateAnchorTransition>;
    result["fill-extrusion-pattern"] = &setProperty<FillExtrusionLayer, PropertyValue<std::string>, &FillExtrusionLayer::setFillExtrusionPattern, false, false>;
    result["fill-extrusion-pattern-transition"] = &setTransition<FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionPatternTransition>;
    result["fill-extrusion-height"] = &setProperty<FillExtrusionLayer, PropertyValue<float>, &FillExtrusionLayer::setFillExtrusionHeight, true, false>;
    result["fill-extrusion-height-transition"] = &setTransition<FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionHeightTransition>;
    result["fill-extrusion-base"] = &setProperty<FillExtrusionLayer, PropertyValue<float>, &FillExtrusionLayer::setFillExtrusionBase, true, false>;
    result["fill-extrusion-base-transition"] = &setTransition<FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionBaseTransition>;

    result["raster-opacity"] = &setProperty<RasterLayer, PropertyValue<float>, &RasterLayer::setRasterOpacity, false, false>;
    result["raster-opacity-transition"] = &setTransition<RasterLayer, &RasterLayer::setRasterOpacityTransition>;
    result["raster-hue-rotate"] = &setProperty<RasterLayer, PropertyValue<float>, &RasterLayer::setRasterHueRotate, false, false>;
    result["raster-hue-rotate-transition"] = &setTransition<RasterLayer, &RasterLayer::setRasterHueRotateTransition>;
    result["raster-brightness-min"] = &setProperty<RasterLayer, PropertyValue<float>, &RasterLayer::setRasterBrightnessMin, false, false>;
    result["raster-brightness-min-transition"] = &setTransition<RasterLayer, &RasterLayer::setRasterBrightnessMinTransition>;
    result["raster-brightness-max"] = &setProperty<RasterLayer, PropertyValue<float>, &RasterLayer::setRasterBrightnessMax, false, false>;
    result["raster-brightness-max-transition"] = &setTransition<RasterLayer, &RasterLayer::setRasterBrightnessMaxTransition>;
    result["raster-saturation"] = &setProperty<RasterLayer, PropertyValue<float>, &RasterLayer::setRasterSaturation, false, false>;
    result["raster-saturation-transition"] = &setTransition<RasterLayer, &RasterLayer::setRasterSaturationTransition>;
    result["raster-contrast"] = &setProperty<RasterLayer, PropertyValue<float>, &RasterLayer::setRasterContrast, false, false>;
    result["raster-contrast-transition"] = &setTransition<RasterLayer, &RasterLayer::setRasterContrastTransition>;
    result["raster-resampling"] = &setProperty<RasterLayer, PropertyValue<RasterResamplingType>, &RasterLayer::setRasterResampling, false, false>;
    result["raster-resampling-transition"] = &setTransition<RasterLayer, &RasterLayer::setRasterResamplingTransition>;
    result["raster-fade-duration"] = &setProperty<RasterLayer, PropertyValue<float>, &RasterLayer::setRasterFadeDuration, false, false>;
    result["raster-fade-duration-transition"] = &setTransition<RasterLayer, &RasterLayer::setRasterFadeDurationTransition>;

    result["hillshade-illumination-direction"] = &setProperty<HillshadeLayer, PropertyValue<float>, &HillshadeLayer::setHillshadeIlluminationDirection, false, false>;
    result["hillshade-illumination-direction-transition"] = &setTransition<HillshadeLayer, &HillshadeLayer::setHillshadeIlluminationDirectionTransition>;
    result["hillshade-illumination-anchor"] = &setProperty<HillshadeLayer, PropertyValue<HillshadeIlluminationAnchorType>, &HillshadeLayer::setHillshadeIlluminationAnchor, false, false>;
    result["hillshade-illumination-anchor-transition"] = &setTransition<HillshadeLayer, &HillshadeLayer::setHillshadeIlluminationAnchorTransition>;
    result["hillshade-exaggeration"] = &setProperty<HillshadeLayer, PropertyValue<float>, &HillshadeLayer::setHillshadeExaggeration, false, false>;
    result["hillshade-exaggeration-transition"] = &setTransition<HillshadeLayer, &HillshadeLayer::setHillshadeExaggerationTransition>;
    result["hillshade-shadow-color"] = &setProperty<HillshadeLayer, PropertyValue<Color>, &HillshadeLayer::setHillshadeShadowColor, false, false>;
    result["hillshade-shadow-color-transition"] = &setTransition<HillshadeLayer, &HillshadeLayer::setHillshadeShadowColorTransition>;
    result["hillshade-highlight-color"] = &setProperty<HillshadeLayer, PropertyValue<Color>, &HillshadeLayer::setHillshadeHighlightColor, false, false>;
    result["hillshade-highlight-color-transition"] = &setTransition<HillshadeLayer, &HillshadeLayer::setHillshadeHighlightColorTransition>;
    result["hillshade-accent-color"] = &setProperty<HillshadeLayer, PropertyValue<Color>, &HillshadeLayer::setHillshadeAccentColor, false, false>;
    result["hillshade-accent-color-transition"] = &setTransition<HillshadeLayer, &HillshadeLayer::setHillshadeAccentColorTransition>;

    result["background-color"] = &setProperty<BackgroundLayer, PropertyValue<Color>, &BackgroundLayer::setBackgroundColor, false, false>;
    result["background-color-transition"] = &setTransition<BackgroundLayer, &BackgroundLayer::setBackgroundColorTransition>;
    result["background-pattern"] = &setProperty<BackgroundLayer, PropertyValue<std::string>, &BackgroundLayer::setBackgroundPattern, false, false>;
    result["background-pattern-transition"] = &setTransition<BackgroundLayer, &BackgroundLayer::setBackgroundPatternTransition>;
    result["background-opacity"] = &setProperty<BackgroundLayer, PropertyValue<float>, &BackgroundLayer::setBackgroundOpacity, false, false>;
    result["background-opacity-transition"] = &setTransition<BackgroundLayer, &BackgroundLayer::setBackgroundOpacityTransition>;

    return result;
}

} // namespace conversion
} // namespace style
} // namespace mbgl
