#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/circle_layer_properties.hpp>

namespace mbgl {

class RenderCircleLayer: public RenderLayer {
public:

    RenderCircleLayer(const style::CircleLayer::Impl&);
    ~RenderCircleLayer() final = default;

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
    style::CirclePaintProperties::Unevaluated unevaluated;
    style::CirclePaintProperties::Evaluated evaluated;

    const style::CircleLayer::Impl* const impl;
};

template <>
inline bool RenderLayer::is<RenderCircleLayer>() const {
    return type == style::LayerType::Circle;
}

}
