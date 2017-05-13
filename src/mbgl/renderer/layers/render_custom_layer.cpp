#include <mbgl/renderer/layers/render_custom_layer.hpp>
#include <mbgl/style/layers/custom_layer_impl.hpp>
#include <mbgl/renderer/bucket.hpp>

namespace mbgl {

RenderCustomLayer::RenderCustomLayer(Immutable<style::CustomLayer::Impl> _impl)
    : RenderLayer(style::LayerType::Custom, _impl) {
}

const style::CustomLayer::Impl& RenderCustomLayer::impl() const {
    return static_cast<const style::CustomLayer::Impl&>(*baseImpl);
}

void RenderCustomLayer::evaluate(const PropertyEvaluationParameters&) {
    passes = RenderPass::Translucent;
}

bool RenderCustomLayer::hasTransition() const {
    return false;
}

std::unique_ptr<Bucket> RenderCustomLayer::createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const {
    assert(false);
    return nullptr;
}
    
} // namespace mbgl
