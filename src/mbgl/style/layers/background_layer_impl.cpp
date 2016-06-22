#include <mbgl/style/layers/background_layer_impl.hpp>
#include <mbgl/renderer/bucket.hpp>

namespace mbgl {
namespace style {

void BackgroundLayer::Impl::cascade(const CascadeParameters& parameters) {
    paint.cascade(parameters);
}

bool BackgroundLayer::Impl::recalculate(const CalculationParameters& parameters) {
    bool hasTransitions = paint.recalculate(parameters);

    passes = paint.backgroundOpacity > 0 ? RenderPass::Translucent : RenderPass::None;

    return hasTransitions;
}

std::unique_ptr<Bucket> BackgroundLayer::Impl::createBucket(BucketParameters&) const {
    return nullptr;
}

} // namespace style
} // namespace mbgl
