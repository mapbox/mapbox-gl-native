#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/raster_layer_properties.hpp>

namespace mbgl {

class RenderRasterLayer: public RenderLayer {
public:

    RenderRasterLayer(const style::RasterLayer::Impl&);
    ~RenderRasterLayer() final = default;

    std::unique_ptr<RenderLayer> clone() const override;

    void cascade(const CascadeParameters&) override;
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;

    std::unique_ptr<Bucket> createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const override;

    // Paint properties
    style::RasterPaintProperties::Unevaluated unevaluated;
    style::RasterPaintProperties::Evaluated evaluated;

    const style::RasterLayer::Impl* const impl;
};

template <>
inline bool RenderLayer::is<RenderRasterLayer>() const {
    return type == style::LayerType::Raster;
}

} // namespace mbgl
