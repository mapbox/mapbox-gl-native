#pragma once

#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/fill_layer_properties.hpp>

namespace mbgl {
namespace style {

class FillLayer::Impl : public Layer::Impl {
public:
    std::unique_ptr<Layer> clone() const override;
    std::unique_ptr<Layer> cloneRef(const std::string& id) const override;

    void cascade(const CascadeParameters&) override;
    bool evaluate(const PropertyEvaluationParameters&) override;

    std::unique_ptr<Bucket> createBucket(BucketParameters&, const GeometryTileLayer&) const override;

    float getQueryRadius() const override;
    bool queryIntersectsGeometry(
            const GeometryCoordinates& queryGeometry,
            const GeometryCollection& geometry,
            const float bearing,
            const float pixelsToTileUnits) const override;

    FillPaintProperties paint;
};

} // namespace style
} // namespace mbgl
