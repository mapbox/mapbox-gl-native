// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layer_properties.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
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

struct FillOpacity : DataDrivenPaintProperty<float, attributes::opacity, uniforms::opacity> {
    static float defaultValue() { return 1; }
};

struct FillColor : DataDrivenPaintProperty<Color, attributes::color, uniforms::color> {
    static Color defaultValue() { return Color::black(); }
};

struct FillOutlineColor : DataDrivenPaintProperty<Color, attributes::outline_color, uniforms::outline_color> {
    static Color defaultValue() { return {}; }
};

struct FillTranslate : PaintProperty<std::array<float, 2>> {
    static std::array<float, 2> defaultValue() { return {{ 0, 0 }}; }
};

struct FillTranslateAnchor : PaintProperty<TranslateAnchorType> {
    static TranslateAnchorType defaultValue() { return TranslateAnchorType::Map; }
};

struct FillPattern : CrossFadedDataDrivenPaintProperty<std::string, attributes::pattern_to, uniforms::pattern_to, attributes::pattern_from, uniforms::pattern_from> {
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

class FillLayerProperties final : public LayerProperties {
public:
    explicit FillLayerProperties(Immutable<FillLayer::Impl>);
    FillLayerProperties(
        Immutable<FillLayer::Impl>,
        CrossfadeParameters,
        FillPaintProperties::PossiblyEvaluated);
    ~FillLayerProperties() override;

    const FillLayer::Impl& layerImpl() const;
    // Data members.
    CrossfadeParameters crossfade;
    FillPaintProperties::PossiblyEvaluated evaluated;
};

} // namespace style
} // namespace mbgl
