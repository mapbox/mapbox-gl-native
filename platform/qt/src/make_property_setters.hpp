#pragma once

// This file is generated. Edit make_property_setters.hpp.ejs, then run `make style-code`.

#include "property_setter.hpp"

#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/background_layer.hpp>

#include <QMapbox>

#include <map>

namespace mbgl {
namespace style {
namespace conversion {

auto makeQtLayoutPropertySetters() {
    std::map<QMapbox::LayoutPropertyType, QtLayoutPropertySetter> result;

    result[QMapbox::Visibility] = &setQtVisibility;
    result[QMapbox::LineCap] = makeQtPropertySetter(&LineLayer::setLineCap);
    result[QMapbox::LineJoin] = makeQtPropertySetter(&LineLayer::setLineJoin);
    result[QMapbox::LineMiterLimit] = makeQtPropertySetter(&LineLayer::setLineMiterLimit);
    result[QMapbox::LineRoundLimit] = makeQtPropertySetter(&LineLayer::setLineRoundLimit);
    result[QMapbox::SymbolPlacement] = makeQtPropertySetter(&SymbolLayer::setSymbolPlacement);
    result[QMapbox::SymbolSpacing] = makeQtPropertySetter(&SymbolLayer::setSymbolSpacing);
    result[QMapbox::SymbolAvoidEdges] = makeQtPropertySetter(&SymbolLayer::setSymbolAvoidEdges);
    result[QMapbox::IconAllowOverlap] = makeQtPropertySetter(&SymbolLayer::setIconAllowOverlap);
    result[QMapbox::IconIgnorePlacement] = makeQtPropertySetter(&SymbolLayer::setIconIgnorePlacement);
    result[QMapbox::IconOptional] = makeQtPropertySetter(&SymbolLayer::setIconOptional);
    result[QMapbox::IconRotationAlignment] = makeQtPropertySetter(&SymbolLayer::setIconRotationAlignment);
    result[QMapbox::IconSize] = makeQtPropertySetter(&SymbolLayer::setIconSize);
    result[QMapbox::IconTextFit] = makeQtPropertySetter(&SymbolLayer::setIconTextFit);
    result[QMapbox::IconTextFitPadding] = makeQtPropertySetter(&SymbolLayer::setIconTextFitPadding);
    result[QMapbox::IconImage] = makeQtPropertySetter(&SymbolLayer::setIconImage);
    result[QMapbox::IconRotate] = makeQtPropertySetter(&SymbolLayer::setIconRotate);
    result[QMapbox::IconPadding] = makeQtPropertySetter(&SymbolLayer::setIconPadding);
    result[QMapbox::IconKeepUpright] = makeQtPropertySetter(&SymbolLayer::setIconKeepUpright);
    result[QMapbox::IconOffset] = makeQtPropertySetter(&SymbolLayer::setIconOffset);
    result[QMapbox::TextPitchAlignment] = makeQtPropertySetter(&SymbolLayer::setTextPitchAlignment);
    result[QMapbox::TextRotationAlignment] = makeQtPropertySetter(&SymbolLayer::setTextRotationAlignment);
    result[QMapbox::TextField] = makeQtPropertySetter(&SymbolLayer::setTextField);
    result[QMapbox::TextFont] = makeQtPropertySetter(&SymbolLayer::setTextFont);
    result[QMapbox::TextSize] = makeQtPropertySetter(&SymbolLayer::setTextSize);
    result[QMapbox::TextMaxWidth] = makeQtPropertySetter(&SymbolLayer::setTextMaxWidth);
    result[QMapbox::TextLineHeight] = makeQtPropertySetter(&SymbolLayer::setTextLineHeight);
    result[QMapbox::TextLetterSpacing] = makeQtPropertySetter(&SymbolLayer::setTextLetterSpacing);
    result[QMapbox::TextJustify] = makeQtPropertySetter(&SymbolLayer::setTextJustify);
    result[QMapbox::TextAnchor] = makeQtPropertySetter(&SymbolLayer::setTextAnchor);
    result[QMapbox::TextMaxAngle] = makeQtPropertySetter(&SymbolLayer::setTextMaxAngle);
    result[QMapbox::TextRotate] = makeQtPropertySetter(&SymbolLayer::setTextRotate);
    result[QMapbox::TextPadding] = makeQtPropertySetter(&SymbolLayer::setTextPadding);
    result[QMapbox::TextKeepUpright] = makeQtPropertySetter(&SymbolLayer::setTextKeepUpright);
    result[QMapbox::TextTransform] = makeQtPropertySetter(&SymbolLayer::setTextTransform);
    result[QMapbox::TextOffset] = makeQtPropertySetter(&SymbolLayer::setTextOffset);
    result[QMapbox::TextAllowOverlap] = makeQtPropertySetter(&SymbolLayer::setTextAllowOverlap);
    result[QMapbox::TextIgnorePlacement] = makeQtPropertySetter(&SymbolLayer::setTextIgnorePlacement);
    result[QMapbox::TextOptional] = makeQtPropertySetter(&SymbolLayer::setTextOptional);

    return result;
}

auto makeQtPaintPropertySetters() {
    std::map<QMapbox::PaintPropertyType, QtPaintPropertySetter> result;

    result[QMapbox::FillAntialias] = makeQtPropertySetter(&FillLayer::setFillAntialias);
    result[QMapbox::FillOpacity] = makeQtPropertySetter(&FillLayer::setFillOpacity);
    result[QMapbox::FillColor] = makeQtPropertySetter(&FillLayer::setFillColor);
    result[QMapbox::FillOutlineColor] = makeQtPropertySetter(&FillLayer::setFillOutlineColor);
    result[QMapbox::FillTranslate] = makeQtPropertySetter(&FillLayer::setFillTranslate);
    result[QMapbox::FillTranslateAnchor] = makeQtPropertySetter(&FillLayer::setFillTranslateAnchor);
    result[QMapbox::FillPattern] = makeQtPropertySetter(&FillLayer::setFillPattern);
    result[QMapbox::LineOpacity] = makeQtPropertySetter(&LineLayer::setLineOpacity);
    result[QMapbox::LineColor] = makeQtPropertySetter(&LineLayer::setLineColor);
    result[QMapbox::LineTranslate] = makeQtPropertySetter(&LineLayer::setLineTranslate);
    result[QMapbox::LineTranslateAnchor] = makeQtPropertySetter(&LineLayer::setLineTranslateAnchor);
    result[QMapbox::LineWidth] = makeQtPropertySetter(&LineLayer::setLineWidth);
    result[QMapbox::LineGapWidth] = makeQtPropertySetter(&LineLayer::setLineGapWidth);
    result[QMapbox::LineOffset] = makeQtPropertySetter(&LineLayer::setLineOffset);
    result[QMapbox::LineBlur] = makeQtPropertySetter(&LineLayer::setLineBlur);
    result[QMapbox::LineDasharray] = makeQtPropertySetter(&LineLayer::setLineDasharray);
    result[QMapbox::LinePattern] = makeQtPropertySetter(&LineLayer::setLinePattern);
    result[QMapbox::IconOpacity] = makeQtPropertySetter(&SymbolLayer::setIconOpacity);
    result[QMapbox::IconColor] = makeQtPropertySetter(&SymbolLayer::setIconColor);
    result[QMapbox::IconHaloColor] = makeQtPropertySetter(&SymbolLayer::setIconHaloColor);
    result[QMapbox::IconHaloWidth] = makeQtPropertySetter(&SymbolLayer::setIconHaloWidth);
    result[QMapbox::IconHaloBlur] = makeQtPropertySetter(&SymbolLayer::setIconHaloBlur);
    result[QMapbox::IconTranslate] = makeQtPropertySetter(&SymbolLayer::setIconTranslate);
    result[QMapbox::IconTranslateAnchor] = makeQtPropertySetter(&SymbolLayer::setIconTranslateAnchor);
    result[QMapbox::TextOpacity] = makeQtPropertySetter(&SymbolLayer::setTextOpacity);
    result[QMapbox::TextColor] = makeQtPropertySetter(&SymbolLayer::setTextColor);
    result[QMapbox::TextHaloColor] = makeQtPropertySetter(&SymbolLayer::setTextHaloColor);
    result[QMapbox::TextHaloWidth] = makeQtPropertySetter(&SymbolLayer::setTextHaloWidth);
    result[QMapbox::TextHaloBlur] = makeQtPropertySetter(&SymbolLayer::setTextHaloBlur);
    result[QMapbox::TextTranslate] = makeQtPropertySetter(&SymbolLayer::setTextTranslate);
    result[QMapbox::TextTranslateAnchor] = makeQtPropertySetter(&SymbolLayer::setTextTranslateAnchor);
    result[QMapbox::CircleRadius] = makeQtPropertySetter(&CircleLayer::setCircleRadius);
    result[QMapbox::CircleColor] = makeQtPropertySetter(&CircleLayer::setCircleColor);
    result[QMapbox::CircleBlur] = makeQtPropertySetter(&CircleLayer::setCircleBlur);
    result[QMapbox::CircleOpacity] = makeQtPropertySetter(&CircleLayer::setCircleOpacity);
    result[QMapbox::CircleTranslate] = makeQtPropertySetter(&CircleLayer::setCircleTranslate);
    result[QMapbox::CircleTranslateAnchor] = makeQtPropertySetter(&CircleLayer::setCircleTranslateAnchor);
    result[QMapbox::RasterOpacity] = makeQtPropertySetter(&RasterLayer::setRasterOpacity);
    result[QMapbox::RasterHueRotate] = makeQtPropertySetter(&RasterLayer::setRasterHueRotate);
    result[QMapbox::RasterBrightnessMin] = makeQtPropertySetter(&RasterLayer::setRasterBrightnessMin);
    result[QMapbox::RasterBrightnessMax] = makeQtPropertySetter(&RasterLayer::setRasterBrightnessMax);
    result[QMapbox::RasterSaturation] = makeQtPropertySetter(&RasterLayer::setRasterSaturation);
    result[QMapbox::RasterContrast] = makeQtPropertySetter(&RasterLayer::setRasterContrast);
    result[QMapbox::RasterFadeDuration] = makeQtPropertySetter(&RasterLayer::setRasterFadeDuration);
    result[QMapbox::BackgroundColor] = makeQtPropertySetter(&BackgroundLayer::setBackgroundColor);
    result[QMapbox::BackgroundPattern] = makeQtPropertySetter(&BackgroundLayer::setBackgroundPattern);
    result[QMapbox::BackgroundOpacity] = makeQtPropertySetter(&BackgroundLayer::setBackgroundOpacity);

    return result;
}

} // namespace conversion
} // namespace style
} // namespace mbgl
