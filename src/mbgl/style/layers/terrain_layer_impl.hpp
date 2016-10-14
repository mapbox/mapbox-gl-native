#pragma once

#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/terrain_layer.hpp>
#include <mbgl/style/layers/terrain_layer_properties.hpp>

namespace mbgl {
namespace style {

class TerrainLayer::Impl : public Layer::Impl {
public:
    std::unique_ptr<Layer> clone() const override;
    std::unique_ptr<Layer> cloneRef(const std::string& id) const override;

    void cascade(const CascadeParameters&) override;
    bool recalculate(const CalculationParameters&) override;

    std::unique_ptr<Bucket> createBucket(BucketParameters&) const override;

    TerrainPaintProperties paint;
};

} // namespace style
} // namespace mbgl
