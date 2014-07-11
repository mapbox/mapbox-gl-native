#ifndef LLMR_STYLE_PROPERTY_KEY
#define LLMR_STYLE_PROPERTY_KEY

namespace llmr {

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
    IconRotateAnchor,

    TextOpacity,
    TextSize,
    TextColor,
    TextHaloColor,
    TextHaloWidth,
    TextHaloBlur,

    CompositeOpacity,

    RasterOpacity,
    RasterSpin,
    RasterBrightnessLow,
    RasterBrightnessHigh,
    RasterSaturation,
    RasterContrast,
    RasterFade,

    BackgroundColor
};

}

#endif
