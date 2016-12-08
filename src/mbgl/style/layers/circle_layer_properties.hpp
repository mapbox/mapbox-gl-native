// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>

namespace mbgl {
namespace style {

struct CircleRadius : PaintProperty<float> {
    static float defaultValue() { return 5; }
};

struct CircleColor : PaintProperty<Color> {
    static Color defaultValue() { return Color::black(); }
};

struct CircleBlur : PaintProperty<float> {
    static float defaultValue() { return 0; }
};

struct CircleOpacity : PaintProperty<float> {
    static float defaultValue() { return 1; }
};

struct CircleTranslate : PaintProperty<std::array<float, 2>> {
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct CircleTranslateAnchor : PaintProperty<TranslateAnchorType> {
    static TranslateAnchorType defaultValue() { return TranslateAnchorType::Map; }
};

struct CirclePitchScale : PaintProperty<CirclePitchScaleType> {
    static CirclePitchScaleType defaultValue() { return CirclePitchScaleType::Map; }
};

struct CircleStrokeWidth : PaintProperty<float> {
    static float defaultValue() { return 0; }
};

struct CircleStrokeColor : PaintProperty<Color> {
    static Color defaultValue() { return Color::black(); }
};

struct CircleStrokeOpacity : PaintProperty<float> {
    static float defaultValue() { return 1; }
};

class CirclePaintProperties : public PaintProperties<
    CircleRadius,
    CircleColor,
    CircleBlur,
    CircleOpacity,
    CircleTranslate,
    CircleTranslateAnchor,
    CirclePitchScale,
    CircleStrokeWidth,
    CircleStrokeColor,
    CircleStrokeOpacity
> {};

} // namespace style
} // namespace mbgl
