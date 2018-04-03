#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/fill_layer_impl.hpp>
#include <mbgl/style/layers/fill_layer_properties.hpp>

namespace mbgl {

class RenderFillLayer: public RenderLayer {
public:
    RenderFillLayer(Immutable<style::FillLayer::Impl>);
    ~RenderFillLayer() final = default;

    void transition(const TransitionParameters&) override;
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;
    void render(PaintParameters&, RenderSource*) override;

    bool queryIntersectsFeature(
            const GeometryCoordinates&,
            const GeometryTileFeature&,
            const float,
            const TransformState&,
            const float,
            const mat4&) const override;

    std::unique_ptr<Bucket> createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const override;

    // Paint properties
    style::FillPaintProperties::Unevaluated unevaluated;
    style::FillPaintProperties::PossiblyEvaluated evaluated;

    const style::FillLayer::Impl& impl() const;
};

template <>
inline bool RenderLayer::is<RenderFillLayer>() const {
    return type == style::LayerType::Fill;
}

} // namespace mbgl
