// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/style/properties.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>

namespace mbgl {
namespace style {

struct CircleRadius : DataDrivenPaintProperty<float, attributes::a_radius, uniforms::u_radius> {
    static float defaultValue() { return 5; }
};

struct CircleColor : DataDrivenPaintProperty<Color, attributes::a_color, uniforms::u_color> {
    static Color defaultValue() { return Color::black(); }
};

struct CircleBlur : DataDrivenPaintProperty<float, attributes::a_blur, uniforms::u_blur> {
    static float defaultValue() { return 0; }
};

struct CircleOpacity : DataDrivenPaintProperty<float, attributes::a_opacity, uniforms::u_opacity> {
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

struct CirclePitchAlignment : PaintProperty<AlignmentType> {
    static AlignmentType defaultValue() { return AlignmentType::Viewport; }
};

struct CircleStrokeWidth : DataDrivenPaintProperty<float, attributes::a_stroke_width, uniforms::u_stroke_width> {
    static float defaultValue() { return 0; }
};

struct CircleStrokeColor : DataDrivenPaintProperty<Color, attributes::a_stroke_color, uniforms::u_stroke_color> {
    static Color defaultValue() { return Color::black(); }
};

struct CircleStrokeOpacity : DataDrivenPaintProperty<float, attributes::a_stroke_opacity, uniforms::u_stroke_opacity> {
    static float defaultValue() { return 1; }
};

class CirclePaintProperties : public Properties<
    CircleRadius,
    CircleColor,
    CircleBlur,
    CircleOpacity,
    CircleTranslate,
    CircleTranslateAnchor,
    CirclePitchScale,
    CirclePitchAlignment,
    CircleStrokeWidth,
    CircleStrokeColor,
    CircleStrokeOpacity
> {};

} // namespace style
} // namespace mbgl
