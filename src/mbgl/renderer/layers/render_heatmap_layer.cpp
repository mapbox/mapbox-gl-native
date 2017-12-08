#include <mbgl/renderer/layers/render_heatmap_layer.hpp>
#include <mbgl/renderer/buckets/heatmap_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/heatmap_program.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/style/layers/heatmap_layer_impl.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/intersection_tests.hpp>

#pragma GCC diagnostic ignored "-Wunused-parameter"

namespace mbgl {

using namespace style;

RenderHeatmapLayer::RenderHeatmapLayer(Immutable<style::HeatmapLayer::Impl> _impl)
    : RenderLayer(style::LayerType::Heatmap, _impl),
      unevaluated(impl().paint.untransitioned()) {
}

const style::HeatmapLayer::Impl& RenderHeatmapLayer::impl() const {
    return static_cast<const style::HeatmapLayer::Impl&>(*baseImpl);
}

std::unique_ptr<Bucket> RenderHeatmapLayer::createBucket(const BucketParameters& parameters, const std::vector<const RenderLayer*>& layers) const {
    return std::make_unique<HeatmapBucket>(parameters, layers);
}

void RenderHeatmapLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl().paint.transitioned(parameters, std::move(unevaluated));
}

void RenderHeatmapLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    evaluated = unevaluated.evaluate(parameters);

    passes = ((evaluated.get<style::HeatmapRadius>() > 0)
              && (evaluated.get<style::HeatmapColor>().a > 0)
              && (evaluated.get<style::HeatmapOpacity>() > 0))
             ? RenderPass::Translucent : RenderPass::None;
}

bool RenderHeatmapLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

void RenderHeatmapLayer::render(PaintParameters& parameters, RenderSource*) {
    if (parameters.pass == RenderPass::Opaque) {
        return;
    }

    for (const RenderTile& tile : renderTiles) {
        assert(dynamic_cast<HeatmapBucket*>(tile.tile.getBucket(*baseImpl)));
        HeatmapBucket& bucket = *reinterpret_cast<HeatmapBucket*>(tile.tile.getBucket(*baseImpl));

        parameters.programs.heatmap.get(evaluated).draw(
            parameters.context,
            gl::Triangles(),
            parameters.depthModeForSublayer(0, gl::DepthMode::ReadOnly),
            parameters.mapMode != MapMode::Continuous
                ? parameters.stencilModeForClipping(tile.clip)
                : gl::StencilMode::disabled(),
            parameters.colorModeForRenderPass(),
            HeatmapProgram::UniformValues {
                uniforms::u_matrix::Value{
                    tile.translatedMatrix(std::array<float, 2>{{0, 0}},
                                          TranslateAnchorType::Map,
                                          parameters.state)
                },
                uniforms::u_extrude_scale::Value{ std::array<float, 2> {{
                    tile.id.pixelsToTileUnits(1, parameters.state.getZoom()),
                    tile.id.pixelsToTileUnits(1, parameters.state.getZoom())
                }}}
            },
            *bucket.vertexBuffer,
            *bucket.indexBuffer,
            bucket.segments,
            bucket.paintPropertyBinders.at(getID()),
            evaluated,
            parameters.state.getZoom(),
            getID()
        );
    }
}

bool RenderHeatmapLayer::queryIntersectsFeature(
        const GeometryCoordinates& queryGeometry,
        const GeometryTileFeature& feature,
        const float zoom,
        const float bearing,
        const float pixelsToTileUnits) const {
    return false;
}

} // namespace mbgl
