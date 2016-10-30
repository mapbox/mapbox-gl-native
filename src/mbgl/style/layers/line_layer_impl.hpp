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

    void cascade(const CascadeParameters&) override;
    bool evaluate(const PropertyEvaluationParameters&) override;

    std::unique_ptr<Bucket> createBucket(BucketParameters&) const override;

    float getQueryRadius() const override;
    bool queryIntersectsGeometry(
            const GeometryCoordinates& queryGeometry,
            const GeometryCollection& geometry,
            const float bearing,
            const float pixelsToTileUnits) const override;

    LineLayoutProperties layout;
    LinePaintProperties paint;

    // Special case
    float dashLineWidth = 1;

private:
    float getLineWidth() const;
};

} // namespace style
} // namespace mbgl
