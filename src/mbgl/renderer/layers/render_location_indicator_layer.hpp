#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/location_indicator_layer.hpp>
#include <mbgl/style/layers/location_indicator_layer_impl.hpp>
#include <mbgl/style/layers/location_indicator_layer_properties.hpp>

namespace mbgl {
class RenderLocationIndicatorImpl;
class RenderLocationIndicatorLayer final : public RenderLayer {
public:
    explicit RenderLocationIndicatorLayer(Immutable<style::LocationIndicatorLayer::Impl>);
    ~RenderLocationIndicatorLayer() override;

private:
    void transition(const TransitionParameters &) override;
    void evaluate(const PropertyEvaluationParameters &) override;
    bool hasTransition() const override;
    bool hasCrossfade() const override;
    void markContextDestroyed() override;
    void prepare(const LayerPrepareParameters &) override;

    void render(PaintParameters &) override;
    void populateDynamicRenderFeatureIndex(DynamicFeatureIndex &) const override;

    bool contextDestroyed = false;
    std::unique_ptr<RenderLocationIndicatorImpl> renderImpl;
    style::LocationIndicatorPaintProperties::Unevaluated unevaluated;
};

} // namespace mbgl
