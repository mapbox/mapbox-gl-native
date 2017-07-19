#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_properties.hpp>

namespace mbgl {

class RenderFillExtrusionLayer: public RenderLayer {
public:
    RenderFillExtrusionLayer(Immutable<style::FillExtrusionLayer::Impl>);
    ~RenderFillExtrusionLayer() final = default;

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
    style::FillExtrusionPaintProperties::Unevaluated unevaluated;
    style::FillExtrusionPaintProperties::PossiblyEvaluated evaluated;

    const style::FillExtrusionLayer::Impl& impl() const;
};

template <>
inline bool RenderLayer::is<RenderFillExtrusionLayer>() const {
    return type == style::LayerType::FillExtrusion;
}

} // namespace mbgl
