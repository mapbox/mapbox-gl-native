#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_properties.hpp>

namespace mbgl {

class RenderFillExtrusionLayer: public RenderLayer {
public:

    RenderFillExtrusionLayer(const style::FillExtrusionLayer::Impl&);
    ~RenderFillExtrusionLayer() final = default;

    std::unique_ptr<RenderLayer> clone() const override;

    void cascade(const style::CascadeParameters&) override;
    bool evaluate(const style::PropertyEvaluationParameters&) override;

    std::unique_ptr<Bucket> createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const override;

    // Paint properties
    style::FillExtrusionPaintProperties::Unevaluated unevaluated;
    style::FillExtrusionPaintProperties::Evaluated evaluated;
};

template <>
inline bool RenderLayer::is<RenderFillExtrusionLayer>() const {
    return type == style::LayerType::FillExtrusion;
}

} // namespace mbgl
