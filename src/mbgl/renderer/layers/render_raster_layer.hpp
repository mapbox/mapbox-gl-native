#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/renderer/sources/render_image_source.hpp>
#include <mbgl/style/layers/raster_layer_impl.hpp>
#include <mbgl/style/layers/raster_layer_properties.hpp>

namespace mbgl {

class RenderRasterLayer final : public RenderLayer {
public:
    explicit RenderRasterLayer(Immutable<style::RasterLayer::Impl>);
    ~RenderRasterLayer() override;

private:
    void transition(const TransitionParameters&) override;
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;
    bool hasCrossfade() const override;
    void prepare(const LayerPrepareParameters&) override;
    void render(PaintParameters&) override;

    // Paint properties
    style::RasterPaintProperties::Unevaluated unevaluated;
    optional<ImageLayerRenderData> imageData;
};

} // namespace mbgl
