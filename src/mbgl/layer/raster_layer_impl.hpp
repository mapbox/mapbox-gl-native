#pragma once

#include <mbgl/layer/layer_impl.hpp>
#include <mbgl/layer/raster_layer.hpp>
#include <mbgl/layer/raster_layer_properties.hpp>

namespace mbgl {

class RasterLayer::Impl : public Layer::Impl {
public:
    std::unique_ptr<Layer> clone() const override;

    void parseLayout(const JSValue&) override {};
    void parsePaints(const JSValue&) override;

    void cascade(const StyleCascadeParameters&) override;
    bool recalculate(const StyleCalculationParameters&) override;

    std::unique_ptr<Bucket> createBucket(StyleBucketParameters&) const override;

    RasterPaintProperties paint;
};

} // namespace mbgl
