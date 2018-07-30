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


    result["line-cap"] = &setProperty<LineLayer, PropertyValue<LineCapType>, &LineLayer::setLineCap>;
    result["line-join"] = &setProperty<LineLayer, DataDrivenPropertyValue<LineJoinType>, &LineLayer::setLineJoin>;
    result["line-miter-limit"] = &setProperty<LineLayer, PropertyValue<float>, &LineLayer::setLineMiterLimit>;
    result["line-round-limit"] = &setProperty<LineLayer, PropertyValue<float>, &LineLayer::setLineRoundLimit>;

    result["symbol-placement"] = &setProperty<SymbolLayer, PropertyValue<SymbolPlacementType>, &SymbolLayer::setSymbolPlacement>;
    result["symbol-spacing"] = &setProperty<SymbolLayer, PropertyValue<float>, &SymbolLayer::setSymbolSpacing>;
    result["symbol-avoid-edges"] = &setProperty<SymbolLayer, PropertyValue<bool>, &SymbolLayer::setSymbolAvoidEdges>;
    result["icon-allow-overlap"] = &setProperty<SymbolLayer, PropertyValue<bool>, &SymbolLayer::setIconAllowOverlap>;
    result["icon-ignore-placement"] = &setProperty<SymbolLayer, PropertyValue<bool>, &SymbolLayer::setIconIgnorePlacement>;
    result["icon-optional"] = &setProperty<SymbolLayer, PropertyValue<bool>, &SymbolLayer::setIconOptional>;
    result["icon-rotation-alignment"] = &setProperty<SymbolLayer, PropertyValue<AlignmentType>, &SymbolLayer::setIconRotationAlignment>;
    result["icon-size"] = &setProperty<SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setIconSize>;
    result["icon-text-fit"] = &setProperty<SymbolLayer, PropertyValue<IconTextFitType>, &SymbolLayer::setIconTextFit>;
    result["icon-text-fit-padding"] = &setProperty<SymbolLayer, PropertyValue<std::array<float, 4>>, &SymbolLayer::setIconTextFitPadding>;
    result["icon-image"] = &setProperty<SymbolLayer, DataDrivenPropertyValue<std::string>, &SymbolLayer::setIconImage, true>;
    result["icon-rotate"] = &setProperty<SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setIconRotate>;
    result["icon-padding"] = &setProperty<SymbolLayer, PropertyValue<float>, &SymbolLayer::setIconPadding>;
    result["icon-keep-upright"] = &setProperty<SymbolLayer, PropertyValue<bool>, &SymbolLayer::setIconKeepUpright>;
    result["icon-offset"] = &setProperty<SymbolLayer, DataDrivenPropertyValue<std::array<float, 2>>, &SymbolLayer::setIconOffset>;
    result["icon-anchor"] = &setProperty<SymbolLayer, DataDrivenPropertyValue<SymbolAnchorType>, &SymbolLayer::setIconAnchor>;
    result["icon-pitch-alignment"] = &setProperty<SymbolLayer, PropertyValue<AlignmentType>, &SymbolLayer::setIconPitchAlignment>;
    result["text-pitch-alignment"] = &setProperty<SymbolLayer, PropertyValue<AlignmentType>, &SymbolLayer::setTextPitchAlignment>;
    result["text-rotation-alignment"] = &setProperty<SymbolLayer, PropertyValue<AlignmentType>, &SymbolLayer::setTextRotationAlignment>;
    result["text-field"] = &setProperty<SymbolLayer, DataDrivenPropertyValue<std::string>, &SymbolLayer::setTextField, true>;
    result["text-font"] = &setProperty<SymbolLayer, DataDrivenPropertyValue<std::vector<std::string>>, &SymbolLayer::setTextFont>;
    result["text-size"] = &setProperty<SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setTextSize>;
    result["text-max-width"] = &setProperty<SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setTextMaxWidth>;
    result["text-line-height"] = &setProperty<SymbolLayer, PropertyValue<float>, &SymbolLayer::setTextLineHeight>;
    result["text-letter-spacing"] = &setProperty<SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setTextLetterSpacing>;
    result["text-justify"] = &setProperty<SymbolLayer, DataDrivenPropertyValue<TextJustifyType>, &SymbolLayer::setTextJustify>;
    result["text-anchor"] = &setProperty<SymbolLayer, DataDrivenPropertyValue<SymbolAnchorType>, &SymbolLayer::setTextAnchor>;
    result["text-max-angle"] = &setProperty<SymbolLayer, PropertyValue<float>, &SymbolLayer::setTextMaxAngle>;
    result["text-rotate"] = &setProperty<SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setTextRotate>;
    result["text-padding"] = &setProperty<SymbolLayer, PropertyValue<float>, &SymbolLayer::setTextPadding>;
    result["text-keep-upright"] = &setProperty<SymbolLayer, PropertyValue<bool>, &SymbolLayer::setTextKeepUpright>;
    result["text-transform"] = &setProperty<SymbolLayer, DataDrivenPropertyValue<TextTransformType>, &SymbolLayer::setTextTransform>;
    result["text-offset"] = &setProperty<SymbolLayer, DataDrivenPropertyValue<std::array<float, 2>>, &SymbolLayer::setTextOffset>;
    result["text-allow-overlap"] = &setProperty<SymbolLayer, PropertyValue<bool>, &SymbolLayer::setTextAllowOverlap>;
    result["text-ignore-placement"] = &setProperty<SymbolLayer, PropertyValue<bool>, &SymbolLayer::setTextIgnorePlacement>;
    result["text-optional"] = &setProperty<SymbolLayer, PropertyValue<bool>, &SymbolLayer::setTextOptional>;







    return result;
}

