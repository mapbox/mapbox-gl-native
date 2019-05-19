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
    void transition(const TransitionParameters&) override;
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;
    bool hasCrossfade() const override;

    void render(PaintParameters&) override;
    void prepare(const LayerPrepareParameters&) override;

    // Paint properties
    style::HillshadePaintProperties::Unevaluated unevaluated;
    uint8_t maxzoom = util::TERRAIN_RGB_MAXZOOM;

    const std::array<float, 2> getLatRange(const UnwrappedTileID& id);
    const std::array<float, 2> getLight(const PaintParameters& parameters);
};

} // namespace mbgl
