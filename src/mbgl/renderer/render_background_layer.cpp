#include <mbgl/renderer/render_background_layer.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>
#include <mbgl/renderer/bucket.hpp>

namespace mbgl {

RenderBackgroundLayer::RenderBackgroundLayer(const style::BackgroundLayer::Impl& _impl)
        : RenderLayer(style::LayerType::Background, _impl),
          impl(&_impl) {
}

std::unique_ptr<RenderLayer> RenderBackgroundLayer::clone() const {
    return std::make_unique<RenderBackgroundLayer>(*this);
}

std::unique_ptr<Bucket> RenderBackgroundLayer::createBucket(const BucketParameters &,
                                                            const std::vector<const RenderLayer *> &) const {
    assert(false);
    return nullptr;
}

void RenderBackgroundLayer::cascade(const CascadeParameters &parameters) {
    unevaluated = impl->cascading.cascade(parameters, std::move(unevaluated));
}

void RenderBackgroundLayer::evaluate(const PropertyEvaluationParameters &parameters) {
    evaluated = unevaluated.evaluate(parameters);

    passes = evaluated.get<style::BackgroundOpacity>() > 0 ? RenderPass::Translucent
                                                           : RenderPass::None;
}

bool RenderBackgroundLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

}
