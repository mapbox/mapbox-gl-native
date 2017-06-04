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

struct FillAntialias : PaintProperty<bool> {
    static bool defaultValue() { return true; }
};

struct FillOpacity : DataDrivenPaintProperty<float, attributes::a_opacity, uniforms::u_opacity> {
    static float defaultValue() { return 1; }
};

struct FillColor : DataDrivenPaintProperty<Color, attributes::a_color, uniforms::u_color> {
    static Color defaultValue() { return Color::black(); }
};

struct FillOutlineColor : DataDrivenPaintProperty<Color, attributes::a_outline_color, uniforms::u_outline_color> {
    static Color defaultValue() { return {}; }
};

struct FillTranslate : PaintProperty<std::array<float, 2>> {
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct FillTranslateAnchor : PaintProperty<TranslateAnchorType> {
    static TranslateAnchorType defaultValue() { return TranslateAnchorType::Map; }
};

struct FillPattern : CrossFadedPaintProperty<std::string> {
    static std::string defaultValue() { return ""; }
};

class FillPaintProperties : public Properties<
    FillAntialias,
    FillOpacity,
    FillColor,
    FillOutlineColor,
    FillTranslate,
    FillTranslateAnchor,
    FillPattern
> {};

} // namespace style
} // namespace mbgl
