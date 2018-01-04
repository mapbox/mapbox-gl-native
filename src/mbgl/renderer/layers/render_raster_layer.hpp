#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/raster_layer_impl.hpp>
#include <mbgl/style/layers/raster_layer_properties.hpp>

namespace mbgl {

class RenderRasterLayer: public RenderLayer {
public:
    RenderRasterLayer(Immutable<style::RasterLayer::Impl>);
    ~RenderRasterLayer() final = default;

    void transition(const TransitionParameters&) override;
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;

    void render(PaintParameters&, RenderSource*) override;

    std::unique_ptr<Bucket> createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const override;

    // Paint properties
    style::RasterPaintProperties::Unevaluated unevaluated;
    style::RasterPaintProperties::PossiblyEvaluated evaluated;

    const style::RasterLayer::Impl& impl() const;
};

template <>
inline bool RenderLayer::is<RenderRasterLayer>() const {
    return type == style::LayerType::Raster;
}

} // namespace mbgl
