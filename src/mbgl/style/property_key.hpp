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

    LineCap,
    LineJoin,
    LineMiterLimit,
    LineRoundLimit,

    CircleRadius,
    CircleColor,
    CircleBlur,

    SymbolPlacement,
    SymbolMinDistance,
    SymbolAvoidEdges,

    IconOpacity,
    IconSize,
    IconColor,
    IconHaloColor,
    IconHaloWidth,
    IconHaloBlur,
    IconTranslate,
    IconTranslateAnchor,

    IconAllowOverlap,
    IconIgnorePlacement,
    IconOptional,
    IconRotationAlignment,
    IconMaxSize,
    IconImage,
    IconOffset,
    IconPadding,
    IconRotate,
    IconKeepUpright,

    TextOpacity,
    TextSize,
    TextColor,
    TextHaloColor,
    TextHaloWidth,
    TextHaloBlur,
    TextTranslate,
    TextTranslateAnchor,

    TextRotationAlignment,
    TextField,
    TextFont,
    TextMaxSize,
    TextMaxWidth,
    TextLineHeight,
    TextLetterSpacing,
    TextMaxAngle,
    TextRotate,
    TextPadding,
    TextIgnorePlacement,
    TextOptional,
    TextJustify,
    TextAnchor,
    TextKeepUpright,
    TextTransform,
    TextOffset,
    TextAllowOverlap,

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

    Visibilty
};

}

#endif
