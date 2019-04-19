#pragma once

#include <mbgl/renderer/render_layer.hpp>
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

    void render(PaintParameters&, RenderSource*) override;

    // Paint properties
    style::RasterPaintProperties::Unevaluated unevaluated;
};

} // namespace mbgl
