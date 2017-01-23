// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>

namespace mbgl {
namespace style {

struct FillExtrusionOpacity : PaintProperty<float> {
    static float defaultValue() { return 1; }
};

struct FillExtrusionColor : PaintProperty<Color> {
    static Color defaultValue() { return Color::black(); }
};

struct FillExtrusionTranslate : PaintProperty<std::array<float, 2>> {
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct FillExtrusionTranslateAnchor : PaintProperty<TranslateAnchorType> {
    static TranslateAnchorType defaultValue() { return TranslateAnchorType::Map; }
};

struct FillExtrusionPattern : CrossFadedPaintProperty<std::string> {
    static std::string defaultValue() { return ""; }
};

struct FillExtrusionHeight : PaintProperty<float> {
    static float defaultValue() { return 0; }
};

struct FillExtrusionBase : PaintProperty<float> {
    static float defaultValue() { return 0; }
};

class FillExtrusionPaintProperties : public PaintProperties<
    FillExtrusionOpacity,
    FillExtrusionColor,
    FillExtrusionTranslate,
    FillExtrusionTranslateAnchor,
    FillExtrusionPattern,
    FillExtrusionHeight,
    FillExtrusionBase
> {};

} // namespace style
} // namespace mbgl
