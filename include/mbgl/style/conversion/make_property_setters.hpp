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
    std::unordered_map<std::string, LayoutPropertySetter<V>> result;

    result["visibility"] = &setVisibility<V>;


    result["line-cap"] = &setLayoutProperty<V, LineLayer, PropertyValue<LineCapType>, &LineLayer::setLineCap>;
    result["line-join"] = &setLayoutProperty<V, LineLayer, PropertyValue<LineJoinType>, &LineLayer::setLineJoin>;
    result["line-miter-limit"] = &setLayoutProperty<V, LineLayer, PropertyValue<float>, &LineLayer::setLineMiterLimit>;
    result["line-round-limit"] = &setLayoutProperty<V, LineLayer, PropertyValue<float>, &LineLayer::setLineRoundLimit>;

    result["symbol-placement"] = &setLayoutProperty<V, SymbolLayer, PropertyValue<SymbolPlacementType>, &SymbolLayer::setSymbolPlacement>;
    result["symbol-spacing"] = &setLayoutProperty<V, SymbolLayer, PropertyValue<float>, &SymbolLayer::setSymbolSpacing>;
    result["symbol-avoid-edges"] = &setLayoutProperty<V, SymbolLayer, PropertyValue<bool>, &SymbolLayer::setSymbolAvoidEdges>;
    result["icon-allow-overlap"] = &setLayoutProperty<V, SymbolLayer, PropertyValue<bool>, &SymbolLayer::setIconAllowOverlap>;
    result["icon-ignore-placement"] = &setLayoutProperty<V, SymbolLayer, PropertyValue<bool>, &SymbolLayer::setIconIgnorePlacement>;
    result["icon-optional"] = &setLayoutProperty<V, SymbolLayer, PropertyValue<bool>, &SymbolLayer::setIconOptional>;
    result["icon-rotation-alignment"] = &setLayoutProperty<V, SymbolLayer, PropertyValue<AlignmentType>, &SymbolLayer::setIconRotationAlignment>;
    result["icon-size"] = &setLayoutProperty<V, SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setIconSize>;
    result["icon-text-fit"] = &setLayoutProperty<V, SymbolLayer, PropertyValue<IconTextFitType>, &SymbolLayer::setIconTextFit>;
    result["icon-text-fit-padding"] = &setLayoutProperty<V, SymbolLayer, PropertyValue<std::array<float, 4>>, &SymbolLayer::setIconTextFitPadding>;
    result["icon-image"] = &setLayoutProperty<V, SymbolLayer, DataDrivenPropertyValue<std::string>, &SymbolLayer::setIconImage>;
    result["icon-rotate"] = &setLayoutProperty<V, SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setIconRotate>;
    result["icon-padding"] = &setLayoutProperty<V, SymbolLayer, PropertyValue<float>, &SymbolLayer::setIconPadding>;
    result["icon-keep-upright"] = &setLayoutProperty<V, SymbolLayer, PropertyValue<bool>, &SymbolLayer::setIconKeepUpright>;
    result["icon-offset"] = &setLayoutProperty<V, SymbolLayer, DataDrivenPropertyValue<std::array<float, 2>>, &SymbolLayer::setIconOffset>;
    result["text-pitch-alignment"] = &setLayoutProperty<V, SymbolLayer, PropertyValue<AlignmentType>, &SymbolLayer::setTextPitchAlignment>;
    result["text-rotation-alignment"] = &setLayoutProperty<V, SymbolLayer, PropertyValue<AlignmentType>, &SymbolLayer::setTextRotationAlignment>;
    result["text-field"] = &setLayoutProperty<V, SymbolLayer, DataDrivenPropertyValue<std::string>, &SymbolLayer::setTextField>;
    result["text-font"] = &setLayoutProperty<V, SymbolLayer, PropertyValue<std::vector<std::string>>, &SymbolLayer::setTextFont>;
    result["text-size"] = &setLayoutProperty<V, SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setTextSize>;
    result["text-max-width"] = &setLayoutProperty<V, SymbolLayer, PropertyValue<float>, &SymbolLayer::setTextMaxWidth>;
    result["text-line-height"] = &setLayoutProperty<V, SymbolLayer, PropertyValue<float>, &SymbolLayer::setTextLineHeight>;
    result["text-letter-spacing"] = &setLayoutProperty<V, SymbolLayer, PropertyValue<float>, &SymbolLayer::setTextLetterSpacing>;
    result["text-justify"] = &setLayoutProperty<V, SymbolLayer, PropertyValue<TextJustifyType>, &SymbolLayer::setTextJustify>;
    result["text-anchor"] = &setLayoutProperty<V, SymbolLayer, PropertyValue<TextAnchorType>, &SymbolLayer::setTextAnchor>;
    result["text-max-angle"] = &setLayoutProperty<V, SymbolLayer, PropertyValue<float>, &SymbolLayer::setTextMaxAngle>;
    result["text-rotate"] = &setLayoutProperty<V, SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setTextRotate>;
    result["text-padding"] = &setLayoutProperty<V, SymbolLayer, PropertyValue<float>, &SymbolLayer::setTextPadding>;
    result["text-keep-upright"] = &setLayoutProperty<V, SymbolLayer, PropertyValue<bool>, &SymbolLayer::setTextKeepUpright>;
    result["text-transform"] = &setLayoutProperty<V, SymbolLayer, DataDrivenPropertyValue<TextTransformType>, &SymbolLayer::setTextTransform>;
    result["text-offset"] = &setLayoutProperty<V, SymbolLayer, DataDrivenPropertyValue<std::array<float, 2>>, &SymbolLayer::setTextOffset>;
    result["text-allow-overlap"] = &setLayoutProperty<V, SymbolLayer, PropertyValue<bool>, &SymbolLayer::setTextAllowOverlap>;
    result["text-ignore-placement"] = &setLayoutProperty<V, SymbolLayer, PropertyValue<bool>, &SymbolLayer::setTextIgnorePlacement>;
    result["text-optional"] = &setLayoutProperty<V, SymbolLayer, PropertyValue<bool>, &SymbolLayer::setTextOptional>;





    return result;
}

