#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/hillshade_layer_impl.hpp>
#include <mbgl/style/layers/hillshade_layer_properties.hpp>
#include <mbgl/tile/tile_id.hpp>

namespace mbgl {

class RenderHillshadeLayer: public RenderLayer {
public:
    explicit RenderHillshadeLayer(Immutable<style::HillshadeLayer::Impl>);
    ~RenderHillshadeLayer() override;

private:
    LayerRenderer createRenderer() override;
    void transition(const TransitionParameters&) override;
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;
    bool hasCrossfade() const override;

    void prepare(const LayerPrepareParameters&) override;

    // Paint properties
    style::HillshadePaintProperties::Unevaluated unevaluated;
    uint8_t maxzoom = util::TERRAIN_RGB_MAXZOOM;
};

} // namespace mbgl
