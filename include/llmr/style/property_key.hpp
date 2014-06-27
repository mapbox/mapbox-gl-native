#ifndef LLMR_STYLE_PROPERTY_KEY
#define LLMR_STYLE_PROPERTY_KEY

namespace llmr {

enum class PropertyKey {
    FillEnabled,
    FillAntialias,
    FillOpacity,
    FillColor,
    FillOutlineColor,
    FillTranslate, // for transitions only
    FillTranslateX,
    FillTranslateY,
    FillTranslateAnchor,
    FillImage,

    LineEnabled,
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

    IconEnabled,
    IconOpacity,
    IconRotate,
    IconRotateAnchor,

    TextEnabled,
    TextOpacity,
    TextSize,
    TextColor,
    TextHaloColor,
    TextHaloWidth,
    TextHaloBlur,

    CompositeEnabled,
    CompositeOpacity,

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
