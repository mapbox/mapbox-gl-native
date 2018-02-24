#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/heatmap_layer_impl.hpp>
#include <mbgl/style/layers/heatmap_layer_properties.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/offscreen_texture.hpp>

namespace mbgl {

class RenderHeatmapLayer: public RenderLayer {
public:
    RenderHeatmapLayer(Immutable<style::HeatmapLayer::Impl>);
    ~RenderHeatmapLayer() final = default;

    void transition(const TransitionParameters&) override;
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;
    void render(PaintParameters&, RenderSource*) override;

    bool queryIntersectsFeature(
            const GeometryCoordinates&,
            const GeometryTileFeature&,
            const float,
            const float,
            const float) const override;

    void updateColorRamp();

    std::unique_ptr<Bucket> createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const override;

    // Paint properties
    style::HeatmapPaintProperties::Unevaluated unevaluated;
    style::HeatmapPaintProperties::PossiblyEvaluated evaluated;

    const style::HeatmapLayer::Impl& impl() const;

    PremultipliedImage colorRamp;
    optional<OffscreenTexture> renderTexture;
    optional<gl::Texture> colorRampTexture;
};

template <>
inline bool RenderLayer::is<RenderHeatmapLayer>() const {
    return type == style::LayerType::Heatmap;
}

} // namespace mbgl
