#include <mbgl/style/layers/raster_layer_impl.hpp>
#include <mbgl/renderer/bucket.hpp>

namespace mbgl {
namespace style {

void RasterLayer::Impl::cascade(const CascadeParameters& parameters) {
    paint.cascade(parameters);
}

bool RasterLayer::Impl::evaluate(const PropertyEvaluationParameters& parameters) {
    paint.evaluate(parameters);

    passes = paint.evaluated.get<RasterOpacity>() > 0 ? RenderPass::Translucent : RenderPass::None;

    return paint.hasTransition();
}

std::unique_ptr<Bucket> RasterLayer::Impl::createBucket(BucketParameters&, const GeometryTileLayer&) const {
    return nullptr;
}

} // namespace style
} // namespace mbgl
