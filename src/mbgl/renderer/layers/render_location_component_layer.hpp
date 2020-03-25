#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/location_component_layer.hpp>
#include <mbgl/style/layers/location_component_layer_impl.hpp>
#include <mbgl/style/layers/location_component_layer_properties.hpp>

namespace mbgl {
class RenderLocationComponentImpl;
class RenderLocationComponentLayer final : public RenderLayer {
public:
    explicit RenderLocationComponentLayer(Immutable<style::LocationComponentLayer::Impl>);
    ~RenderLocationComponentLayer() override;

private:
    void transition(const TransitionParameters &) override;
    void evaluate(const PropertyEvaluationParameters &) override;
    bool hasTransition() const override;
    bool hasCrossfade() const override;
    void markContextDestroyed() override;
    void prepare(const LayerPrepareParameters &) override;

    void render(PaintParameters &) override;

    bool contextDestroyed = false;
    RenderLocationComponentImpl *renderImpl = nullptr;
    style::LocationComponentPaintProperties::Unevaluated unevaluated;
};

} // namespace mbgl
