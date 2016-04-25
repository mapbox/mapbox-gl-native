#pragma once

#include <mbgl/layer/layer_impl.hpp>
#include <mbgl/layer/line_layer.hpp>
#include <mbgl/layer/line_layer_properties.hpp>

namespace mbgl {

class LineLayer::Impl : public Layer::Impl {
public:
    std::unique_ptr<Layer> clone() const override;

    void parseLayout(const JSValue&) override;
    void parsePaints(const JSValue&) override;

    void cascade(const StyleCascadeParameters&) override;
    bool recalculate(const StyleCalculationParameters&) override;

    std::unique_ptr<Bucket> createBucket(StyleBucketParameters&) const override;

    float getQueryRadius() const override;
    bool queryIntersectsGeometry(
            const GeometryCollection& queryGeometry,
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

} // namespace mbgl
