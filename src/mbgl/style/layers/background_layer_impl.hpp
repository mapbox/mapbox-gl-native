#pragma once

#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/background_layer_properties.hpp>

namespace mbgl {
namespace style {

class BackgroundLayer::Impl : public Layer::Impl {
public:
    std::unique_ptr<Layer> clone() const override;
    std::unique_ptr<Layer> cloneRef(const std::string& id) const override;

    void cascade(const CascadeParameters&) override;
    bool evaluate(const PropertyEvaluationParameters&) override;

    std::unique_ptr<Bucket> createBucket(BucketParameters&, const GeometryTileLayer&) const override;

    BackgroundPaintProperties paint;
};

} // namespace style
} // namespace mbgl
