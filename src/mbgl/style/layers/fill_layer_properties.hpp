// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>

namespace mbgl {
namespace style {

struct FillAntialias : PaintProperty<bool> {
    static bool defaultValue() { return true; }
};

struct FillOpacity : PaintProperty<float> {
    static float defaultValue() { return 1; }
};

struct FillColor : PaintProperty<Color> {
    static Color defaultValue() { return Color::black(); }
};

struct FillOutlineColor : PaintProperty<Color> {
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

class FillPaintProperties : public PaintProperties<
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
