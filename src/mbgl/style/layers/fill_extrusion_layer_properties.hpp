// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layer_properties.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/style/properties.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>

namespace mbgl {
namespace style {

struct FillExtrusionBase : DataDrivenPaintProperty<float, attributes::base, uniforms::base> {
    static float defaultValue() { return 0; }
};

struct FillExtrusionColor : DataDrivenPaintProperty<Color, attributes::color, uniforms::color> {
    static Color defaultValue() { return Color::black(); }
};

struct FillExtrusionHeight : DataDrivenPaintProperty<float, attributes::height, uniforms::height> {
    static float defaultValue() { return 0; }
};

struct FillExtrusionOpacity : PaintProperty<float> {
    static float defaultValue() { return 1; }
};

struct FillExtrusionPattern : CrossFadedDataDrivenPaintProperty<std::string, attributes::pattern_to, uniforms::pattern_to, attributes::pattern_from, uniforms::pattern_from> {
    static std::string defaultValue() { return ""; }
};

struct FillExtrusionTranslate : PaintProperty<std::array<float, 2>> {
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct FillExtrusionTranslateAnchor : PaintProperty<TranslateAnchorType> {
    static TranslateAnchorType defaultValue() { return TranslateAnchorType::Map; }
};

struct FillExtrusionVerticalGradient : PaintProperty<bool> {
    static bool defaultValue() { return true; }
};

class FillExtrusionPaintProperties : public Properties<
    FillExtrusionBase,
    FillExtrusionColor,
    FillExtrusionHeight,
    FillExtrusionOpacity,
    FillExtrusionPattern,
    FillExtrusionTranslate,
    FillExtrusionTranslateAnchor,
    FillExtrusionVerticalGradient
> {};

class FillExtrusionLayerProperties final : public LayerProperties {
public:
    explicit FillExtrusionLayerProperties(Immutable<FillExtrusionLayer::Impl>);
    FillExtrusionLayerProperties(
        Immutable<FillExtrusionLayer::Impl>,
        CrossfadeParameters,
        FillExtrusionPaintProperties::PossiblyEvaluated);
    ~FillExtrusionLayerProperties() override;

    unsigned long constantsMask() const override;

    const FillExtrusionLayer::Impl& layerImpl() const;
    // Data members.
    CrossfadeParameters crossfade;
    FillExtrusionPaintProperties::PossiblyEvaluated evaluated;
};

} // namespace style
} // namespace mbgl
