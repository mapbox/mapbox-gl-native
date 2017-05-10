#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/line_layer_impl.hpp>
#include <mbgl/style/layers/line_layer_properties.hpp>

namespace mbgl {

class RenderLineLayer: public RenderLayer {
public:
    RenderLineLayer(Immutable<style::LineLayer::Impl>);
    ~RenderLineLayer() final = default;

    void transition(const TransitionParameters&) override;
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;

    bool queryIntersectsFeature(
            const GeometryCoordinates&,
            const GeometryTileFeature&,
            const float,
            const float,
            const float) const override;

    std::unique_ptr<Bucket> createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const override;

    // Paint properties
    style::LinePaintProperties::Unevaluated unevaluated;
    style::LinePaintProperties::PossiblyEvaluated evaluated;

    const style::LineLayer::Impl& impl() const;

    // Special case
    float dashLineWidth = 1;

private:
    float getLineWidth(const GeometryTileFeature&, const float) const;
};

template <>
inline bool RenderLayer::is<RenderLineLayer>() const {
    return type == style::LayerType::Line;
}

} // namespace mbgl
