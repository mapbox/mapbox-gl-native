#include <mbgl/layer/background_layer_impl.hpp>
#include <mbgl/renderer/bucket.hpp>

namespace mbgl {

void BackgroundLayer::Impl::parsePaints(const JSValue& layer) {
    paint.parse(layer);
}

void BackgroundLayer::Impl::cascade(const StyleCascadeParameters& parameters) {
    paint.cascade(parameters);
}

bool BackgroundLayer::Impl::recalculate(const StyleCalculationParameters& parameters) {
    bool hasTransitions = paint.recalculate(parameters);

    passes = paint.backgroundOpacity > 0 ? RenderPass::Translucent : RenderPass::None;

    return hasTransitions;
}

std::unique_ptr<Bucket> BackgroundLayer::Impl::createBucket(StyleBucketParameters&) const {
    return nullptr;
}

} // namespace mbgl
