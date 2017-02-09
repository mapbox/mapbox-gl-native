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


    result["line-cap"] = makePropertySetter<V>(&LineLayer::setLineCap);
    result["line-join"] = makePropertySetter<V>(&LineLayer::setLineJoin);
    result["line-miter-limit"] = makePropertySetter<V>(&LineLayer::setLineMiterLimit);
    result["line-round-limit"] = makePropertySetter<V>(&LineLayer::setLineRoundLimit);

    result["symbol-placement"] = makePropertySetter<V>(&SymbolLayer::setSymbolPlacement);
    result["symbol-spacing"] = makePropertySetter<V>(&SymbolLayer::setSymbolSpacing);
    result["symbol-avoid-edges"] = makePropertySetter<V>(&SymbolLayer::setSymbolAvoidEdges);
    result["icon-allow-overlap"] = makePropertySetter<V>(&SymbolLayer::setIconAllowOverlap);
    result["icon-ignore-placement"] = makePropertySetter<V>(&SymbolLayer::setIconIgnorePlacement);
    result["icon-optional"] = makePropertySetter<V>(&SymbolLayer::setIconOptional);
    result["icon-rotation-alignment"] = makePropertySetter<V>(&SymbolLayer::setIconRotationAlignment);
    result["icon-size"] = makePropertySetter<V>(&SymbolLayer::setIconSize);
    result["icon-text-fit"] = makePropertySetter<V>(&SymbolLayer::setIconTextFit);
    result["icon-text-fit-padding"] = makePropertySetter<V>(&SymbolLayer::setIconTextFitPadding);
    result["icon-image"] = makePropertySetter<V>(&SymbolLayer::setIconImage);
    result["icon-rotate"] = makePropertySetter<V>(&SymbolLayer::setIconRotate);
    result["icon-padding"] = makePropertySetter<V>(&SymbolLayer::setIconPadding);
    result["icon-keep-upright"] = makePropertySetter<V>(&SymbolLayer::setIconKeepUpright);
    result["icon-offset"] = makePropertySetter<V>(&SymbolLayer::setIconOffset);
    result["text-pitch-alignment"] = makePropertySetter<V>(&SymbolLayer::setTextPitchAlignment);
    result["text-rotation-alignment"] = makePropertySetter<V>(&SymbolLayer::setTextRotationAlignment);
    result["text-field"] = makePropertySetter<V>(&SymbolLayer::setTextField);
    result["text-font"] = makePropertySetter<V>(&SymbolLayer::setTextFont);
    result["text-size"] = makePropertySetter<V>(&SymbolLayer::setTextSize);
    result["text-max-width"] = makePropertySetter<V>(&SymbolLayer::setTextMaxWidth);
    result["text-line-height"] = makePropertySetter<V>(&SymbolLayer::setTextLineHeight);
    result["text-letter-spacing"] = makePropertySetter<V>(&SymbolLayer::setTextLetterSpacing);
    result["text-justify"] = makePropertySetter<V>(&SymbolLayer::setTextJustify);
    result["text-anchor"] = makePropertySetter<V>(&SymbolLayer::setTextAnchor);
    result["text-max-angle"] = makePropertySetter<V>(&SymbolLayer::setTextMaxAngle);
    result["text-rotate"] = makePropertySetter<V>(&SymbolLayer::setTextRotate);
    result["text-padding"] = makePropertySetter<V>(&SymbolLayer::setTextPadding);
    result["text-keep-upright"] = makePropertySetter<V>(&SymbolLayer::setTextKeepUpright);
    result["text-transform"] = makePropertySetter<V>(&SymbolLayer::setTextTransform);
    result["text-offset"] = makePropertySetter<V>(&SymbolLayer::setTextOffset);
    result["text-allow-overlap"] = makePropertySetter<V>(&SymbolLayer::setTextAllowOverlap);
    result["text-ignore-placement"] = makePropertySetter<V>(&SymbolLayer::setTextIgnorePlacement);
    result["text-optional"] = makePropertySetter<V>(&SymbolLayer::setTextOptional);





    return result;
}

