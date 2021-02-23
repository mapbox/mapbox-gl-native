// clang-format off

// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layer_properties.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/style/properties.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>

namespace mbgl {
namespace style {

struct BackgroundColor : PaintProperty<Color> {
    static Color defaultValue() { return Color::black(); }
};

struct BackgroundOpacity : PaintProperty<float> {
    static float defaultValue() { return 1; }
};

struct BackgroundPattern : CrossFadedPaintProperty<expression::Image> {
    static expression::Image defaultValue() { return {}; }
};

class BackgroundPaintProperties : public Properties<
    BackgroundColor,
    BackgroundOpacity,
    BackgroundPattern
> {};

class BackgroundLayerProperties final : public LayerProperties {
public:
    explicit BackgroundLayerProperties(Immutable<BackgroundLayer::Impl>);
    BackgroundLayerProperties(
        Immutable<BackgroundLayer::Impl>,
        CrossfadeParameters,
        BackgroundPaintProperties::PossiblyEvaluated);
    ~BackgroundLayerProperties() override;

    unsigned long constantsMask() const override;

    const BackgroundLayer::Impl& layerImpl() const;
    // Data members.
    CrossfadeParameters crossfade;
    BackgroundPaintProperties::PossiblyEvaluated evaluated;
};

} // namespace style
} // namespace mbgl

// clang-format on
