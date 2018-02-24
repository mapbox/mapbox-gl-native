#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/hillshade_layer_impl.hpp>
#include <mbgl/style/layers/hillshade_layer_properties.hpp>
#include <mbgl/tile/tile_id.hpp>

namespace mbgl {

class RenderHillshadeLayer: public RenderLayer {
public:
    RenderHillshadeLayer(Immutable<style::HillshadeLayer::Impl>);
    ~RenderHillshadeLayer() final = default;

    void transition(const TransitionParameters&) override;
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;

    void render(PaintParameters&, RenderSource* src) override;

    std::unique_ptr<Bucket> createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const override;

    // Paint properties
    style::HillshadePaintProperties::Unevaluated unevaluated;
    style::HillshadePaintProperties::PossiblyEvaluated evaluated;

    const style::HillshadeLayer::Impl& impl() const;
private:
    const std::array<float, 2> getLatRange(const UnwrappedTileID& id);
    const std::array<float, 2> getLight(const PaintParameters& parameters);
};

template <>
inline bool RenderLayer::is<RenderHillshadeLayer>() const {
    return type == style::LayerType::Hillshade;
}

} // namespace mbgl
