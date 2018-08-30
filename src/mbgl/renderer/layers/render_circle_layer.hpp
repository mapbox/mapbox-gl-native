#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/circle_layer_impl.hpp>
#include <mbgl/style/layers/circle_layer_properties.hpp>
#include <mbgl/layout/circle_layout.hpp>

namespace mbgl {

class RenderCircleLayer: public RenderLayer {
public:
    RenderCircleLayer(Immutable<style::CircleLayer::Impl>);
    ~RenderCircleLayer() final = default;

    using StyleLayerImpl = style::CircleLayer::Impl;

    void transition(const TransitionParameters&) override;
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;
    bool hasCrossfade() const override;
    void render(PaintParameters&, RenderSource*) override;

    style::CirclePaintProperties::PossiblyEvaluated paintProperties() const;

    bool queryIntersectsFeature(
            const GeometryCoordinates&,
            const GeometryTileFeature&,
            const float,
            const TransformState&,
            const float,
            const mat4&) const override;

    std::unique_ptr<Layout> createLayout(const BucketParameters&, const std::vector<const RenderLayer*>&,
                                         std::unique_ptr<GeometryTileLayer>, GlyphDependencies&,
                                         ImageDependencies&) const override;
    // Paint properties
    style::CirclePaintProperties::Unevaluated unevaluated;
    style::CirclePaintProperties::PossiblyEvaluated evaluated;

    const style::CircleLayer::Impl& impl() const;
};

template <>
inline bool RenderLayer::is<RenderCircleLayer>() const {
    return type == style::LayerType::Circle;
}

} // namespace mbgl
