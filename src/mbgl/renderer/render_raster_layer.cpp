#include <mbgl/renderer/render_raster_layer.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/style/layers/raster_layer_impl.hpp>

namespace mbgl {

RenderRasterLayer::RenderRasterLayer(const style::RasterLayer::Impl& _impl)
        : RenderLayer(style::LayerType::Raster, _impl),
          impl(&_impl) {
}

std::unique_ptr<RenderLayer> RenderRasterLayer::clone() const {
    return std::make_unique<RenderRasterLayer>(*this);
}

std::unique_ptr<Bucket> RenderRasterLayer::createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const {
    assert(false);
    return nullptr;
}

void RenderRasterLayer::cascade(const style::CascadeParameters& parameters) {
    unevaluated = impl->cascading.cascade(parameters, std::move(unevaluated));
}

bool RenderRasterLayer::evaluate(const style::PropertyEvaluationParameters& parameters) {
    evaluated = unevaluated.evaluate(parameters);

    passes = evaluated.get<style::RasterOpacity>() > 0 ? RenderPass::Translucent : RenderPass::None;

    return unevaluated.hasTransition();
}

} // namespace mbgl
