#include <mbgl/style/property_fallback.hpp>
#include <mbgl/style/style_properties.hpp>

namespace mbgl {

const std::map<PropertyKey, PropertyValue> PropertyFallbackValue::properties = {
    { PropertyKey::FillAntialias, defaultStyleProperties<FillProperties>().antialias },
    { PropertyKey::FillOpacity, defaultStyleProperties<FillProperties>().opacity },
    { PropertyKey::FillColor, defaultStyleProperties<FillProperties>().fill_color },
    // no FillOutlineColor on purpose.
    { PropertyKey::FillTranslateX, defaultStyleProperties<FillProperties>().translate[0] },
    { PropertyKey::FillTranslateY, defaultStyleProperties<FillProperties>().translate[1] },
    { PropertyKey::FillTranslateAnchor, defaultStyleProperties<FillProperties>().translateAnchor },

    { PropertyKey::LineOpacity, defaultStyleProperties<LineProperties>().opacity },
    { PropertyKey::LineColor, defaultStyleProperties<LineProperties>().color },
    { PropertyKey::LineTranslateX, defaultStyleProperties<LineProperties>().translate[0] },
    { PropertyKey::LineTranslateY, defaultStyleProperties<LineProperties>().translate[1] },
    { PropertyKey::LineTranslateAnchor, defaultStyleProperties<LineProperties>().translateAnchor },
    { PropertyKey::LineWidth, defaultStyleProperties<LineProperties>().width },
    { PropertyKey::LineOffset, defaultStyleProperties<LineProperties>().offset },
    { PropertyKey::LineBlur, defaultStyleProperties<LineProperties>().blur },
    { PropertyKey::LineDashLand, defaultStyleProperties<LineProperties>().dash_array[0] },
    { PropertyKey::LineDashGap, defaultStyleProperties<LineProperties>().dash_array[1] },

    { PropertyKey::IconOpacity, defaultStyleProperties<SymbolProperties>().icon.opacity },
    { PropertyKey::IconRotate, defaultStyleProperties<SymbolProperties>().icon.rotate },
    { PropertyKey::IconSize, defaultStyleProperties<SymbolProperties>().icon.size },
    { PropertyKey::IconColor, defaultStyleProperties<SymbolProperties>().icon.color },
    { PropertyKey::IconHaloColor, defaultStyleProperties<SymbolProperties>().icon.halo_color },
    { PropertyKey::IconHaloWidth, defaultStyleProperties<SymbolProperties>().icon.halo_width },
    { PropertyKey::IconHaloBlur, defaultStyleProperties<SymbolProperties>().icon.halo_blur },
    { PropertyKey::IconTranslateX, defaultStyleProperties<SymbolProperties>().icon.translate[0] },
    { PropertyKey::IconTranslateY, defaultStyleProperties<SymbolProperties>().icon.translate[1] },
    { PropertyKey::IconTranslateAnchor, defaultStyleProperties<SymbolProperties>().icon.translate_anchor },

    { PropertyKey::TextOpacity, defaultStyleProperties<SymbolProperties>().text.opacity },
    { PropertyKey::TextSize, defaultStyleProperties<SymbolProperties>().text.size },
    { PropertyKey::TextColor, defaultStyleProperties<SymbolProperties>().text.color },
    { PropertyKey::TextHaloColor, defaultStyleProperties<SymbolProperties>().text.halo_color },
    { PropertyKey::TextHaloWidth, defaultStyleProperties<SymbolProperties>().text.halo_width },
    { PropertyKey::TextHaloBlur, defaultStyleProperties<SymbolProperties>().text.halo_blur },
    { PropertyKey::TextTranslateX, defaultStyleProperties<SymbolProperties>().text.translate[0] },
    { PropertyKey::TextTranslateY, defaultStyleProperties<SymbolProperties>().text.translate[1] },
    { PropertyKey::TextTranslateAnchor, defaultStyleProperties<SymbolProperties>().text.translate_anchor },

    { PropertyKey::RasterOpacity, defaultStyleProperties<RasterProperties>().opacity },
    { PropertyKey::RasterHueRotate, defaultStyleProperties<RasterProperties>().hue_rotate },
    { PropertyKey::RasterBrightnessLow, defaultStyleProperties<RasterProperties>().brightness[0] },
    { PropertyKey::RasterBrightnessHigh, defaultStyleProperties<RasterProperties>().brightness[1] },
    { PropertyKey::RasterSaturation, defaultStyleProperties<RasterProperties>().saturation },
    { PropertyKey::RasterContrast, defaultStyleProperties<RasterProperties>().contrast },
    { PropertyKey::RasterFade, defaultStyleProperties<RasterProperties>().fade },

    { PropertyKey::BackgroundColor, defaultStyleProperties<BackgroundProperties>().color },
};

const PropertyValue PropertyFallbackValue::defaultProperty = false;

}
