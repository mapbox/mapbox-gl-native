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
        auto bucket_ = tile.tile.getBucket<CircleBucket>(*baseImpl);
        if (!bucket_) {
            continue;
        }
        CircleBucket& bucket = *bucket_;

        const auto& paintPropertyBinders = bucket.paintPropertyBinders.at(getID());

        auto& programInstance = parameters.programs.circle.get(evaluated);
   
        const auto allUniformValues = programInstance.computeAllUniformValues(
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
            paintPropertyBinders,
            evaluated,
            parameters.state.getZoom()
        );
        const auto allAttributeBindings = programInstance.computeAllAttributeBindings(
            *bucket.vertexBuffer,
            paintPropertyBinders,
            evaluated
        );

        checkRenderability(parameters, programInstance.activeBindingCount(allAttributeBindings));

        programInstance.draw(
            parameters.context,
            gl::Triangles(),
            parameters.depthModeForSublayer(0, gl::DepthMode::ReadOnly),
            parameters.mapMode != MapMode::Continuous
                ? parameters.stencilModeForClipping(tile.clip)
                : gl::StencilMode::disabled(),
            parameters.colorModeForRenderPass(),
            *bucket.indexBuffer,
            bucket.segments,
            allUniformValues,
            allAttributeBindings,
            getID()
        );
    }
}

GeometryCoordinate projectPoint(const GeometryCoordinate& p, const mat4& posMatrix, const Size& size) {
    vec4 pos = {{ static_cast<double>(p.x), static_cast<double>(p.y), 0, 1 }};
    matrix::transformMat4(pos, pos, posMatrix);
    return {
        static_cast<int16_t>((static_cast<float>(pos[0] / pos[3]) + 1) * size.width * 0.5),
        static_cast<int16_t>((static_cast<float>(pos[1] / pos[3]) + 1) * size.height * 0.5)
    };
}

GeometryCoordinates projectQueryGeometry(const GeometryCoordinates& queryGeometry, const mat4& posMatrix, const Size& size) {
    GeometryCoordinates projectedGeometry;
    for (auto& p : queryGeometry) {
        projectedGeometry.push_back(projectPoint(p, posMatrix, size));
    }
    return projectedGeometry;
}

bool RenderCircleLayer::queryIntersectsFeature(
        const GeometryCoordinates& queryGeometry,
        const GeometryTileFeature& feature,
        const float zoom,
        const TransformState& transformState,
        const float pixelsToTileUnits,
        const mat4& posMatrix) const {

    // Translate query geometry
    const GeometryCoordinates& translatedQueryGeometry = FeatureIndex::translateQueryGeometry(
            queryGeometry,
            evaluated.get<style::CircleTranslate>(),
            evaluated.get<style::CircleTranslateAnchor>(),
            transformState.getAngle(),
            pixelsToTileUnits).value_or(queryGeometry);

    // Evaluate functions
    auto radius = evaluated.evaluate<style::CircleRadius>(zoom, feature);
    auto stroke = evaluated.evaluate<style::CircleStrokeWidth>(zoom, feature);
    auto size = radius + stroke;

    // For pitch-alignment: map, compare feature geometry to query geometry in the plane of the tile
    // Otherwise, compare geometry in the plane of the viewport
    // A circle with fixed scaling relative to the viewport gets larger in tile space as it moves into the distance
    // A circle with fixed scaling relative to the map gets smaller in viewport space as it moves into the distance
    bool alignWithMap = evaluated.evaluate<style::CirclePitchAlignment>(zoom, feature) == AlignmentType::Map;
    const GeometryCoordinates& transformedQueryGeometry = alignWithMap ?
        translatedQueryGeometry :
        projectQueryGeometry(translatedQueryGeometry, posMatrix, transformState.getSize());
    auto transformedSize = alignWithMap ? size * pixelsToTileUnits : size;

    auto geometry = feature.getGeometries();
    for (auto& ring : geometry) {
        for (auto& point : ring) {
            const GeometryCoordinate& transformedPoint = alignWithMap ? point : projectPoint(point, posMatrix, transformState.getSize());

            float adjustedSize = transformedSize;
            vec4 center = {{ static_cast<double>(point.x), static_cast<double>(point.y), 0, 1 }};
            matrix::transformMat4(center, center, posMatrix);
            auto pitchScale = evaluated.evaluate<style::CirclePitchScale>(zoom, feature);
            auto pitchAlignment = evaluated.evaluate<style::CirclePitchAlignment>(zoom, feature);
            if (pitchScale == CirclePitchScaleType::Viewport && pitchAlignment == AlignmentType::Map) {
                adjustedSize *= center[3] / transformState.getCameraToCenterDistance();
            } else if (pitchScale == CirclePitchScaleType::Map && pitchAlignment == AlignmentType::Viewport) {
                adjustedSize *= transformState.getCameraToCenterDistance() / center[3];
            }

            if (util::polygonIntersectsBufferedPoint(transformedQueryGeometry, transformedPoint, adjustedSize)) return true;
        }
    }

    return false;
}

} // namespace mbgl
