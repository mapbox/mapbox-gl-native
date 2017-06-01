#include <mbgl/renderer/layers/render_raster_layer.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/style/layers/raster_layer_impl.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/renderer/sources/render_image_source.hpp>
#include <mbgl/renderer/painter.hpp>

namespace mbgl {

RenderRasterLayer::RenderRasterLayer(Immutable<style::RasterLayer::Impl> _impl)
    : RenderLayer(style::LayerType::Raster, _impl) {
}

const style::RasterLayer::Impl& RenderRasterLayer::impl() const {
    return static_cast<const style::RasterLayer::Impl&>(*baseImpl);
}

std::unique_ptr<Bucket> RenderRasterLayer::createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const {
    assert(false);
    return nullptr;
}

void RenderRasterLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl().paint.transition(parameters, std::move(unevaluated));
}

void RenderRasterLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    evaluated = unevaluated.evaluate(parameters);

    passes = evaluated.get<style::RasterOpacity>() > 0 ? RenderPass::Translucent : RenderPass::None;
}

bool RenderRasterLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

void RenderRasterLayer::uploadBuckets(gl::Context& context, RenderSource* source) {
    RenderLayer::uploadBuckets(context, source);
    if (renderTiles.empty()) {
        RenderImageSource* imageSource = source->as<RenderImageSource>();
        if (imageSource) {
            imageSource->upload(context);
        }
    }
}

void RenderRasterLayer::render(Painter& painter, PaintParameters& parameters, RenderSource* source) {
    RenderLayer::render(painter, parameters, source);
    if (renderTiles.empty()) {
        RenderImageSource* imageSource = source->as<RenderImageSource>();
        if (imageSource) {
            imageSource->render(painter, parameters, *this);
        }
    }
}

} // namespace mbgl
