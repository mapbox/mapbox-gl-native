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

    std::unique_ptr<Bucket> createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const final;
    void render(PaintParameters&, RenderSource*) final;

    const style::CustomLayer::Impl& impl() const;

    void markContextDestroyed() {
        contextDestroyed = true;
    };

private:
    bool initialized = false;
    bool contextDestroyed = false;
    void * context = nullptr;
};

template <>
inline bool RenderLayer::is<RenderCustomLayer>() const {
    return type == style::LayerType::Custom;
}

} // namespace mbgl
