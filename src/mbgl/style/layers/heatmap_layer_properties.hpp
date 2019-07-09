// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layer_properties.hpp>
#include <mbgl/style/layers/heatmap_layer.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/style/properties.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>

namespace mbgl {
namespace style {

struct HeatmapColor : ColorRampProperty {
};

struct HeatmapIntensity : PaintProperty<float> {
    static float defaultValue() { return 1; }
};

struct HeatmapOpacity : PaintProperty<float> {
    static float defaultValue() { return 1; }
};

struct HeatmapRadius : DataDrivenPaintProperty<float, attributes::radius, uniforms::radius> {
    static float defaultValue() { return 30; }
};

struct HeatmapWeight : DataDrivenPaintProperty<float, attributes::weight, uniforms::weight> {
    static float defaultValue() { return 1; }
};

class HeatmapPaintProperties : public Properties<
    HeatmapColor,
    HeatmapIntensity,
    HeatmapOpacity,
    HeatmapRadius,
    HeatmapWeight
> {};

class HeatmapLayerProperties final : public LayerProperties {
public:
    explicit HeatmapLayerProperties(Immutable<HeatmapLayer::Impl>);
    HeatmapLayerProperties(
        Immutable<HeatmapLayer::Impl>,
        HeatmapPaintProperties::PossiblyEvaluated);
    ~HeatmapLayerProperties() override;

    unsigned long constantsMask() const override;

    const HeatmapLayer::Impl& layerImpl() const;
    // Data members.
    HeatmapPaintProperties::PossiblyEvaluated evaluated;
};

} // namespace style
} // namespace mbgl
