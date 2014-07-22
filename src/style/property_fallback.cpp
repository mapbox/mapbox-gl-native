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

    { PropertyKey::IconOpacity, defaultStyleProperties<IconProperties>().opacity },
    { PropertyKey::IconRotate, defaultStyleProperties<IconProperties>().rotate },
    { PropertyKey::IconRotateAnchor, defaultStyleProperties<IconProperties>().rotate_anchor },

    { PropertyKey::TextOpacity, defaultStyleProperties<TextProperties>().opacity },
    { PropertyKey::TextSize, defaultStyleProperties<TextProperties>().size },
    { PropertyKey::TextColor, defaultStyleProperties<TextProperties>().color },
    { PropertyKey::TextHaloColor, defaultStyleProperties<TextProperties>().halo_color },
    { PropertyKey::TextHaloWidth, defaultStyleProperties<TextProperties>().halo_width },
    { PropertyKey::TextHaloBlur, defaultStyleProperties<TextProperties>().halo_blur },

    { PropertyKey::CompositeOpacity, defaultStyleProperties<CompositeProperties>().opacity },

    { PropertyKey::RasterOpacity, defaultStyleProperties<RasterProperties>().opacity },
    { PropertyKey::RasterSpin, defaultStyleProperties<RasterProperties>().spin },
    { PropertyKey::RasterBrightnessLow, defaultStyleProperties<RasterProperties>().brightness[0] },
    { PropertyKey::RasterBrightnessHigh, defaultStyleProperties<RasterProperties>().brightness[1] },
    { PropertyKey::RasterSaturation, defaultStyleProperties<RasterProperties>().saturation },
    { PropertyKey::RasterContrast, defaultStyleProperties<RasterProperties>().contrast },
    { PropertyKey::RasterFade, defaultStyleProperties<RasterProperties>().fade },

    { PropertyKey::BackgroundColor, defaultStyleProperties<BackgroundProperties>().color },
};

const PropertyValue PropertyFallbackValue::defaultProperty = false;

}
