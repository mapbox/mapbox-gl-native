#pragma once

#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/line_layer_properties.hpp>

namespace mbgl {
namespace style {

class LineLayer::Impl : public Layer::Impl {
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

    LineLayoutProperties layout;
    LinePaintProperties paint;

    // Special case
    float dashLineWidth = 1;

private:
    float getLineWidth(const GeometryTileFeature&, const float) const;
};

} // namespace style
} // namespace mbgl