template <class V>
auto makePaintPropertySetters() {
    std::unordered_map<std::string, PaintPropertySetter<V>> result;

    result["fill-antialias"] = &setPaintProperty<V, FillLayer, PropertyValue<bool>, &FillLayer::setFillAntialias>;
    result["fill-antialias-transition"] = &setTransition<V, FillLayer, &FillLayer::setFillAntialiasTransition>;
    result["fill-opacity"] = &setPaintProperty<V, FillLayer, DataDrivenPropertyValue<float>, &FillLayer::setFillOpacity>;
    result["fill-opacity-transition"] = &setTransition<V, FillLayer, &FillLayer::setFillOpacityTransition>;
    result["fill-color"] = &setPaintProperty<V, FillLayer, DataDrivenPropertyValue<Color>, &FillLayer::setFillColor>;
    result["fill-color-transition"] = &setTransition<V, FillLayer, &FillLayer::setFillColorTransition>;
    result["fill-outline-color"] = &setPaintProperty<V, FillLayer, DataDrivenPropertyValue<Color>, &FillLayer::setFillOutlineColor>;
    result["fill-outline-color-transition"] = &setTransition<V, FillLayer, &FillLayer::setFillOutlineColorTransition>;
    result["fill-translate"] = &setPaintProperty<V, FillLayer, PropertyValue<std::array<float, 2>>, &FillLayer::setFillTranslate>;
    result["fill-translate-transition"] = &setTransition<V, FillLayer, &FillLayer::setFillTranslateTransition>;
    result["fill-translate-anchor"] = &setPaintProperty<V, FillLayer, PropertyValue<TranslateAnchorType>, &FillLayer::setFillTranslateAnchor>;
    result["fill-translate-anchor-transition"] = &setTransition<V, FillLayer, &FillLayer::setFillTranslateAnchorTransition>;
    result["fill-pattern"] = &setPaintProperty<V, FillLayer, PropertyValue<std::string>, &FillLayer::setFillPattern>;
    result["fill-pattern-transition"] = &setTransition<V, FillLayer, &FillLayer::setFillPatternTransition>;

    result["line-opacity"] = &setPaintProperty<V, LineLayer, DataDrivenPropertyValue<float>, &LineLayer::setLineOpacity>;
    result["line-opacity-transition"] = &setTransition<V, LineLayer, &LineLayer::setLineOpacityTransition>;
    result["line-color"] = &setPaintProperty<V, LineLayer, DataDrivenPropertyValue<Color>, &LineLayer::setLineColor>;
    result["line-color-transition"] = &setTransition<V, LineLayer, &LineLayer::setLineColorTransition>;
    result["line-translate"] = &setPaintProperty<V, LineLayer, PropertyValue<std::array<float, 2>>, &LineLayer::setLineTranslate>;
    result["line-translate-transition"] = &setTransition<V, LineLayer, &LineLayer::setLineTranslateTransition>;
    result["line-translate-anchor"] = &setPaintProperty<V, LineLayer, PropertyValue<TranslateAnchorType>, &LineLayer::setLineTranslateAnchor>;
    result["line-translate-anchor-transition"] = &setTransition<V, LineLayer, &LineLayer::setLineTranslateAnchorTransition>;
    result["line-width"] = &setPaintProperty<V, LineLayer, PropertyValue<float>, &LineLayer::setLineWidth>;
    result["line-width-transition"] = &setTransition<V, LineLayer, &LineLayer::setLineWidthTransition>;
    result["line-gap-width"] = &setPaintProperty<V, LineLayer, DataDrivenPropertyValue<float>, &LineLayer::setLineGapWidth>;
    result["line-gap-width-transition"] = &setTransition<V, LineLayer, &LineLayer::setLineGapWidthTransition>;
    result["line-offset"] = &setPaintProperty<V, LineLayer, DataDrivenPropertyValue<float>, &LineLayer::setLineOffset>;
    result["line-offset-transition"] = &setTransition<V, LineLayer, &LineLayer::setLineOffsetTransition>;
    result["line-blur"] = &setPaintProperty<V, LineLayer, DataDrivenPropertyValue<float>, &LineLayer::setLineBlur>;
    result["line-blur-transition"] = &setTransition<V, LineLayer, &LineLayer::setLineBlurTransition>;
    result["line-dasharray"] = &setPaintProperty<V, LineLayer, PropertyValue<std::vector<float>>, &LineLayer::setLineDasharray>;
    result["line-dasharray-transition"] = &setTransition<V, LineLayer, &LineLayer::setLineDasharrayTransition>;
    result["line-pattern"] = &setPaintProperty<V, LineLayer, PropertyValue<std::string>, &LineLayer::setLinePattern>;
    result["line-pattern-transition"] = &setTransition<V, LineLayer, &LineLayer::setLinePatternTransition>;

    result["icon-opacity"] = &setPaintProperty<V, SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setIconOpacity>;
    result["icon-opacity-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setIconOpacityTransition>;
    result["icon-color"] = &setPaintProperty<V, SymbolLayer, DataDrivenPropertyValue<Color>, &SymbolLayer::setIconColor>;
    result["icon-color-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setIconColorTransition>;
    result["icon-halo-color"] = &setPaintProperty<V, SymbolLayer, DataDrivenPropertyValue<Color>, &SymbolLayer::setIconHaloColor>;
    result["icon-halo-color-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setIconHaloColorTransition>;
    result["icon-halo-width"] = &setPaintProperty<V, SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setIconHaloWidth>;
    result["icon-halo-width-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setIconHaloWidthTransition>;
    result["icon-halo-blur"] = &setPaintProperty<V, SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setIconHaloBlur>;
    result["icon-halo-blur-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setIconHaloBlurTransition>;
    result["icon-translate"] = &setPaintProperty<V, SymbolLayer, PropertyValue<std::array<float, 2>>, &SymbolLayer::setIconTranslate>;
    result["icon-translate-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setIconTranslateTransition>;
    result["icon-translate-anchor"] = &setPaintProperty<V, SymbolLayer, PropertyValue<TranslateAnchorType>, &SymbolLayer::setIconTranslateAnchor>;
    result["icon-translate-anchor-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setIconTranslateAnchorTransition>;
    result["text-opacity"] = &setPaintProperty<V, SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setTextOpacity>;
    result["text-opacity-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setTextOpacityTransition>;
    result["text-color"] = &setPaintProperty<V, SymbolLayer, DataDrivenPropertyValue<Color>, &SymbolLayer::setTextColor>;
    result["text-color-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setTextColorTransition>;
    result["text-halo-color"] = &setPaintProperty<V, SymbolLayer, DataDrivenPropertyValue<Color>, &SymbolLayer::setTextHaloColor>;
    result["text-halo-color-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setTextHaloColorTransition>;
    result["text-halo-width"] = &setPaintProperty<V, SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setTextHaloWidth>;
    result["text-halo-width-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setTextHaloWidthTransition>;
    result["text-halo-blur"] = &setPaintProperty<V, SymbolLayer, DataDrivenPropertyValue<float>, &SymbolLayer::setTextHaloBlur>;
    result["text-halo-blur-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setTextHaloBlurTransition>;
    result["text-translate"] = &setPaintProperty<V, SymbolLayer, PropertyValue<std::array<float, 2>>, &SymbolLayer::setTextTranslate>;
    result["text-translate-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setTextTranslateTransition>;
    result["text-translate-anchor"] = &setPaintProperty<V, SymbolLayer, PropertyValue<TranslateAnchorType>, &SymbolLayer::setTextTranslateAnchor>;
    result["text-translate-anchor-transition"] = &setTransition<V, SymbolLayer, &SymbolLayer::setTextTranslateAnchorTransition>;

    result["circle-radius"] = &setPaintProperty<V, CircleLayer, DataDrivenPropertyValue<float>, &CircleLayer::setCircleRadius>;
    result["circle-radius-transition"] = &setTransition<V, CircleLayer, &CircleLayer::setCircleRadiusTransition>;
    result["circle-color"] = &setPaintProperty<V, CircleLayer, DataDrivenPropertyValue<Color>, &CircleLayer::setCircleColor>;
    result["circle-color-transition"] = &setTransition<V, CircleLayer, &CircleLayer::setCircleColorTransition>;
    result["circle-blur"] = &setPaintProperty<V, CircleLayer, DataDrivenPropertyValue<float>, &CircleLayer::setCircleBlur>;
    result["circle-blur-transition"] = &setTransition<V, CircleLayer, &CircleLayer::setCircleBlurTransition>;
    result["circle-opacity"] = &setPaintProperty<V, CircleLayer, DataDrivenPropertyValue<float>, &CircleLayer::setCircleOpacity>;
    result["circle-opacity-transition"] = &setTransition<V, CircleLayer, &CircleLayer::setCircleOpacityTransition>;
    result["circle-translate"] = &setPaintProperty<V, CircleLayer, PropertyValue<std::array<float, 2>>, &CircleLayer::setCircleTranslate>;
    result["circle-translate-transition"] = &setTransition<V, CircleLayer, &CircleLayer::setCircleTranslateTransition>;
    result["circle-translate-anchor"] = &setPaintProperty<V, CircleLayer, PropertyValue<TranslateAnchorType>, &CircleLayer::setCircleTranslateAnchor>;
    result["circle-translate-anchor-transition"] = &setTransition<V, CircleLayer, &CircleLayer::setCircleTranslateAnchorTransition>;
    result["circle-pitch-scale"] = &setPaintProperty<V, CircleLayer, PropertyValue<CirclePitchScaleType>, &CircleLayer::setCirclePitchScale>;
    result["circle-pitch-scale-transition"] = &setTransition<V, CircleLayer, &CircleLayer::setCirclePitchScaleTransition>;
    result["circle-stroke-width"] = &setPaintProperty<V, CircleLayer, DataDrivenPropertyValue<float>, &CircleLayer::setCircleStrokeWidth>;
    result["circle-stroke-width-transition"] = &setTransition<V, CircleLayer, &CircleLayer::setCircleStrokeWidthTransition>;
    result["circle-stroke-color"] = &setPaintProperty<V, CircleLayer, DataDrivenPropertyValue<Color>, &CircleLayer::setCircleStrokeColor>;
    result["circle-stroke-color-transition"] = &setTransition<V, CircleLayer, &CircleLayer::setCircleStrokeColorTransition>;
    result["circle-stroke-opacity"] = &setPaintProperty<V, CircleLayer, DataDrivenPropertyValue<float>, &CircleLayer::setCircleStrokeOpacity>;
    result["circle-stroke-opacity-transition"] = &setTransition<V, CircleLayer, &CircleLayer::setCircleStrokeOpacityTransition>;

    result["fill-extrusion-opacity"] = &setPaintProperty<V, FillExtrusionLayer, PropertyValue<float>, &FillExtrusionLayer::setFillExtrusionOpacity>;
    result["fill-extrusion-opacity-transition"] = &setTransition<V, FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionOpacityTransition>;
    result["fill-extrusion-color"] = &setPaintProperty<V, FillExtrusionLayer, DataDrivenPropertyValue<Color>, &FillExtrusionLayer::setFillExtrusionColor>;
    result["fill-extrusion-color-transition"] = &setTransition<V, FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionColorTransition>;
    result["fill-extrusion-translate"] = &setPaintProperty<V, FillExtrusionLayer, PropertyValue<std::array<float, 2>>, &FillExtrusionLayer::setFillExtrusionTranslate>;
    result["fill-extrusion-translate-transition"] = &setTransition<V, FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionTranslateTransition>;
    result["fill-extrusion-translate-anchor"] = &setPaintProperty<V, FillExtrusionLayer, PropertyValue<TranslateAnchorType>, &FillExtrusionLayer::setFillExtrusionTranslateAnchor>;
    result["fill-extrusion-translate-anchor-transition"] = &setTransition<V, FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionTranslateAnchorTransition>;
    result["fill-extrusion-pattern"] = &setPaintProperty<V, FillExtrusionLayer, PropertyValue<std::string>, &FillExtrusionLayer::setFillExtrusionPattern>;
    result["fill-extrusion-pattern-transition"] = &setTransition<V, FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionPatternTransition>;
    result["fill-extrusion-height"] = &setPaintProperty<V, FillExtrusionLayer, DataDrivenPropertyValue<float>, &FillExtrusionLayer::setFillExtrusionHeight>;
    result["fill-extrusion-height-transition"] = &setTransition<V, FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionHeightTransition>;
    result["fill-extrusion-base"] = &setPaintProperty<V, FillExtrusionLayer, DataDrivenPropertyValue<float>, &FillExtrusionLayer::setFillExtrusionBase>;
    result["fill-extrusion-base-transition"] = &setTransition<V, FillExtrusionLayer, &FillExtrusionLayer::setFillExtrusionBaseTransition>;

    result["raster-opacity"] = &setPaintProperty<V, RasterLayer, PropertyValue<float>, &RasterLayer::setRasterOpacity>;
    result["raster-opacity-transition"] = &setTransition<V, RasterLayer, &RasterLayer::setRasterOpacityTransition>;
    result["raster-hue-rotate"] = &setPaintProperty<V, RasterLayer, PropertyValue<float>, &RasterLayer::setRasterHueRotate>;
    result["raster-hue-rotate-transition"] = &setTransition<V, RasterLayer, &RasterLayer::setRasterHueRotateTransition>;
    result["raster-brightness-min"] = &setPaintProperty<V, RasterLayer, PropertyValue<float>, &RasterLayer::setRasterBrightnessMin>;
    result["raster-brightness-min-transition"] = &setTransition<V, RasterLayer, &RasterLayer::setRasterBrightnessMinTransition>;
    result["raster-brightness-max"] = &setPaintProperty<V, RasterLayer, PropertyValue<float>, &RasterLayer::setRasterBrightnessMax>;
    result["raster-brightness-max-transition"] = &setTransition<V, RasterLayer, &RasterLayer::setRasterBrightnessMaxTransition>;
    result["raster-saturation"] = &setPaintProperty<V, RasterLayer, PropertyValue<float>, &RasterLayer::setRasterSaturation>;
    result["raster-saturation-transition"] = &setTransition<V, RasterLayer, &RasterLayer::setRasterSaturationTransition>;
    result["raster-contrast"] = &setPaintProperty<V, RasterLayer, PropertyValue<float>, &RasterLayer::setRasterContrast>;
    result["raster-contrast-transition"] = &setTransition<V, RasterLayer, &RasterLayer::setRasterContrastTransition>;
    result["raster-fade-duration"] = &setPaintProperty<V, RasterLayer, PropertyValue<float>, &RasterLayer::setRasterFadeDuration>;
    result["raster-fade-duration-transition"] = &setTransition<V, RasterLayer, &RasterLayer::setRasterFadeDurationTransition>;

    result["background-color"] = &setPaintProperty<V, BackgroundLayer, PropertyValue<Color>, &BackgroundLayer::setBackgroundColor>;
    result["background-color-transition"] = &setTransition<V, BackgroundLayer, &BackgroundLayer::setBackgroundColorTransition>;
    result["background-pattern"] = &setPaintProperty<V, BackgroundLayer, PropertyValue<std::string>, &BackgroundLayer::setBackgroundPattern>;
    result["background-pattern-transition"] = &setTransition<V, BackgroundLayer, &BackgroundLayer::setBackgroundPatternTransition>;
    result["background-opacity"] = &setPaintProperty<V, BackgroundLayer, PropertyValue<float>, &BackgroundLayer::setBackgroundOpacity>;
    result["background-opacity-transition"] = &setTransition<V, BackgroundLayer, &BackgroundLayer::setBackgroundOpacityTransition>;

    return result;
}

} // namespace conversion
} // namespace style
} // namespace mbgl