inline auto makePaintPropertySetters() {
    std::unordered_map<std::string, PropertySetter> result;

    result["fill-antialias"] = &setProperty<FillLayer, PropertyValue<bool>, &FillLayer::setFillAntialias>;
    result["fill-antialias-transition"] = &setTransition<FillLayer, &FillLayer::setFillAntialiasTransition>;
    result["fill-opacity"] = &setProperty<FillLayer, DataDrivenPropertyValue<float>, &FillLayer::setFillOpacity>;
    result["fill-opacity-transition"] = &setTransition<FillLayer, &FillLayer::setFillOpacityTransition>;
    result["fill-color"] = &setProperty<FillLayer, DataDrivenPropertyValue<Color>, &FillLayer::setFillColor>;
    result["fill-color-transition"] = &setTransition<FillLayer, &FillLayer::setFillColorTransition>;
    result["fill-outline-color"] = &setProperty<FillLayer, DataDrivenPropertyValue<Color>, &FillLayer::setFillOutlineColor>;
    result["fill-outline-color-transition"] = &setTransition<FillLayer, &FillLayer::setFillOutlineColorTransition>;
    result["fill-translate"] = &setProperty<FillLayer, PropertyValue<std::array<float, 2>>, &FillLayer::setFillTranslate>;
    result["fill-translate-transition"] = &setTransition<FillLayer, &FillLayer::setFillTranslateTransition>;
    result["fill-translate-anchor"] = &setProperty<FillLayer, PropertyValue<TranslateAnchorType>, &FillLayer::setFillTranslateAnchor>;
    result["fill-translate-anchor-transition"] = &setTransition<FillLayer, &FillLayer::setFillTranslateAnchorTransition>;
    result["fill-pattern"] = &setProperty<FillLayer, PropertyValue<std::string>, &FillLayer::setFillPattern>;
    result["fill-pattern-transition"] = &setTransition<FillLayer, &FillLayer::setFillPatternTransition>;

    result["line-opacity"] = &setProperty<LineLayer, DataDrivenPropertyValue<float>, &LineLayer::setLineOpacity>;
    result["line-opacity-transition"] = &setTransition<LineLayer, &LineLayer::setLineOpacityTransition>;
    result["line-color"] = &setProperty<LineLayer, DataDrivenPropertyValue<Color>, &LineLayer::setLineColor>;
    result["line-color-transition"] = &setTransition<LineLayer, &LineLayer::setLineColorTransition>;
    result["line-translate"] = &setProperty<LineLayer, PropertyValue<std::array<float, 2>>, &LineLayer::setLineTranslate>;
    result["line-translate-transition"] = &setTransition<LineLayer, &LineLayer::setLineTranslateTransition>;
    result["line-translate-anchor"] = &setProperty<LineLayer, PropertyValue<TranslateAnchorType>, &LineLayer::setLineTranslateAnchor>;
    result["line-translate-anchor-transition"] = &setTransition<LineLayer, &LineLayer::setLineTranslateAnchorTransition>;
    result["line-width"] = &setProperty<LineLayer, DataDrivenPropertyValue<float>, &LineLayer::setLineWidth>;
    result["line-width-transition"] = &setTransition<LineLayer, &LineLayer::setLineWidthTransition>;
    result["line-gap-width"] = &setProperty<LineLayer, DataDrivenPropertyValue<float>, &LineLayer::setLineGapWidth>;
    result["line-gap-width-transition"] = &setTransition<LineLayer, &LineLayer::setLineGapWidthTransition>;
    result["line-offset"] = &setProperty<LineLayer, DataDrivenPropertyValue<float>, &LineLayer::setLineOffset>;
    result["line-offset-transition"] = &setTransition<LineLayer, &LineLayer::setLineOffsetTransition>;
    result["line-blur"] = &setProperty<LineLayer, DataDrivenPropertyValue<float>, &LineLayer::setLineBlur>;
    result["line-blur-transition"] = &setTransition<LineLayer, &LineLayer::setLineBlurTransition>;
    result["line-dasharray"] = &setProperty<LineLayer, PropertyValue<std::vector<float>>, &LineLayer::setLineDasharray>;
    result["line-dasharray-transition"] = &setTransition<LineLayer, &LineLayer::setLineDasharrayTransition>;
    result["line-pattern"] = &setProperty<LineLayer, PropertyValue<std::string>, &LineLayer::setLinePattern>;
    result["line-pattern-transition"] = &setTransition<LineLayer, &LineLayer::setLinePatternTransition>;

    result["icon-opacity"] = &setProperty<SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setIconOpacity>;
    result["icon-opacity-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setIconOpacityTransition>;
    result["icon-color"] = &setProperty<SymbolLayer, DataDrivenPropertyValue<Color>, &SymbolLayer::setIconColor>;
    result["icon-color-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setIconColorTransition>;
    result["icon-halo-color"] = &setProperty<SymbolLayer, DataDrivenPropertyValue<Color>, &SymbolLayer::setIconHaloColor>;
    result["icon-halo-color-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setIconHaloColorTransition>;
    result["icon-halo-width"] = &setProperty<SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setIconHaloWidth>;
    result["icon-halo-width-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setIconHaloWidthTransition>;
    result["icon-halo-blur"] = &setProperty<SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setIconHaloBlur>;
    result["icon-halo-blur-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setIconHaloBlurTransition>;
    result["icon-translate"] = &setProperty<SymbolLayer, PropertyValue<std::array<float, 2>>, &SymbolLayer::setIconTranslate>;
    result["icon-translate-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setIconTranslateTransition>;
    result["icon-translate-anchor"] = &setProperty<SymbolLayer, PropertyValue<TranslateAnchorType>, &SymbolLayer::setIconTranslateAnchor>;
    result["icon-translate-anchor-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setIconTranslateAnchorTransition>;
    result["text-opacity"] = &setProperty<SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setTextOpacity>;
    result["text-opacity-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setTextOpacityTransition>;
    result["text-color"] = &setProperty<SymbolLayer, DataDrivenPropertyValue<Color>, &SymbolLayer::setTextColor>;
    result["text-color-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setTextColorTransition>;
    result["text-halo-color"] = &setProperty<SymbolLayer, DataDrivenPropertyValue<Color>, &SymbolLayer::setTextHaloColor>;
    result["text-halo-color-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setTextHaloColorTransition>;
    result["text-halo-width"] = &setProperty<SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setTextHaloWidth>;
    result["text-halo-width-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setTextHaloWidthTransition>;
    result["text-halo-blur"] = &setProperty<SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setTextHaloBlur>;
    result["text-halo-blur-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setTextHaloBlurTransition>;
    result["text-translate"] = &setProperty<SymbolLayer, PropertyValue<std::array<float, 2>>, &SymbolLayer::setTextTranslate>;
    result["text-translate-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setTextTranslateTransition>;
    result["text-translate-anchor"] = &setProperty<SymbolLayer, PropertyValue<TranslateAnchorType>, &SymbolLayer::setTextTranslateAnchor>;
    result["text-translate-anchor-transition"] = &setTransition<SymbolLayer, &SymbolLayer::setTextTranslateAnchorTransition>;

    result["circle-radius"] = &setProperty<CircleLayer, DataDrivenPropertyValue<float>, &CircleLayer::setCircleRadius>;
    result["circle-radius-transition"] = &setTransition<CircleLayer, &CircleLayer::setCircleRadiusTransition>;
    result["circle-color"] = &setProperty<CircleLayer, DataDrivenPropertyValue<Color>, &CircleLayer::setCircleColor>;
    result["circle-color-transition"] = &setTransition<CircleLayer, &CircleLayer::setCircleColorTransition>;
    result["circle-blur"] = &setProperty<CircleLayer, DataDrivenPropertyValue<float>, &CircleLayer::setCircleBlur>;
    result["circle-blur-transition"] = &setTransition<CircleLayer, &CircleLayer::setCircleBlurTransition>;
    result["circle-opacity"] = &setProperty<CircleLayer, DataDrivenPropertyValue<float>, &CircleLayer::setCircleOpacity>;
    result["circle-opacity-transition"] = &setTransition<CircleLayer, &CircleLayer::setCircleOpacityTransition>;
    result["circle-translate"] = &setProperty<CircleLayer, PropertyValue<std::array<float, 2>>, &CircleLayer::setCircleTranslate>;
    result["circle-translate-transition"] = &setTransition<CircleLayer, &CircleLayer::setCircleTranslateTransition>;
    result["circle-translate-anchor"] = &setProperty<CircleLayer, PropertyValue<TranslateAnchorType>, &CircleLayer::setCircleTranslateAnchor>;
    result["circle-translate-anchor-transition"] = &setTransition<CircleLayer, &CircleLayer::setCircleTranslateAnchorTransition>;
    result["circle-pitch-scale"] = &setProperty<CircleLayer, PropertyValue<CirclePitchScaleType>, &CircleLayer::setCirclePitchScale>;
    result["circle-pitch-scale-transition"] = &setTransition<CircleLayer, &CircleLayer::setCirclePitchScaleTransition>;
    result["circle-pitch-alignment"] = &setProperty<CircleLayer, PropertyValue<AlignmentType>, &CircleLayer::setCirclePitchAlignment>;
    result["circle-pitch-alignment-transition"] = &setTransition<CircleLayer, &CircleLayer::setCirclePitchAlignmentTransition>;
    result["circle-stroke-width"] = &setProperty<CircleLayer, DataDrivenPropertyValue<float>, &CircleLayer::setCircleStrokeWidth>;
    result["circle-stroke-width-transition"] = &setTransition<CircleLayer, &CircleLayer::setCircleStrokeWidthTransition>;
    result["circle-stroke-color"] = &setProperty<CircleLayer, DataDrivenPropertyValue<Color>, &CircleLayer::setCircleStrokeColor>;
    result["circle-stroke-color-transition"] = &setTransition<CircleLayer, &CircleLayer::setCircleStrokeColorTransition>;
    result["circle-stroke-opacity"] = &setProperty<CircleLayer, DataDrivenPropertyValue<float>, &CircleLayer::setCircleStrokeOpacity>;
    result["circle-stroke-opacity-transition"] = &setTransition<CircleLayer, &CircleLayer::setCircleStrokeOpacityTransition>;

    result["heatmap-radius"] = &setProperty<HeatmapLayer, DataDrivenPropertyValue<float>, &HeatmapLayer::setHeatmapRadius>;
    result["heatmap-radius-transition"] = &setTransition<HeatmapLayer, &HeatmapLayer::setHeatmapRadiusTransition>;
    result["heatmap-weight"] = &setProperty<HeatmapLayer, DataDrivenPropertyValue<float>, &HeatmapLayer::setHeatmapWeight>;
    result["heatmap-weight-transition"] = &setTransition<HeatmapLayer, &HeatmapLayer::setHeatmapWeightTransition>;
    result["heatmap-intensity"] = &setProperty<HeatmapLayer, PropertyValue<float>, &HeatmapLayer::setHeatmapIntensity>;
    result["heatmap-intensity-transition"] = &setTransition<HeatmapLayer, &HeatmapLayer::setHeatmapIntensityTransition>;
    result["heatmap-color"] = &setProperty<HeatmapLayer, ColorRampPropertyValue, &HeatmapLayer::setHeatmapColor>;
    result["heatmap-color-transition"] = &setTransition<HeatmapLayer, &HeatmapLayer::setHeatmapColorTransition>;
    result["heatmap-opacity"] = &setProperty<HeatmapLayer, PropertyValue<float>, &HeatmapLayer::setHeatmapOpacity>;
    result["heatmap-opacity-transition"] = &setTransition<HeatmapLayer, &HeatmapLayer::setHeatmapOpacityTransition>;

    result["fill-extrusion-opacity"] = &setProperty<FillExtrusionLayer, PropertyValue<float>, &FillExtrusionLayer::setFillExtrusionOpacity>;
    result["fill-extrusion-opacity-transition"] = &setTransition<FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionOpacityTransition>;
    result["fill-extrusion-color"] = &setProperty<FillExtrusionLayer, DataDrivenPropertyValue<Color>, &FillExtrusionLayer::setFillExtrusionColor>;
    result["fill-extrusion-color-transition"] = &setTransition<FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionColorTransition>;
    result["fill-extrusion-translate"] = &setProperty<FillExtrusionLayer, PropertyValue<std::array<float, 2>>, &FillExtrusionLayer::setFillExtrusionTranslate>;
    result["fill-extrusion-translate-transition"] = &setTransition<FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionTranslateTransition>;
    result["fill-extrusion-translate-anchor"] = &setProperty<FillExtrusionLayer, PropertyValue<TranslateAnchorType>, &FillExtrusionLayer::setFillExtrusionTranslateAnchor>;
    result["fill-extrusion-translate-anchor-transition"] = &setTransition<FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionTranslateAnchorTransition>;
    result["fill-extrusion-pattern"] = &setProperty<FillExtrusionLayer, PropertyValue<std::string>, &FillExtrusionLayer::setFillExtrusionPattern>;
    result["fill-extrusion-pattern-transition"] = &setTransition<FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionPatternTransition>;
    result["fill-extrusion-height"] = &setProperty<FillExtrusionLayer, DataDrivenPropertyValue<float>, &FillExtrusionLayer::setFillExtrusionHeight>;
    result["fill-extrusion-height-transition"] = &setTransition<FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionHeightTransition>;
    result["fill-extrusion-base"] = &setProperty<FillExtrusionLayer, DataDrivenPropertyValue<float>, &FillExtrusionLayer::setFillExtrusionBase>;
    result["fill-extrusion-base-transition"] = &setTransition<FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionBaseTransition>;

    result["raster-opacity"] = &setProperty<RasterLayer, PropertyValue<float>, &RasterLayer::setRasterOpacity>;
    result["raster-opacity-transition"] = &setTransition<RasterLayer, &RasterLayer::setRasterOpacityTransition>;
    result["raster-hue-rotate"] = &setProperty<RasterLayer, PropertyValue<float>, &RasterLayer::setRasterHueRotate>;
    result["raster-hue-rotate-transition"] = &setTransition<RasterLayer, &RasterLayer::setRasterHueRotateTransition>;
    result["raster-brightness-min"] = &setProperty<RasterLayer, PropertyValue<float>, &RasterLayer::setRasterBrightnessMin>;
    result["raster-brightness-min-transition"] = &setTransition<RasterLayer, &RasterLayer::setRasterBrightnessMinTransition>;
    result["raster-brightness-max"] = &setProperty<RasterLayer, PropertyValue<float>, &RasterLayer::setRasterBrightnessMax>;
    result["raster-brightness-max-transition"] = &setTransition<RasterLayer, &RasterLayer::setRasterBrightnessMaxTransition>;
    result["raster-saturation"] = &setProperty<RasterLayer, PropertyValue<float>, &RasterLayer::setRasterSaturation>;
    result["raster-saturation-transition"] = &setTransition<RasterLayer, &RasterLayer::setRasterSaturationTransition>;
    result["raster-contrast"] = &setProperty<RasterLayer, PropertyValue<float>, &RasterLayer::setRasterContrast>;
    result["raster-contrast-transition"] = &setTransition<RasterLayer, &RasterLayer::setRasterContrastTransition>;
    result["raster-resampling"] = &setProperty<RasterLayer, PropertyValue<RasterResamplingType>, &RasterLayer::setRasterResampling>;
    result["raster-resampling-transition"] = &setTransition<RasterLayer, &RasterLayer::setRasterResamplingTransition>;
    result["raster-fade-duration"] = &setProperty<RasterLayer, PropertyValue<float>, &RasterLayer::setRasterFadeDuration>;
    result["raster-fade-duration-transition"] = &setTransition<RasterLayer, &RasterLayer::setRasterFadeDurationTransition>;

    result["hillshade-illumination-direction"] = &setProperty<HillshadeLayer, PropertyValue<float>, &HillshadeLayer::setHillshadeIlluminationDirection>;
    result["hillshade-illumination-direction-transition"] = &setTransition<HillshadeLayer, &HillshadeLayer::setHillshadeIlluminationDirectionTransition>;
    result["hillshade-illumination-anchor"] = &setProperty<HillshadeLayer, PropertyValue<HillshadeIlluminationAnchorType>, &HillshadeLayer::setHillshadeIlluminationAnchor>;
    result["hillshade-illumination-anchor-transition"] = &setTransition<HillshadeLayer, &HillshadeLayer::setHillshadeIlluminationAnchorTransition>;
    result["hillshade-exaggeration"] = &setProperty<HillshadeLayer, PropertyValue<float>, &HillshadeLayer::setHillshadeExaggeration>;
    result["hillshade-exaggeration-transition"] = &setTransition<HillshadeLayer, &HillshadeLayer::setHillshadeExaggerationTransition>;
    result["hillshade-shadow-color"] = &setProperty<HillshadeLayer, PropertyValue<Color>, &HillshadeLayer::setHillshadeShadowColor>;
    result["hillshade-shadow-color-transition"] = &setTransition<HillshadeLayer, &HillshadeLayer::setHillshadeShadowColorTransition>;
    result["hillshade-highlight-color"] = &setProperty<HillshadeLayer, PropertyValue<Color>, &HillshadeLayer::setHillshadeHighlightColor>;
    result["hillshade-highlight-color-transition"] = &setTransition<HillshadeLayer, &HillshadeLayer::setHillshadeHighlightColorTransition>;
    result["hillshade-accent-color"] = &setProperty<HillshadeLayer, PropertyValue<Color>, &HillshadeLayer::setHillshadeAccentColor>;
    result["hillshade-accent-color-transition"] = &setTransition<HillshadeLayer, &HillshadeLayer::setHillshadeAccentColorTransition>;

    result["background-color"] = &setProperty<BackgroundLayer, PropertyValue<Color>, &BackgroundLayer::setBackgroundColor>;
    result["background-color-transition"] = &setTransition<BackgroundLayer, &BackgroundLayer::setBackgroundColorTransition>;
    result["background-pattern"] = &setProperty<BackgroundLayer, PropertyValue<std::string>, &BackgroundLayer::setBackgroundPattern>;
    result["background-pattern-transition"] = &setTransition<BackgroundLayer, &BackgroundLayer::setBackgroundPatternTransition>;
    result["background-opacity"] = &setProperty<BackgroundLayer, PropertyValue<float>, &BackgroundLayer::setBackgroundOpacity>;
    result["background-opacity-transition"] = &setTransition<BackgroundLayer, &BackgroundLayer::setBackgroundOpacityTransition>;

    return result;
}

} // namespace conversion
} // namespace style
} // namespace mbgl
