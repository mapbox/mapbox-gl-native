#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/line_layer_properties.hpp>

namespace mbgl {

class RenderLineLayer: public RenderLayer {
public:

    RenderLineLayer(const style::LineLayer::Impl&);
    ~RenderLineLayer() final = default;

    std::unique_ptr<RenderLayer> clone() const override;

    void cascade(const CascadeParameters&) override;
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;

    bool queryIntersectsFeature(
            const GeometryCoordinates&,
            const GeometryTileFeature&,
            const float,
            const float,
            const float) const override;

    std::unique_ptr<Bucket> createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const override;

    // Paint properties
    style::LinePaintProperties::Unevaluated unevaluated;
    style::LinePaintProperties::Evaluated evaluated;

    const style::LineLayer::Impl* const impl;

    // Special case
    float dashLineWidth = 1;

private:
    float getLineWidth(const GeometryTileFeature&, const float) const;

};

template <>
inline bool RenderLayer::is<RenderLineLayer>() const {
    return type == style::LayerType::Line;
}

} // namespace mbgl
