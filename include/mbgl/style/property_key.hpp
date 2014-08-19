#ifndef MBGL_STYLE_PROPERTY_KEY
#define MBGL_STYLE_PROPERTY_KEY

namespace mbgl {

enum class PropertyKey {
    FillAntialias,
    FillOpacity,
    FillColor,
    FillOutlineColor,
    FillTranslate, // for transitions only
    FillTranslateX,
    FillTranslateY,
    FillTranslateAnchor,
    FillImage,

    LineOpacity,
    LineColor,
    LineTranslate, // for transitions only
    LineTranslateX,
    LineTranslateY,
    LineTranslateAnchor,
    LineWidth,
    LineOffset,
    LineBlur,
    LineDashArray, // for transitions only
    LineDashLand,
    LineDashGap,
    LineImage,

    IconOpacity,
    IconRotate,
    IconSize,
    IconColor,
    IconHaloColor,
    IconHaloWidth,
    IconHaloBlur,
    IconTranslate, // for transitions only
    IconTranslateX,
    IconTranslateY,
    IconTranslateAnchor,

    TextOpacity,
    TextSize,
    TextColor,
    TextHaloColor,
    TextHaloWidth,
    TextHaloBlur,
    TextTranslate, // for transitions only
    TextTranslateX,
    TextTranslateY,
    TextTranslateAnchor,

    RasterOpacity,
    RasterHueRotate,
    RasterBrightness, // for transitions only
    RasterBrightnessLow,
    RasterBrightnessHigh,
    RasterSaturation,
    RasterContrast,
    RasterFade,

    BackgroundColor
};

}

#endif
