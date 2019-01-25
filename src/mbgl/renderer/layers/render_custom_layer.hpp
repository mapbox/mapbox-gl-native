#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/custom_layer_impl.hpp>

namespace mbgl {

class RenderCustomLayer: public RenderLayer {
public:
    RenderCustomLayer(Immutable<style::CustomLayer::Impl>);
    ~RenderCustomLayer() final;

    void transition(const TransitionParameters&) final {}
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;
    bool hasCrossfade() const override;
    void markContextDestroyed() final;

    void render(PaintParameters&, RenderSource*) final;

    const style::CustomLayer::Impl& impl() const;

private:
    bool contextDestroyed = false;
    std::shared_ptr<style::CustomLayerHost> host;
};

} // namespace mbgl
