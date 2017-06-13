// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/style/properties.hpp>
#include <mbgl/programs/attributes.hpp>

namespace mbgl {
namespace style {

struct LineCap : LayoutProperty<LineCapType> {
    static constexpr const char * key = "line-cap";
    static LineCapType defaultValue() { return LineCapType::Butt; }
};

struct LineJoin : LayoutProperty<LineJoinType> {
    static constexpr const char * key = "line-join";
    static LineJoinType defaultValue() { return LineJoinType::Miter; }
};

struct LineMiterLimit : LayoutProperty<float> {
    static constexpr const char * key = "line-miter-limit";
    static float defaultValue() { return 2; }
};

struct LineRoundLimit : LayoutProperty<float> {
    static constexpr const char * key = "line-round-limit";
    static float defaultValue() { return 1; }
};

struct LineOpacity : DataDrivenPaintProperty<float, attributes::a_opacity> {
    static float defaultValue() { return 1; }
};

struct LineColor : DataDrivenPaintProperty<Color, attributes::a_color> {
    static Color defaultValue() { return Color::black(); }
};

struct LineTranslate : PaintProperty<std::array<float, 2>> {
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct LineTranslateAnchor : PaintProperty<TranslateAnchorType> {
    static TranslateAnchorType defaultValue() { return TranslateAnchorType::Map; }
};

struct LineWidth : DataDrivenPaintProperty<float, attributes::a_width> {
    static float defaultValue() { return 1; }
};

struct LineFloorwidth : DataDrivenPaintProperty<float, attributes::a_floorwidth> {
    static float defaultValue() { return 1; }
};

struct LineGapWidth : DataDrivenPaintProperty<float, attributes::a_gap_width> {
    static float defaultValue() { return 0; }
};

struct LineOffset : DataDrivenPaintProperty<float, attributes::a_offset<1>> {
    static float defaultValue() { return 0; }
};

struct LineBlur : DataDrivenPaintProperty<float, attributes::a_blur> {
    static float defaultValue() { return 0; }
};

struct LineDasharray : CrossFadedPaintProperty<std::vector<float>> {
    static std::vector<float> defaultValue() { return {  }; }
};

struct LinePattern : CrossFadedPaintProperty<std::string> {
    static std::string defaultValue() { return ""; }
};

class LineLayoutProperties : public Properties<
    LineCap,
    LineJoin,
    LineMiterLimit,
    LineRoundLimit
> {};

class LinePaintProperties : public Properties<
    LineOpacity,
    LineColor,
    LineTranslate,
    LineTranslateAnchor,
    LineWidth,
    LineFloorwidth,
    LineGapWidth,
    LineOffset,
    LineBlur,
    LineDasharray,
    LinePattern
> {};

} // namespace style
} // namespace mbgl
