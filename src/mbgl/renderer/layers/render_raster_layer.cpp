#include <mbgl/renderer/layers/render_raster_layer.hpp>
#include <mbgl/renderer/buckets/raster_bucket.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/renderer/sources/render_image_source.hpp>
#include <mbgl/style/layers/raster_layer_impl.hpp>

namespace mbgl {

RenderRasterLayer::RenderRasterLayer(Immutable<style::RasterLayer::Impl> _impl)
    : RenderLayer(style::LayerType::Raster, _impl),
      unevaluated(impl().paint.untransitioned()) {
}

const style::RasterLayer::Impl& RenderRasterLayer::impl() const {
    return static_cast<const style::RasterLayer::Impl&>(*baseImpl);
}

std::unique_ptr<Bucket> RenderRasterLayer::createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const {
    assert(false);
    return nullptr;
}

void RenderRasterLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl().paint.transitioned(parameters, std::move(unevaluated));
}

void RenderRasterLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    evaluated = unevaluated.evaluate(parameters);

    passes = evaluated.get<style::RasterOpacity>() > 0 ? RenderPass::Translucent : RenderPass::None;
}

bool RenderRasterLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

void RenderRasterLayer::render(Painter& painter, PaintParameters& parameters, RenderSource* source) {
    if (RenderImageSource* imageSource = source->as<RenderImageSource>()) {
        imageSource->render(painter, parameters, *this);
    } else {
        for (const RenderTile& tile : renderTiles) {
            Bucket* bucket = tile.tile.getBucket(*baseImpl);
            assert(dynamic_cast<RasterBucket*>(bucket));
            painter.renderRaster(
                parameters,
                *reinterpret_cast<RasterBucket*>(bucket),
                *this,
                tile.matrix,
                false);
        }
    }
}

} // namespace mbgl
