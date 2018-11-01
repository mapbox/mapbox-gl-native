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
    bool hasCrossfade() const override;
    void render(PaintParameters&, RenderSource*) override;
    void update() final;

    bool queryIntersectsFeature(
            const GeometryCoordinates&,
            const GeometryTileFeature&,
            const float,
            const TransformState&,
            const float,
            const mat4&) const override;

    std::unique_ptr<Bucket> createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const override;

    // Paint properties
    style::HeatmapPaintProperties::Unevaluated unevaluated;
    style::HeatmapPaintProperties::PossiblyEvaluated evaluated;

    const style::HeatmapLayer::Impl& impl() const;

    PremultipliedImage colorRamp;
    optional<OffscreenTexture> renderTexture;
    optional<gl::Texture> colorRampTexture;

private:
    void updateColorRamp();
};

inline const RenderHeatmapLayer* toRenderHeatmapLayer(const RenderLayer* layer) {
    return static_cast<const RenderHeatmapLayer*>(layer);
}

} // namespace mbgl
