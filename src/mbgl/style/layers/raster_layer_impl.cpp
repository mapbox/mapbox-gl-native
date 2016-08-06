#include <mbgl/style/layers/raster_layer_impl.hpp>
#include <mbgl/renderer/bucket.hpp>

namespace mbgl {
namespace style {

void RasterLayer::Impl::cascade(const CascadeParameters& parameters) {
    paint.cascade(parameters);
}

bool RasterLayer::Impl::recalculate(const CalculationParameters& parameters) {
    bool hasTransitions = paint.recalculate(parameters);

    passes = paint.rasterOpacity > 0 ? RenderPass::Translucent : RenderPass::None;

    return hasTransitions;
}

std::unique_ptr<Bucket> RasterLayer::Impl::createBucket(BucketParameters&) const {
    return nullptr;
}

} // namespace style
} // namespace mbgl
