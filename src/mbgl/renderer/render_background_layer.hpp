#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/background_layer_properties.hpp>

namespace mbgl {

class RenderBackgroundLayer: public RenderLayer {
public:

    RenderBackgroundLayer(const style::BackgroundLayer::Impl&);
    ~RenderBackgroundLayer() final = default;

    std::unique_ptr<RenderLayer> clone() const override;

    void cascade(const CascadeParameters&) override;
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;

    std::unique_ptr<Bucket> createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const override;

    // Paint properties
    style::BackgroundPaintProperties::Unevaluated unevaluated;
    style::BackgroundPaintProperties::Evaluated evaluated;

    const style::BackgroundLayer::Impl* const impl;
};

template <>
inline bool RenderLayer::is<RenderBackgroundLayer>() const {
    return type == style::LayerType::Background;
}

}
