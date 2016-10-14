#include <mbgl/style/layers/raster_layer_impl.hpp>
#include <mbgl/style/raster_bucket_parameters.hpp>
#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/util/image.hpp>

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

std::unique_ptr<Bucket> RasterLayer::Impl::createBucket(BucketParameters& params) const {
    auto& parameters = params.get<RasterBucketParameters>();
    return std::make_unique<RasterBucket>(decodeImage(*parameters.data));
}

} // namespace style
} // namespace mbgl
