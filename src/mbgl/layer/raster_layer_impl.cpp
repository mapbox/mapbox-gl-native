#include <mbgl/layer/raster_layer_impl.hpp>
#include <mbgl/renderer/bucket.hpp>

namespace mbgl {

void RasterLayer::Impl::parsePaints(const JSValue& layer) {
    paint.parse(layer);
}

void RasterLayer::Impl::cascade(const StyleCascadeParameters& parameters) {
    paint.cascade(parameters);
}

bool RasterLayer::Impl::recalculate(const StyleCalculationParameters& parameters) {
    bool hasTransitions = paint.recalculate(parameters);

    passes = paint.rasterOpacity > 0 ? RenderPass::Translucent : RenderPass::None;

    return hasTransitions;
}

std::unique_ptr<Bucket> RasterLayer::Impl::createBucket(StyleBucketParameters&) const {
    return nullptr;
}

} // namespace mbgl
