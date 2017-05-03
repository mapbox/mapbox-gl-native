#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/fill_layer_properties.hpp>

namespace mbgl {

class RenderFillLayer: public RenderLayer {
public:

    RenderFillLayer(const style::FillLayer::Impl&);
    ~RenderFillLayer() final = default;

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
    style::FillPaintProperties::Unevaluated unevaluated;
    style::FillPaintProperties::Evaluated evaluated;

    const style::FillLayer::Impl* const impl;
};

template <>
inline bool RenderLayer::is<RenderFillLayer>() const {
    return type == style::LayerType::Fill;
}

} // namespace mbgl
