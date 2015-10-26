#include <mbgl/style/property_fallback.hpp>
#include <mbgl/style/style_properties.hpp>

namespace mbgl {

const std::map<PropertyKey, PropertyValue> PropertyFallbackValue::properties = {
    { PropertyKey::FillAntialias, FillPaintProperties().antialias },
    { PropertyKey::FillOpacity, FillPaintProperties().opacity },
    { PropertyKey::FillColor, FillPaintProperties().fill_color },
    // no FillOutlineColor on purpose.
    { PropertyKey::FillTranslate, FillPaintProperties().translate },
    { PropertyKey::FillTranslateAnchor, FillPaintProperties().translateAnchor },

    { PropertyKey::LineOpacity, LinePaintProperties().opacity },
    { PropertyKey::LineColor, LinePaintProperties().color },
    { PropertyKey::LineTranslate, LinePaintProperties().translate },
    { PropertyKey::LineTranslateAnchor, LinePaintProperties().translateAnchor },
    { PropertyKey::LineWidth, LinePaintProperties().width },
    { PropertyKey::LineGapWidth, LinePaintProperties().gap_width },
    { PropertyKey::LineBlur, LinePaintProperties().blur },

    { PropertyKey::CircleRadius, CirclePaintProperties().radius },
    { PropertyKey::CircleColor, CirclePaintProperties().color },
    { PropertyKey::CircleOpacity, CirclePaintProperties().opacity },
    { PropertyKey::CircleTranslate, CirclePaintProperties().translate },
    { PropertyKey::CircleTranslateAnchor, CirclePaintProperties().translateAnchor },
    { PropertyKey::CircleBlur, CirclePaintProperties().blur },

    { PropertyKey::IconOpacity, SymbolPaintProperties().icon.opacity },
    { PropertyKey::IconColor, SymbolPaintProperties().icon.color },
    { PropertyKey::IconHaloColor, SymbolPaintProperties().icon.halo_color },
    { PropertyKey::IconHaloWidth, SymbolPaintProperties().icon.halo_width },
    { PropertyKey::IconHaloBlur, SymbolPaintProperties().icon.halo_blur },
    { PropertyKey::IconTranslate, SymbolPaintProperties().icon.translate },
    { PropertyKey::IconTranslateAnchor, SymbolPaintProperties().icon.translate_anchor },

    { PropertyKey::TextOpacity, SymbolPaintProperties().text.opacity },
    { PropertyKey::TextColor, SymbolPaintProperties().text.color },
    { PropertyKey::TextHaloColor, SymbolPaintProperties().text.halo_color },
    { PropertyKey::TextHaloWidth, SymbolPaintProperties().text.halo_width },
    { PropertyKey::TextHaloBlur, SymbolPaintProperties().text.halo_blur },
    { PropertyKey::TextTranslate, SymbolPaintProperties().text.translate },
    { PropertyKey::TextTranslateAnchor, SymbolPaintProperties().text.translate_anchor },

    { PropertyKey::RasterOpacity, RasterPaintProperties().opacity },
    { PropertyKey::RasterHueRotate, RasterPaintProperties().hue_rotate },
    { PropertyKey::RasterBrightnessLow, RasterPaintProperties().brightness[0] },
    { PropertyKey::RasterBrightnessHigh, RasterPaintProperties().brightness[1] },
    { PropertyKey::RasterSaturation, RasterPaintProperties().saturation },
    { PropertyKey::RasterContrast, RasterPaintProperties().contrast },
    { PropertyKey::RasterFade, RasterPaintProperties().fade },

    { PropertyKey::BackgroundOpacity, BackgroundPaintProperties().opacity },
    { PropertyKey::BackgroundColor, BackgroundPaintProperties().color },
};

const PropertyValue PropertyFallbackValue::defaultProperty = false;

}
