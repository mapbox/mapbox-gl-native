#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/circle_layer_impl.hpp>
#include <mbgl/style/layers/circle_layer_properties.hpp>

namespace mbgl {

class RenderCircleLayer: public RenderLayer {
public:
    RenderCircleLayer(Immutable<style::CircleLayer::Impl>);
    ~RenderCircleLayer() final = default;

    void transition(const TransitionParameters&) override;
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;
    void render(PaintParameters&, RenderSource*) override;

    bool queryIntersectsFeature(
            const GeometryCoordinates&,
            const GeometryTileFeature&,
            const float,
            const float,
            const float) const override;

    std::unique_ptr<Bucket> createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const override;

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
