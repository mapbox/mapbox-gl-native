#include <mbgl/renderer/layers/render_circle_layer.hpp>
#include <mbgl/renderer/buckets/circle_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/circle_program.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/style/layers/circle_layer_impl.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/intersection_tests.hpp>

namespace mbgl {

using namespace style;

RenderCircleLayer::RenderCircleLayer(Immutable<style::CircleLayer::Impl> _impl)
    : RenderLayer(style::LayerType::Circle, _impl),
      unevaluated(impl().paint.untransitioned()) {
}

const style::CircleLayer::Impl& RenderCircleLayer::impl() const {
    return static_cast<const style::CircleLayer::Impl&>(*baseImpl);
}

std::unique_ptr<Bucket> RenderCircleLayer::createBucket(const BucketParameters& parameters, const std::vector<const RenderLayer*>& layers) const {
    return std::make_unique<CircleBucket>(parameters, layers);
}

void RenderCircleLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl().paint.transitioned(parameters, std::move(unevaluated));
}

void RenderCircleLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    evaluated = unevaluated.evaluate(parameters);

    passes = ((evaluated.get<style::CircleRadius>().constantOr(1) > 0 ||
               evaluated.get<style::CircleStrokeWidth>().constantOr(1) > 0)
              && (evaluated.get<style::CircleColor>().constantOr(Color::black()).a > 0 ||
                  evaluated.get<style::CircleStrokeColor>().constantOr(Color::black()).a > 0)
              && (evaluated.get<style::CircleOpacity>().constantOr(1) > 0 ||
                  evaluated.get<style::CircleStrokeOpacity>().constantOr(1) > 0))
             ? RenderPass::Translucent : RenderPass::None;
}

bool RenderCircleLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

void RenderCircleLayer::render(PaintParameters& parameters, RenderSource*) {
    if (parameters.pass == RenderPass::Opaque) {
        return;
    }

    const bool scaleWithMap = evaluated.get<CirclePitchScale>() == CirclePitchScaleType::Map;
    const bool pitchWithMap = evaluated.get<CirclePitchAlignment>() == AlignmentType::Map;

    for (const RenderTile& tile : renderTiles) {
        assert(dynamic_cast<CircleBucket*>(tile.tile.getBucket(*baseImpl)));
        CircleBucket& bucket = *reinterpret_cast<CircleBucket*>(tile.tile.getBucket(*baseImpl));

        parameters.programs.circle.get(evaluated).draw(
            parameters.context,
            gl::Triangles(),
            parameters.depthModeForSublayer(0, gl::DepthMode::ReadOnly),
            parameters.mapMode != MapMode::Continuous
                ? parameters.stencilModeForClipping(tile.clip)
                : gl::StencilMode::disabled(),
            parameters.colorModeForRenderPass(),
            CircleProgram::UniformValues {
                uniforms::u_matrix::Value{
                    tile.translatedMatrix(evaluated.get<CircleTranslate>(),
                                          evaluated.get<CircleTranslateAnchor>(),
                                          parameters.state)
                },
                uniforms::u_scale_with_map::Value{ scaleWithMap },
                uniforms::u_extrude_scale::Value{ pitchWithMap
                    ? std::array<float, 2> {{
                        tile.id.pixelsToTileUnits(1, parameters.state.getZoom()),
                        tile.id.pixelsToTileUnits(1, parameters.state.getZoom()) }}
                    : parameters.pixelsToGLUnits },
                uniforms::u_camera_to_center_distance::Value{ parameters.state.getCameraToCenterDistance() },
                uniforms::u_pitch_with_map::Value{ pitchWithMap }
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

bool RenderCircleLayer::queryIntersectsFeature(
        const GeometryCoordinates& queryGeometry,
        const GeometryTileFeature& feature,
        const float zoom,
        const float bearing,
        const float pixelsToTileUnits) const {

    // Translate query geometry
    auto translatedQueryGeometry = FeatureIndex::translateQueryGeometry(
            queryGeometry,
            evaluated.get<style::CircleTranslate>(),
            evaluated.get<style::CircleTranslateAnchor>(),
            bearing,
            pixelsToTileUnits);

    // Evaluate functions
    auto radius = evaluated.evaluate<style::CircleRadius>(zoom, feature) * pixelsToTileUnits;
    auto stroke = evaluated.evaluate<style::CircleStrokeWidth>(zoom, feature) * pixelsToTileUnits;

    // Test intersection
    return util::polygonIntersectsBufferedMultiPoint(translatedQueryGeometry.value_or(queryGeometry), feature.getGeometries(), radius + stroke);
}

} // namespace mbgl
