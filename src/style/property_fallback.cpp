#include <llmr/style/property_fallback.hpp>

namespace llmr {

const std::map<PropertyKey, PropertyValue> PropertyFallbackValue::properties = {
    { PropertyKey::FillEnabled, true },
    { PropertyKey::FillAntialias, true },
    { PropertyKey::FillOpacity, 1.0f },
    { PropertyKey::FillColor, Color({{ 0, 0, 0, 1 }}) },
    { PropertyKey::FillTranslateX, 0.0f },
    { PropertyKey::FillTranslateY, 0.0f },
    { PropertyKey::FillTranslateAnchor, TranslateAnchorType::Map },

    { PropertyKey::LineEnabled, true },
    { PropertyKey::LineOpacity, 1.0f },
    { PropertyKey::LineColor, Color({{ 0, 0, 0, 1 }}) },
    { PropertyKey::LineTranslateX, 0.0f },
    { PropertyKey::LineTranslateY, 0.0f },
    { PropertyKey::LineTranslateAnchor, TranslateAnchorType::Map },
    { PropertyKey::LineWidth, 1.0f },
    { PropertyKey::LineOffset, 0.0f },
    { PropertyKey::LineBlur, 1.0f },
    { PropertyKey::LineDashLand, 1.0f },
    { PropertyKey::LineDashGap, -1.0f },

    { PropertyKey::IconEnabled, true },
    { PropertyKey::IconOpacity, 1.0f },
    { PropertyKey::IconRotate, 0.0f },
    { PropertyKey::IconRotateAnchor, RotateAnchorType::Viewport },

    { PropertyKey::TextEnabled, true },
    { PropertyKey::TextOpacity, 1.0f },
    { PropertyKey::TextSize, 16.0f },
    { PropertyKey::TextColor, Color({{ 0, 0, 0, 1 }}) },
    { PropertyKey::TextHaloColor, Color({{ 0, 0, 0, 0 }}) },
    { PropertyKey::TextHaloWidth, 0.25f },
    { PropertyKey::TextHaloBlur, 1.0f },

    { PropertyKey::CompositeEnabled, true },
    { PropertyKey::CompositeOpacity, 1.0f },

    { PropertyKey::RasterEnabled, true },
    { PropertyKey::RasterOpacity, 1.0f },
    { PropertyKey::RasterSpin, 0.0f },
    { PropertyKey::RasterBrightnessLow, 0.0f },
    { PropertyKey::RasterBrightnessHigh, 1.0f },
    { PropertyKey::RasterSaturation, 0.0f },
    { PropertyKey::RasterContrast, 0.0f },
    { PropertyKey::RasterFade, 0.0f },

    { PropertyKey::BackgroundColor, Color({{ 0, 0, 0, 0 }}) },
};

const PropertyValue PropertyFallbackValue::defaultProperty = false;

}
