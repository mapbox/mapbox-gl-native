#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/heatmap_layer_impl.hpp>
#include <mbgl/style/layers/heatmap_layer_properties.hpp>
#include <mbgl/gfx/texture.hpp>
#include <mbgl/gfx/offscreen_texture.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {

class RenderHeatmapLayer final : public RenderLayer {
public:
    explicit RenderHeatmapLayer(Immutable<style::HeatmapLayer::Impl>);
    ~RenderHeatmapLayer() override;

private:
    void transition(const TransitionParameters&) override;
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;
    bool hasCrossfade() const override;
    void upload(gfx::UploadPass&) override;
    void render(PaintParameters&) override;

    bool queryIntersectsFeature(const GeometryCoordinates&, const GeometryTileFeature&, const float,
                                const TransformState&, const float, const mat4&, const FeatureState&) const override;

    // Paint properties
    style::HeatmapPaintProperties::Unevaluated unevaluated;
    PremultipliedImage colorRamp;
    std::unique_ptr<gfx::OffscreenTexture> renderTexture;
    optional<gfx::Texture> colorRampTexture;

    void updateColorRamp();
};

} // namespace mbgl
