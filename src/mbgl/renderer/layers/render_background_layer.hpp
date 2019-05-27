#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>
#include <mbgl/style/layers/background_layer_properties.hpp>

namespace mbgl {

class RenderBackgroundLayer final : public RenderLayer {
public:
    explicit RenderBackgroundLayer(Immutable<style::BackgroundLayer::Impl>);
    ~RenderBackgroundLayer() override;

private:
    void transition(const TransitionParameters&) override;
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;
    bool hasCrossfade() const override;
    optional<Color> getSolidBackground() const override;
    void render(PaintParameters&) override;
    void prepare(const LayerPrepareParameters&) override;

    // Paint properties
    style::BackgroundPaintProperties::Unevaluated unevaluated;
};

} // namespace mbgl
