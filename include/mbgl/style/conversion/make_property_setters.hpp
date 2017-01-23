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
    result["fill-opacity"] = makePropertySetter<V>(&FillLayer::setFillOpacity);
    result["fill-color"] = makePropertySetter<V>(&FillLayer::setFillColor);
    result["fill-outline-color"] = makePropertySetter<V>(&FillLayer::setFillOutlineColor);
    result["fill-translate"] = makePropertySetter<V>(&FillLayer::setFillTranslate);
    result["fill-translate-anchor"] = makePropertySetter<V>(&FillLayer::setFillTranslateAnchor);
    result["fill-pattern"] = makePropertySetter<V>(&FillLayer::setFillPattern);

    result["line-opacity"] = makePropertySetter<V>(&LineLayer::setLineOpacity);
    result["line-color"] = makePropertySetter<V>(&LineLayer::setLineColor);
    result["line-translate"] = makePropertySetter<V>(&LineLayer::setLineTranslate);
    result["line-translate-anchor"] = makePropertySetter<V>(&LineLayer::setLineTranslateAnchor);
    result["line-width"] = makePropertySetter<V>(&LineLayer::setLineWidth);
    result["line-gap-width"] = makePropertySetter<V>(&LineLayer::setLineGapWidth);
    result["line-offset"] = makePropertySetter<V>(&LineLayer::setLineOffset);
    result["line-blur"] = makePropertySetter<V>(&LineLayer::setLineBlur);
    result["line-dasharray"] = makePropertySetter<V>(&LineLayer::setLineDasharray);
    result["line-pattern"] = makePropertySetter<V>(&LineLayer::setLinePattern);

    result["icon-opacity"] = makePropertySetter<V>(&SymbolLayer::setIconOpacity);
    result["icon-color"] = makePropertySetter<V>(&SymbolLayer::setIconColor);
    result["icon-halo-color"] = makePropertySetter<V>(&SymbolLayer::setIconHaloColor);
    result["icon-halo-width"] = makePropertySetter<V>(&SymbolLayer::setIconHaloWidth);
    result["icon-halo-blur"] = makePropertySetter<V>(&SymbolLayer::setIconHaloBlur);
    result["icon-translate"] = makePropertySetter<V>(&SymbolLayer::setIconTranslate);
    result["icon-translate-anchor"] = makePropertySetter<V>(&SymbolLayer::setIconTranslateAnchor);
    result["text-opacity"] = makePropertySetter<V>(&SymbolLayer::setTextOpacity);
    result["text-color"] = makePropertySetter<V>(&SymbolLayer::setTextColor);
    result["text-halo-color"] = makePropertySetter<V>(&SymbolLayer::setTextHaloColor);
    result["text-halo-width"] = makePropertySetter<V>(&SymbolLayer::setTextHaloWidth);
    result["text-halo-blur"] = makePropertySetter<V>(&SymbolLayer::setTextHaloBlur);
    result["text-translate"] = makePropertySetter<V>(&SymbolLayer::setTextTranslate);
    result["text-translate-anchor"] = makePropertySetter<V>(&SymbolLayer::setTextTranslateAnchor);

    result["circle-radius"] = makePropertySetter<V>(&CircleLayer::setCircleRadius);
    result["circle-color"] = makePropertySetter<V>(&CircleLayer::setCircleColor);
    result["circle-blur"] = makePropertySetter<V>(&CircleLayer::setCircleBlur);
    result["circle-opacity"] = makePropertySetter<V>(&CircleLayer::setCircleOpacity);
    result["circle-translate"] = makePropertySetter<V>(&CircleLayer::setCircleTranslate);
    result["circle-translate-anchor"] = makePropertySetter<V>(&CircleLayer::setCircleTranslateAnchor);
    result["circle-pitch-scale"] = makePropertySetter<V>(&CircleLayer::setCirclePitchScale);
    result["circle-stroke-width"] = makePropertySetter<V>(&CircleLayer::setCircleStrokeWidth);
    result["circle-stroke-color"] = makePropertySetter<V>(&CircleLayer::setCircleStrokeColor);
    result["circle-stroke-opacity"] = makePropertySetter<V>(&CircleLayer::setCircleStrokeOpacity);

    result["fill-extrusion-opacity"] = makePropertySetter<V>(&FillExtrusionLayer::setFillExtrusionOpacity);
    result["fill-extrusion-color"] = makePropertySetter<V>(&FillExtrusionLayer::setFillExtrusionColor);
    result["fill-extrusion-translate"] = makePropertySetter<V>(&FillExtrusionLayer::setFillExtrusionTranslate);
    result["fill-extrusion-translate-anchor"] = makePropertySetter<V>(&FillExtrusionLayer::setFillExtrusionTranslateAnchor);
    result["fill-extrusion-pattern"] = makePropertySetter<V>(&FillExtrusionLayer::setFillExtrusionPattern);
    result["fill-extrusion-height"] = makePropertySetter<V>(&FillExtrusionLayer::setFillExtrusionHeight);
    result["fill-extrusion-base"] = makePropertySetter<V>(&FillExtrusionLayer::setFillExtrusionBase);

    result["raster-opacity"] = makePropertySetter<V>(&RasterLayer::setRasterOpacity);
    result["raster-hue-rotate"] = makePropertySetter<V>(&RasterLayer::setRasterHueRotate);
    result["raster-brightness-min"] = makePropertySetter<V>(&RasterLayer::setRasterBrightnessMin);
    result["raster-brightness-max"] = makePropertySetter<V>(&RasterLayer::setRasterBrightnessMax);
    result["raster-saturation"] = makePropertySetter<V>(&RasterLayer::setRasterSaturation);
    result["raster-contrast"] = makePropertySetter<V>(&RasterLayer::setRasterContrast);
    result["raster-fade-duration"] = makePropertySetter<V>(&RasterLayer::setRasterFadeDuration);

    result["background-color"] = makePropertySetter<V>(&BackgroundLayer::setBackgroundColor);
    result["background-pattern"] = makePropertySetter<V>(&BackgroundLayer::setBackgroundPattern);
    result["background-opacity"] = makePropertySetter<V>(&BackgroundLayer::setBackgroundOpacity);

    return result;
}

} // namespace conversion
} // namespace style
} // namespace mbgl
