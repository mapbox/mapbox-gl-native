#include "node_style.hpp"

#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/background_layer.hpp>

namespace node_mbgl {

inline PropertySetters makeLayoutPropertySetters() {
    using namespace mbgl::style;
    PropertySetters result;


    result["line-cap"] = makePropertySetter(&LineLayer::setLineCap);
    result["line-join"] = makePropertySetter(&LineLayer::setLineJoin);
    result["line-miter-limit"] = makePropertySetter(&LineLayer::setLineMiterLimit);
    result["line-round-limit"] = makePropertySetter(&LineLayer::setLineRoundLimit);

    result["symbol-placement"] = makePropertySetter(&SymbolLayer::setSymbolPlacement);
    result["symbol-spacing"] = makePropertySetter(&SymbolLayer::setSymbolSpacing);
    result["symbol-avoid-edges"] = makePropertySetter(&SymbolLayer::setSymbolAvoidEdges);
    result["icon-allow-overlap"] = makePropertySetter(&SymbolLayer::setIconAllowOverlap);
    result["icon-ignore-placement"] = makePropertySetter(&SymbolLayer::setIconIgnorePlacement);
    result["icon-optional"] = makePropertySetter(&SymbolLayer::setIconOptional);
    result["icon-rotation-alignment"] = makePropertySetter(&SymbolLayer::setIconRotationAlignment);
    result["icon-size"] = makePropertySetter(&SymbolLayer::setIconSize);
    result["icon-text-fit"] = makePropertySetter(&SymbolLayer::setIconTextFit);
    result["icon-text-fit-padding"] = makePropertySetter(&SymbolLayer::setIconTextFitPadding);
    result["icon-image"] = makePropertySetter(&SymbolLayer::setIconImage);
    result["icon-rotate"] = makePropertySetter(&SymbolLayer::setIconRotate);
    result["icon-padding"] = makePropertySetter(&SymbolLayer::setIconPadding);
    result["icon-keep-upright"] = makePropertySetter(&SymbolLayer::setIconKeepUpright);
    result["icon-offset"] = makePropertySetter(&SymbolLayer::setIconOffset);
    result["text-pitch-alignment"] = makePropertySetter(&SymbolLayer::setTextPitchAlignment);
    result["text-rotation-alignment"] = makePropertySetter(&SymbolLayer::setTextRotationAlignment);
    result["text-field"] = makePropertySetter(&SymbolLayer::setTextField);
    result["text-font"] = makePropertySetter(&SymbolLayer::setTextFont);
    result["text-size"] = makePropertySetter(&SymbolLayer::setTextSize);
    result["text-max-width"] = makePropertySetter(&SymbolLayer::setTextMaxWidth);
    result["text-line-height"] = makePropertySetter(&SymbolLayer::setTextLineHeight);
    result["text-letter-spacing"] = makePropertySetter(&SymbolLayer::setTextLetterSpacing);
    result["text-justify"] = makePropertySetter(&SymbolLayer::setTextJustify);
    result["text-anchor"] = makePropertySetter(&SymbolLayer::setTextAnchor);
    result["text-max-angle"] = makePropertySetter(&SymbolLayer::setTextMaxAngle);
    result["text-rotate"] = makePropertySetter(&SymbolLayer::setTextRotate);
    result["text-padding"] = makePropertySetter(&SymbolLayer::setTextPadding);
    result["text-keep-upright"] = makePropertySetter(&SymbolLayer::setTextKeepUpright);
    result["text-transform"] = makePropertySetter(&SymbolLayer::setTextTransform);
    result["text-offset"] = makePropertySetter(&SymbolLayer::setTextOffset);
    result["text-allow-overlap"] = makePropertySetter(&SymbolLayer::setTextAllowOverlap);
    result["text-ignore-placement"] = makePropertySetter(&SymbolLayer::setTextIgnorePlacement);
    result["text-optional"] = makePropertySetter(&SymbolLayer::setTextOptional);




    return result;
}

inline PropertySetters makePaintPropertySetters() {
    using namespace mbgl::style;
    PropertySetters result;

    result["fill-antialias"] = makePropertySetter(&FillLayer::setFillAntialias);
    result["fill-opacity"] = makePropertySetter(&FillLayer::setFillOpacity);
    result["fill-color"] = makePropertySetter(&FillLayer::setFillColor);
    result["fill-outline-color"] = makePropertySetter(&FillLayer::setFillOutlineColor);
    result["fill-translate"] = makePropertySetter(&FillLayer::setFillTranslate);
    result["fill-translate-anchor"] = makePropertySetter(&FillLayer::setFillTranslateAnchor);
    result["fill-pattern"] = makePropertySetter(&FillLayer::setFillPattern);

    result["line-opacity"] = makePropertySetter(&LineLayer::setLineOpacity);
    result["line-color"] = makePropertySetter(&LineLayer::setLineColor);
    result["line-translate"] = makePropertySetter(&LineLayer::setLineTranslate);
    result["line-translate-anchor"] = makePropertySetter(&LineLayer::setLineTranslateAnchor);
    result["line-width"] = makePropertySetter(&LineLayer::setLineWidth);
    result["line-gap-width"] = makePropertySetter(&LineLayer::setLineGapWidth);
    result["line-offset"] = makePropertySetter(&LineLayer::setLineOffset);
    result["line-blur"] = makePropertySetter(&LineLayer::setLineBlur);
    result["line-dasharray"] = makePropertySetter(&LineLayer::setLineDasharray);
    result["line-pattern"] = makePropertySetter(&LineLayer::setLinePattern);

    result["icon-opacity"] = makePropertySetter(&SymbolLayer::setIconOpacity);
    result["icon-color"] = makePropertySetter(&SymbolLayer::setIconColor);
    result["icon-halo-color"] = makePropertySetter(&SymbolLayer::setIconHaloColor);
    result["icon-halo-width"] = makePropertySetter(&SymbolLayer::setIconHaloWidth);
    result["icon-halo-blur"] = makePropertySetter(&SymbolLayer::setIconHaloBlur);
    result["icon-translate"] = makePropertySetter(&SymbolLayer::setIconTranslate);
    result["icon-translate-anchor"] = makePropertySetter(&SymbolLayer::setIconTranslateAnchor);
    result["text-opacity"] = makePropertySetter(&SymbolLayer::setTextOpacity);
    result["text-color"] = makePropertySetter(&SymbolLayer::setTextColor);
    result["text-halo-color"] = makePropertySetter(&SymbolLayer::setTextHaloColor);
    result["text-halo-width"] = makePropertySetter(&SymbolLayer::setTextHaloWidth);
    result["text-halo-blur"] = makePropertySetter(&SymbolLayer::setTextHaloBlur);
    result["text-translate"] = makePropertySetter(&SymbolLayer::setTextTranslate);
    result["text-translate-anchor"] = makePropertySetter(&SymbolLayer::setTextTranslateAnchor);

    result["circle-radius"] = makePropertySetter(&CircleLayer::setCircleRadius);
    result["circle-color"] = makePropertySetter(&CircleLayer::setCircleColor);
    result["circle-blur"] = makePropertySetter(&CircleLayer::setCircleBlur);
    result["circle-opacity"] = makePropertySetter(&CircleLayer::setCircleOpacity);
    result["circle-translate"] = makePropertySetter(&CircleLayer::setCircleTranslate);
    result["circle-translate-anchor"] = makePropertySetter(&CircleLayer::setCircleTranslateAnchor);

    result["raster-opacity"] = makePropertySetter(&RasterLayer::setRasterOpacity);
    result["raster-hue-rotate"] = makePropertySetter(&RasterLayer::setRasterHueRotate);
    result["raster-brightness-min"] = makePropertySetter(&RasterLayer::setRasterBrightnessMin);
    result["raster-brightness-max"] = makePropertySetter(&RasterLayer::setRasterBrightnessMax);
    result["raster-saturation"] = makePropertySetter(&RasterLayer::setRasterSaturation);
    result["raster-contrast"] = makePropertySetter(&RasterLayer::setRasterContrast);
    result["raster-fade-duration"] = makePropertySetter(&RasterLayer::setRasterFadeDuration);

    result["background-color"] = makePropertySetter(&BackgroundLayer::setBackgroundColor);
    result["background-pattern"] = makePropertySetter(&BackgroundLayer::setBackgroundPattern);
    result["background-opacity"] = makePropertySetter(&BackgroundLayer::setBackgroundOpacity);

    return result;
}

}
