#pragma once

#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/raster_layer_properties.hpp>

namespace mbgl {
namespace style {

class RasterLayer::Impl : public Layer::Impl {
public:
    std::unique_ptr<Layer> clone() const override;

    void parseLayout(const JSValue&) override {};
    void parsePaints(const JSValue&) override;

    void cascade(const CascadeParameters&) override;
    bool recalculate(const CalculationParameters&) override;

    std::unique_ptr<Bucket> createBucket(BucketParameters&) const override;

    RasterPaintProperties paint;
};

} // namespace style
} // namespace mbgl
