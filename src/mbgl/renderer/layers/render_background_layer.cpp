#include <mbgl/renderer/layers/render_background_layer.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>
#include <mbgl/renderer/bucket.hpp>

namespace mbgl {

RenderBackgroundLayer::RenderBackgroundLayer(Immutable<style::BackgroundLayer::Impl> _impl)
    : RenderLayer(style::LayerType::Background, _impl) {
}

const style::BackgroundLayer::Impl& RenderBackgroundLayer::impl() const {
    return static_cast<const style::BackgroundLayer::Impl&>(*baseImpl);
}

std::unique_ptr<Bucket> RenderBackgroundLayer::createBucket(const BucketParameters &,
                                                            const std::vector<const RenderLayer *> &) const {
    assert(false);
    return nullptr;
}

void RenderBackgroundLayer::transition(const TransitionParameters &parameters) {
    unevaluated = impl().paint.transition(parameters, std::move(unevaluated));
}

void RenderBackgroundLayer::evaluate(const PropertyEvaluationParameters &parameters) {
    evaluated = unevaluated.evaluate(parameters);

    passes = evaluated.get<style::BackgroundOpacity>() > 0 ? RenderPass::Translucent
                                                           : RenderPass::None;
}

bool RenderBackgroundLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

} // namespace mbgl
