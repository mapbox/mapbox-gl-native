// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>

namespace mbgl {
namespace style {

struct LineCap : LayoutProperty<LineCapType> {
    static LineCapType defaultValue() { return LineCapType::Butt; }
};

struct LineJoin : LayoutProperty<LineJoinType> {
    static LineJoinType defaultValue() { return LineJoinType::Miter; }
};

struct LineMiterLimit : LayoutProperty<float> {
    static float defaultValue() { return 2; }
};

struct LineRoundLimit : LayoutProperty<float> {
    static float defaultValue() { return 1; }
};

struct LineOpacity : PaintProperty<float> {
    static float defaultValue() { return 1; }
};

struct LineColor : PaintProperty<Color> {
    static Color defaultValue() { return Color::black(); }
};

struct LineTranslate : PaintProperty<std::array<float, 2>> {
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct LineTranslateAnchor : PaintProperty<TranslateAnchorType> {
    static TranslateAnchorType defaultValue() { return TranslateAnchorType::Map; }
};

struct LineWidth : PaintProperty<float> {
    static float defaultValue() { return 1; }
};

struct LineGapWidth : PaintProperty<float> {
    static float defaultValue() { return 0; }
};

struct LineOffset : PaintProperty<float> {
    static float defaultValue() { return 0; }
};

struct LineBlur : PaintProperty<float> {
    static float defaultValue() { return 0; }
};

struct LineDasharray : CrossFadedPaintProperty<std::vector<float>> {
    static std::vector<float> defaultValue() { return {  }; }
};

struct LinePattern : CrossFadedPaintProperty<std::string> {
    static std::string defaultValue() { return ""; }
};

class LineLayoutProperties : public LayoutProperties<
    LineCap,
    LineJoin,
    LineMiterLimit,
    LineRoundLimit
> {};

class LinePaintProperties : public PaintProperties<
    LineOpacity,
    LineColor,
    LineTranslate,
    LineTranslateAnchor,
    LineWidth,
    LineGapWidth,
    LineOffset,
    LineBlur,
    LineDasharray,
    LinePattern
> {};

} // namespace style
} // namespace mbgl
