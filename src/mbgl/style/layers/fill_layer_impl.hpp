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
    void stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const override;

    void cascade(const CascadeParameters&) override;
    bool evaluate(const PropertyEvaluationParameters&) override;

    std::unique_ptr<Bucket> createBucket(const BucketParameters&, const std::vector<const Layer*>&) const override;

    bool queryIntersectsFeature(
            const GeometryCoordinates&,
            const GeometryTileFeature&,
            const float,
            const float,
            const float) const override;

    FillPaintProperties paint;
};

} // namespace style
} // namespace mbgl
