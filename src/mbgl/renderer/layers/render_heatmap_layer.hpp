#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/heatmap_layer_impl.hpp>
#include <mbgl/style/layers/heatmap_layer_properties.hpp>
#include <mbgl/gfx/offscreen_texture.hpp>

namespace mbgl {

class ColorRampRenderData;

class RenderHeatmapLayer final : public RenderLayer {
public:
    struct RenderData {
        std::unique_ptr<gfx::OffscreenTexture> texture = nullptr;
    };
    explicit RenderHeatmapLayer(Immutable<style::HeatmapLayer::Impl>);
    ~RenderHeatmapLayer() override;

private:
    LayerRenderer createRenderer() override;
    LayerUploader createUploader() override;
    void transition(const TransitionParameters&) override;
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;
    bool hasCrossfade() const override;
    void render(PaintParameters&) override {}

    bool queryIntersectsFeature(
            const GeometryCoordinates&,
            const GeometryTileFeature&,
            const float,
            const TransformState&,
            const float,
            const mat4&) const override;

    // Paint properties
    style::HeatmapPaintProperties::Unevaluated unevaluated;
    // Data shared between renderItems.
    std::shared_ptr<RenderData> sharedRenderData;

    void updateColorRamp();
    std::shared_ptr<ColorRampRenderData> colorRamp;
};

} // namespace mbgl
