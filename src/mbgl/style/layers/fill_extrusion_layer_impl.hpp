#pragma once

#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_properties.hpp>

namespace mbgl {
namespace style {

class FillExtrusionLayer::Impl : public Layer::Impl {
public:
    std::unique_ptr<Layer> clone() const override;
    std::unique_ptr<Layer> cloneRef(const std::string& id) const override;
    void stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const override;

    void cascade(const CascadeParameters&) override;
    bool evaluate(const PropertyEvaluationParameters&) override;

    std::unique_ptr<Bucket> createBucket(const BucketParameters&,
                                         const std::vector<const Layer*>&) const override;

    float getQueryRadius() const override;
    bool queryIntersectsGeometry(const GeometryCoordinates& queryGeometry,
                                 const GeometryCollection& geometry,
                                 const float bearing,
                                 const float pixelsToTileUnits) const override;

    FillExtrusionPaintProperties paint;
};

} // namespace style
} // namespace mbgl
