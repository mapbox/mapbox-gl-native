// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layer_properties.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/style/properties.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>

namespace mbgl {
namespace style {

struct LineCap : LayoutProperty<LineCapType> {
    static constexpr const char *name() { return "line-cap"; }
    static LineCapType defaultValue() { return LineCapType::Butt; }
};

struct LineJoin : DataDrivenLayoutProperty<LineJoinType> {
    static constexpr const char *name() { return "line-join"; }
    static LineJoinType defaultValue() { return LineJoinType::Miter; }
};

struct LineMiterLimit : LayoutProperty<float> {
    static constexpr const char *name() { return "line-miter-limit"; }
    static float defaultValue() { return 2; }
};

struct LineRoundLimit : LayoutProperty<float> {
    static constexpr const char *name() { return "line-round-limit"; }
    static float defaultValue() { return 1; }
};

struct LineBlur : DataDrivenPaintProperty<float, attributes::blur, uniforms::blur> {
    static float defaultValue() { return 0; }
};

struct LineColor : DataDrivenPaintProperty<Color, attributes::color, uniforms::color> {
    static Color defaultValue() { return Color::black(); }
};

struct LineDasharray : CrossFadedPaintProperty<std::vector<float>> {
    static std::vector<float> defaultValue() { return {  }; }
};

struct LineFloorWidth : DataDrivenPaintProperty<float, attributes::floorwidth, uniforms::floorwidth> {
    static float defaultValue() { return 1; }
    using EvaluatorType = DataDrivenPropertyEvaluator<float, true>;
};

struct LineGapWidth : DataDrivenPaintProperty<float, attributes::gapwidth, uniforms::gapwidth> {
    static float defaultValue() { return 0; }
};

struct LineGradient : ColorRampProperty {
};

struct LineOffset : DataDrivenPaintProperty<float, attributes::offset, uniforms::offset> {
    static float defaultValue() { return 0; }
};

struct LineOpacity : DataDrivenPaintProperty<float, attributes::opacity, uniforms::opacity> {
    static float defaultValue() { return 1; }
};

struct LinePattern : CrossFadedDataDrivenPaintProperty<std::string, attributes::pattern_to, uniforms::pattern_to, attributes::pattern_from, uniforms::pattern_from> {
    static std::string defaultValue() { return ""; }
};

struct LineTranslate : PaintProperty<std::array<float, 2>> {
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct LineTranslateAnchor : PaintProperty<TranslateAnchorType> {
    static TranslateAnchorType defaultValue() { return TranslateAnchorType::Map; }
};

struct LineWidth : DataDrivenPaintProperty<float, attributes::width, uniforms::width> {
    static float defaultValue() { return 1; }
};

class LineLayoutProperties : public Properties<
    LineCap,
    LineJoin,
    LineMiterLimit,
    LineRoundLimit
> {};

class LinePaintProperties : public Properties<
    LineBlur,
    LineColor,
    LineDasharray,
    LineFloorWidth,
    LineGapWidth,
    LineGradient,
    LineOffset,
    LineOpacity,
    LinePattern,
    LineTranslate,
    LineTranslateAnchor,
    LineWidth
> {};

class LineLayerProperties final : public LayerProperties {
public:
    explicit LineLayerProperties(Immutable<LineLayer::Impl>);
    LineLayerProperties(
        Immutable<LineLayer::Impl>,
        CrossfadeParameters,
        LinePaintProperties::PossiblyEvaluated);
    ~LineLayerProperties() override;

    unsigned long constantsMask() const override;

    const LineLayer::Impl& layerImpl() const;
    // Data members.
    CrossfadeParameters crossfade;
    LinePaintProperties::PossiblyEvaluated evaluated;
};

} // namespace style
} // namespace mbgl
