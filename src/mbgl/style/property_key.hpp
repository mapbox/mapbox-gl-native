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
    LineGapWidth,
    LineBlur,
    LineDashArray, // for transitions only
    LineImage,

    LineCap,
    LineJoin,
    LineMiterLimit,
    LineRoundLimit,

    SymbolPlacement,
    SymbolMinDistance,
    SymbolAvoidEdges,

    IconOpacity,
    IconSize,
    IconColor,
    IconHaloColor,
    IconHaloWidth,
    IconHaloBlur,
    IconTranslate, // for transitions only
    IconTranslateX,
    IconTranslateY,
    IconTranslateAnchor,

    IconAllowOverlap,
    IconIgnorePlacement,
    IconOptional,
    IconRotationAlignment,
    IconMaxSize,
    IconImage,
    IconPadding,
    IconRotate,
    IconKeepUpright,
    IconOffsetX,
    IconOffsetY,

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
    TextOffsetX,
    TextOffsetY,
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
