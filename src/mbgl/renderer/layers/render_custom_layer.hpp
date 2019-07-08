#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/custom_layer_impl.hpp>

namespace mbgl {

class RenderCustomLayer final : public RenderLayer {
public:
    explicit RenderCustomLayer(Immutable<style::CustomLayer::Impl>);
    ~RenderCustomLayer() override;

private:
    LayerRenderer createRenderer() override;
    void transition(const TransitionParameters&) override {}
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;
    bool hasCrossfade() const override;
    void markContextDestroyed() override;
    void prepare(const LayerPrepareParameters&) override;

    bool contextDestroyed = false;
    std::shared_ptr<style::CustomLayerHost> host;
};

} // namespace mbgl
