#include <mbgl/style/layers/background_layer_impl.hpp>
#include <mbgl/renderer/bucket.hpp>

namespace mbgl {
namespace style {

void BackgroundLayer::Impl::cascade(const CascadeParameters& parameters) {
    paint.cascade(parameters);
}

bool BackgroundLayer::Impl::evaluate(const PropertyEvaluationParameters& parameters) {
    paint.evaluate(parameters);

    passes = paint.evaluated.get<BackgroundOpacity>() > 0 ? RenderPass::Translucent : RenderPass::None;

    return paint.hasTransition();
}

std::unique_ptr<Bucket> BackgroundLayer::Impl::createBucket(BucketParameters&, const GeometryTileLayer&) const {
    return nullptr;
}

} // namespace style
} // namespace mbgl