template <class V>
auto makePaintPropertySetters() {
    std::unordered_map<std::string, PaintPropertySetter<V>> result;

    result["fill-antialias"] = makePropertySetter<V>(&FillLayer::setFillAntialias);
    result["fill-antialias-transition"] = makeTransitionSetter<V>(&FillLayer::setFillAntialiasTransition);
    result["fill-opacity"] = makePropertySetter<V>(&FillLayer::setFillOpacity);
    result["fill-opacity-transition"] = makeTransitionSetter<V>(&FillLayer::setFillOpacityTransition);
    result["fill-color"] = makePropertySetter<V>(&FillLayer::setFillColor);
    result["fill-color-transition"] = makeTransitionSetter<V>(&FillLayer::setFillColorTransition);
    result["fill-outline-color"] = makePropertySetter<V>(&FillLayer::setFillOutlineColor);
    result["fill-outline-color-transition"] = makeTransitionSetter<V>(&FillLayer::setFillOutlineColorTransition);
    result["fill-translate"] = makePropertySetter<V>(&FillLayer::setFillTranslate);
    result["fill-translate-transition"] = makeTransitionSetter<V>(&FillLayer::setFillTranslateTransition);
    result["fill-translate-anchor"] = makePropertySetter<V>(&FillLayer::setFillTranslateAnchor);
    result["fill-translate-anchor-transition"] = makeTransitionSetter<V>(&FillLayer::setFillTranslateAnchorTransition);
    result["fill-pattern"] = makePropertySetter<V>(&FillLayer::setFillPattern);
    result["fill-pattern-transition"] = makeTransitionSetter<V>(&FillLayer::setFillPatternTransition);

    result["line-opacity"] = makePropertySetter<V>(&LineLayer::setLineOpacity);
    result["line-opacity-transition"] = makeTransitionSetter<V>(&LineLayer::setLineOpacityTransition);
    result["line-color"] = makePropertySetter<V>(&LineLayer::setLineColor);
    result["line-color-transition"] = makeTransitionSetter<V>(&LineLayer::setLineColorTransition);
    result["line-translate"] = makePropertySetter<V>(&LineLayer::setLineTranslate);
    result["line-translate-transition"] = makeTransitionSetter<V>(&LineLayer::setLineTranslateTransition);
    result["line-translate-anchor"] = makePropertySetter<V>(&LineLayer::setLineTranslateAnchor);
    result["line-translate-anchor-transition"] = makeTransitionSetter<V>(&LineLayer::setLineTranslateAnchorTransition);
    result["line-width"] = makePropertySetter<V>(&LineLayer::setLineWidth);
    result["line-width-transition"] = makeTransitionSetter<V>(&LineLayer::setLineWidthTransition);
    result["line-gap-width"] = makePropertySetter<V>(&LineLayer::setLineGapWidth);
    result["line-gap-width-transition"] = makeTransitionSetter<V>(&LineLayer::setLineGapWidthTransition);
    result["line-offset"] = makePropertySetter<V>(&LineLayer::setLineOffset);
    result["line-offset-transition"] = makeTransitionSetter<V>(&LineLayer::setLineOffsetTransition);
    result["line-blur"] = makePropertySetter<V>(&LineLayer::setLineBlur);
    result["line-blur-transition"] = makeTransitionSetter<V>(&LineLayer::setLineBlurTransition);
    result["line-dasharray"] = makePropertySetter<V>(&LineLayer::setLineDasharray);
    result["line-dasharray-transition"] = makeTransitionSetter<V>(&LineLayer::setLineDasharrayTransition);
    result["line-pattern"] = makePropertySetter<V>(&LineLayer::setLinePattern);
    result["line-pattern-transition"] = makeTransitionSetter<V>(&LineLayer::setLinePatternTransition);

    result["icon-opacity"] = makePropertySetter<V>(&SymbolLayer::setIconOpacity);
    result["icon-opacity-transition"] = makeTransitionSetter<V>(&SymbolLayer::setIconOpacityTransition);
    result["icon-color"] = makePropertySetter<V>(&SymbolLayer::setIconColor);
    result["icon-color-transition"] = makeTransitionSetter<V>(&SymbolLayer::setIconColorTransition);
    result["icon-halo-color"] = makePropertySetter<V>(&SymbolLayer::setIconHaloColor);
    result["icon-halo-color-transition"] = makeTransitionSetter<V>(&SymbolLayer::setIconHaloColorTransition);
    result["icon-halo-width"] = makePropertySetter<V>(&SymbolLayer::setIconHaloWidth);
    result["icon-halo-width-transition"] = makeTransitionSetter<V>(&SymbolLayer::setIconHaloWidthTransition);
    result["icon-halo-blur"] = makePropertySetter<V>(&SymbolLayer::setIconHaloBlur);
    result["icon-halo-blur-transition"] = makeTransitionSetter<V>(&SymbolLayer::setIconHaloBlurTransition);
    result["icon-translate"] = makePropertySetter<V>(&SymbolLayer::setIconTranslate);
    result["icon-translate-transition"] = makeTransitionSetter<V>(&SymbolLayer::setIconTranslateTransition);
    result["icon-translate-anchor"] = makePropertySetter<V>(&SymbolLayer::setIconTranslateAnchor);
    result["icon-translate-anchor-transition"] = makeTransitionSetter<V>(&SymbolLayer::setIconTranslateAnchorTransition);
    result["text-opacity"] = makePropertySetter<V>(&SymbolLayer::setTextOpacity);
    result["text-opacity-transition"] = makeTransitionSetter<V>(&SymbolLayer::setTextOpacityTransition);
    result["text-color"] = makePropertySetter<V>(&SymbolLayer::setTextColor);
    result["text-color-transition"] = makeTransitionSetter<V>(&SymbolLayer::setTextColorTransition);
    result["text-halo-color"] = makePropertySetter<V>(&SymbolLayer::setTextHaloColor);
    result["text-halo-color-transition"] = makeTransitionSetter<V>(&SymbolLayer::setTextHaloColorTransition);
    result["text-halo-width"] = makePropertySetter<V>(&SymbolLayer::setTextHaloWidth);
    result["text-halo-width-transition"] = makeTransitionSetter<V>(&SymbolLayer::setTextHaloWidthTransition);
    result["text-halo-blur"] = makePropertySetter<V>(&SymbolLayer::setTextHaloBlur);
    result["text-halo-blur-transition"] = makeTransitionSetter<V>(&SymbolLayer::setTextHaloBlurTransition);
    result["text-translate"] = makePropertySetter<V>(&SymbolLayer::setTextTranslate);
    result["text-translate-transition"] = makeTransitionSetter<V>(&SymbolLayer::setTextTranslateTransition);
    result["text-translate-anchor"] = makePropertySetter<V>(&SymbolLayer::setTextTranslateAnchor);
    result["text-translate-anchor-transition"] = makeTransitionSetter<V>(&SymbolLayer::setTextTranslateAnchorTransition);

    result["circle-radius"] = makePropertySetter<V>(&CircleLayer::setCircleRadius);
    result["circle-radius-transition"] = makeTransitionSetter<V>(&CircleLayer::setCircleRadiusTransition);
    result["circle-color"] = makePropertySetter<V>(&CircleLayer::setCircleColor);
    result["circle-color-transition"] = makeTransitionSetter<V>(&CircleLayer::setCircleColorTransition);
    result["circle-blur"] = makePropertySetter<V>(&CircleLayer::setCircleBlur);
    result["circle-blur-transition"] = makeTransitionSetter<V>(&CircleLayer::setCircleBlurTransition);
    result["circle-opacity"] = makePropertySetter<V>(&CircleLayer::setCircleOpacity);
    result["circle-opacity-transition"] = makeTransitionSetter<V>(&CircleLayer::setCircleOpacityTransition);
    result["circle-translate"] = makePropertySetter<V>(&CircleLayer::setCircleTranslate);
    result["circle-translate-transition"] = makeTransitionSetter<V>(&CircleLayer::setCircleTranslateTransition);
    result["circle-translate-anchor"] = makePropertySetter<V>(&CircleLayer::setCircleTranslateAnchor);
    result["circle-translate-anchor-transition"] = makeTransitionSetter<V>(&CircleLayer::setCircleTranslateAnchorTransition);
    result["circle-pitch-scale"] = makePropertySetter<V>(&CircleLayer::setCirclePitchScale);
    result["circle-pitch-scale-transition"] = makeTransitionSetter<V>(&CircleLayer::setCirclePitchScaleTransition);
    result["circle-stroke-width"] = makePropertySetter<V>(&CircleLayer::setCircleStrokeWidth);
    result["circle-stroke-width-transition"] = makeTransitionSetter<V>(&CircleLayer::setCircleStrokeWidthTransition);
    result["circle-stroke-color"] = makePropertySetter<V>(&CircleLayer::setCircleStrokeColor);
    result["circle-stroke-color-transition"] = makeTransitionSetter<V>(&CircleLayer::setCircleStrokeColorTransition);
    result["circle-stroke-opacity"] = makePropertySetter<V>(&CircleLayer::setCircleStrokeOpacity);
    result["circle-stroke-opacity-transition"] = makeTransitionSetter<V>(&CircleLayer::setCircleStrokeOpacityTransition);

    result["fill-extrusion-opacity"] = makePropertySetter<V>(&FillExtrusionLayer::setFillExtrusionOpacity);
    result["fill-extrusion-opacity-transition"] = makeTransitionSetter<V>(&FillExtrusionLayer::setFillExtrusionOpacityTransition);
    result["fill-extrusion-color"] = makePropertySetter<V>(&FillExtrusionLayer::setFillExtrusionColor);
    result["fill-extrusion-color-transition"] = makeTransitionSetter<V>(&FillExtrusionLayer::setFillExtrusionColorTransition);
    result["fill-extrusion-translate"] = makePropertySetter<V>(&FillExtrusionLayer::setFillExtrusionTranslate);
    result["fill-extrusion-translate-transition"] = makeTransitionSetter<V>(&FillExtrusionLayer::setFillExtrusionTranslateTransition);
    result["fill-extrusion-translate-anchor"] = makePropertySetter<V>(&FillExtrusionLayer::setFillExtrusionTranslateAnchor);
    result["fill-extrusion-translate-anchor-transition"] = makeTransitionSetter<V>(&FillExtrusionLayer::setFillExtrusionTranslateAnchorTransition);
    result["fill-extrusion-pattern"] = makePropertySetter<V>(&FillExtrusionLayer::setFillExtrusionPattern);
    result["fill-extrusion-pattern-transition"] = makeTransitionSetter<V>(&FillExtrusionLayer::setFillExtrusionPatternTransition);
    result["fill-extrusion-height"] = makePropertySetter<V>(&FillExtrusionLayer::setFillExtrusionHeight);
    result["fill-extrusion-height-transition"] = makeTransitionSetter<V>(&FillExtrusionLayer::setFillExtrusionHeightTransition);
    result["fill-extrusion-base"] = makePropertySetter<V>(&FillExtrusionLayer::setFillExtrusionBase);
    result["fill-extrusion-base-transition"] = makeTransitionSetter<V>(&FillExtrusionLayer::setFillExtrusionBaseTransition);

    result["raster-opacity"] = makePropertySetter<V>(&RasterLayer::setRasterOpacity);
    result["raster-opacity-transition"] = makeTransitionSetter<V>(&RasterLayer::setRasterOpacityTransition);
    result["raster-hue-rotate"] = makePropertySetter<V>(&RasterLayer::setRasterHueRotate);
    result["raster-hue-rotate-transition"] = makeTransitionSetter<V>(&RasterLayer::setRasterHueRotateTransition);
    result["raster-brightness-min"] = makePropertySetter<V>(&RasterLayer::setRasterBrightnessMin);
    result["raster-brightness-min-transition"] = makeTransitionSetter<V>(&RasterLayer::setRasterBrightnessMinTransition);
    result["raster-brightness-max"] = makePropertySetter<V>(&RasterLayer::setRasterBrightnessMax);
    result["raster-brightness-max-transition"] = makeTransitionSetter<V>(&RasterLayer::setRasterBrightnessMaxTransition);
    result["raster-saturation"] = makePropertySetter<V>(&RasterLayer::setRasterSaturation);
    result["raster-saturation-transition"] = makeTransitionSetter<V>(&RasterLayer::setRasterSaturationTransition);
    result["raster-contrast"] = makePropertySetter<V>(&RasterLayer::setRasterContrast);
    result["raster-contrast-transition"] = makeTransitionSetter<V>(&RasterLayer::setRasterContrastTransition);
    result["raster-fade-duration"] = makePropertySetter<V>(&RasterLayer::setRasterFadeDuration);
    result["raster-fade-duration-transition"] = makeTransitionSetter<V>(&RasterLayer::setRasterFadeDurationTransition);

    result["background-color"] = makePropertySetter<V>(&BackgroundLayer::setBackgroundColor);
    result["background-color-transition"] = makeTransitionSetter<V>(&BackgroundLayer::setBackgroundColorTransition);
    result["background-pattern"] = makePropertySetter<V>(&BackgroundLayer::setBackgroundPattern);
    result["background-pattern-transition"] = makeTransitionSetter<V>(&BackgroundLayer::setBackgroundPatternTransition);
    result["background-opacity"] = makePropertySetter<V>(&BackgroundLayer::setBackgroundOpacity);
    result["background-opacity-transition"] = makeTransitionSetter<V>(&BackgroundLayer::setBackgroundOpacityTransition);

    return result;
}

} // namespace conversion
} // namespace style
} // namespace mbgl
