#ifndef MBGL_STYLE_PROPERTY_KEY
#define MBGL_STYLE_PROPERTY_KEY

namespace mbgl {

enum class PropertyKey {
    FillAntialias,
    FillOpacity,
    FillColor,
    FillOutlineColor,
    FillTranslate, // for transitions only
    FillTranslateAnchor,
    FillImage,

    LineOpacity,
    LineColor,
    LineTranslate, // for transitions only
    LineTranslateAnchor,
    LineWidth,
    LineGapWidth,
    LineBlur,
    LineDashArray, // for transitions only
    LineImage,

    CircleRadius,
    CircleColor,
    CircleOpacity,
    CircleTranslate,
    CircleTranslateAnchor,
    CircleBlur,

    IconOpacity,
    IconColor,
    IconHaloColor,
    IconHaloWidth,
    IconHaloBlur,
    IconTranslate,
    IconTranslateAnchor,

    TextOpacity,
    TextColor,
    TextHaloColor,
    TextHaloWidth,
    TextHaloBlur,
    TextTranslate,
    TextTranslateAnchor,

    RasterOpacity,
    RasterHueRotate,
    RasterBrightness, // for transitions only
    RasterBrightnessLow,
    RasterBrightnessHigh,
    RasterSaturation,
    RasterContrast,
    RasterFade,

    BackgroundOpacity,
    BackgroundColor,
    BackgroundImage,
};

}

#endif
