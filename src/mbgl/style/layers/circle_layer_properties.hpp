// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layer_properties.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/style/properties.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>

namespace mbgl {
namespace style {

struct CircleBlur : DataDrivenPaintProperty<float, attributes::blur, uniforms::blur> {
    static float defaultValue() { return 0; }
};

struct CircleColor : DataDrivenPaintProperty<Color, attributes::color, uniforms::color> {
    static Color defaultValue() { return Color::black(); }
};

struct CircleOpacity : DataDrivenPaintProperty<float, attributes::opacity, uniforms::opacity> {
    static float defaultValue() { return 1; }
};

struct CirclePitchAlignment : PaintProperty<AlignmentType> {
    static AlignmentType defaultValue() { return AlignmentType::Viewport; }
};

struct CirclePitchScale : PaintProperty<CirclePitchScaleType> {
    static CirclePitchScaleType defaultValue() { return CirclePitchScaleType::Map; }
};

struct CircleRadius : DataDrivenPaintProperty<float, attributes::radius, uniforms::radius> {
    static float defaultValue() { return 5; }
};

struct CircleStrokeColor : DataDrivenPaintProperty<Color, attributes::stroke_color, uniforms::stroke_color> {
    static Color defaultValue() { return Color::black(); }
};

struct CircleStrokeOpacity : DataDrivenPaintProperty<float, attributes::stroke_opacity, uniforms::stroke_opacity> {
    static float defaultValue() { return 1; }
};

struct CircleStrokeWidth : DataDrivenPaintProperty<float, attributes::stroke_width, uniforms::stroke_width> {
    static float defaultValue() { return 0; }
};

struct CircleTranslate : PaintProperty<std::array<float, 2>> {
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct CircleTranslateAnchor : PaintProperty<TranslateAnchorType> {
    static TranslateAnchorType defaultValue() { return TranslateAnchorType::Map; }
};

class CirclePaintProperties : public Properties<
    CircleBlur,
    CircleColor,
    CircleOpacity,
    CirclePitchAlignment,
    CirclePitchScale,
    CircleRadius,
    CircleStrokeColor,
    CircleStrokeOpacity,
    CircleStrokeWidth,
    CircleTranslate,
    CircleTranslateAnchor
> {};

class CircleLayerProperties final : public LayerProperties {
public:
    explicit CircleLayerProperties(Immutable<CircleLayer::Impl>);
    CircleLayerProperties(
        Immutable<CircleLayer::Impl>,
        CirclePaintProperties::PossiblyEvaluated);
    ~CircleLayerProperties() override;

    unsigned long constantsMask() const override;

    const CircleLayer::Impl& layerImpl() const;
    // Data members.
    CirclePaintProperties::PossiblyEvaluated evaluated;
};

} // namespace style
} // namespace mbgl
