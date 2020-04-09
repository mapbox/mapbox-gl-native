#include <mbgl/renderer/layers/render_circle_layer.hpp>
#include <mbgl/renderer/buckets/circle_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/circle_program.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/style/layers/circle_layer_impl.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/gfx/cull_face_mode.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/intersection_tests.hpp>

namespace mbgl {

using namespace style;

namespace {

struct RenderableSegment {
    RenderableSegment(const Segment<CircleAttributes>& segment_,
                      CircleProgram& programInstance_,
                      const CircleBucket& bucket_,
                      const CircleProgram::UniformValues& allUniformValues_,
                      const CircleProgram::AttributeBindings& allAttributeBindings_,
                      float sortKey_)
        : segment(segment_),
          programInstance(programInstance_),
          bucket(bucket_),
          allUniformValues(allUniformValues_),
          allAttributeBindings(allAttributeBindings_),
          sortKey(sortKey_) {}

    const Segment<CircleAttributes>& segment;
    CircleProgram& programInstance;
    const CircleBucket& bucket;
    const CircleProgram::UniformValues& allUniformValues;
    const CircleProgram::AttributeBindings& allAttributeBindings;
    const float sortKey;

    friend bool operator<(const RenderableSegment& lhs, const RenderableSegment& rhs) {
        return lhs.sortKey < rhs.sortKey;
    }
};

inline const style::CircleLayer::Impl& impl_cast(const Immutable<style::Layer::Impl>& impl) {
    assert(impl->getTypeInfo() == CircleLayer::Impl::staticTypeInfo());
    return static_cast<const style::CircleLayer::Impl&>(*impl);
}

} // namespace

RenderCircleLayer::RenderCircleLayer(Immutable<style::CircleLayer::Impl> _impl)
    : RenderLayer(makeMutable<CircleLayerProperties>(std::move(_impl))),
      unevaluated(impl_cast(baseImpl).paint.untransitioned()) {}

void RenderCircleLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl_cast(baseImpl).paint.transitioned(parameters, std::move(unevaluated));
}

void RenderCircleLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    auto properties = makeMutable<CircleLayerProperties>(
        staticImmutableCast<CircleLayer::Impl>(baseImpl),
        unevaluated.evaluate(parameters));
    const auto& evaluated = properties->evaluated;

    passes = ((evaluated.get<style::CircleRadius>().constantOr(1) > 0 ||
               evaluated.get<style::CircleStrokeWidth>().constantOr(1) > 0)
              && (evaluated.get<style::CircleColor>().constantOr(Color::black()).a > 0 ||
                  evaluated.get<style::CircleStrokeColor>().constantOr(Color::black()).a > 0)
              && (evaluated.get<style::CircleOpacity>().constantOr(1) > 0 ||
                  evaluated.get<style::CircleStrokeOpacity>().constantOr(1) > 0))
             ? RenderPass::Translucent : RenderPass::None;
    properties->renderPasses = mbgl::underlying_type(passes);
    evaluatedProperties = std::move(properties);
}

bool RenderCircleLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

bool RenderCircleLayer::hasCrossfade() const {
    return false;
}

void RenderCircleLayer::render(PaintParameters& parameters) {
    assert(renderTiles);
    if (parameters.pass == RenderPass::Opaque) {
        return;
    }

    const bool sortFeaturesByKey = !impl_cast(baseImpl).layout.get<CircleSortKey>().isUndefined();
    std::multiset<RenderableSegment> renderableSegments{};

    for (const RenderTile& tile : *renderTiles) {
        const LayerRenderData* renderData = getRenderDataForPass(tile, parameters.pass);
        if (!renderData) {
            continue;
        }
        auto& bucket = static_cast<CircleBucket&>(*renderData->bucket);
        const auto& evaluated = getEvaluated<CircleLayerProperties>(renderData->layerProperties);
        const bool scaleWithMap = evaluated.get<CirclePitchScale>() == CirclePitchScaleType::Map;
        const bool pitchWithMap = evaluated.get<CirclePitchAlignment>() == AlignmentType::Map;
        const auto& paintPropertyBinders = bucket.paintPropertyBinders.at(getID());

        auto& programInstance = parameters.programs.getCircleLayerPrograms().circle;

        const auto allUniformValues = CircleProgram::computeAllUniformValues(
            CircleProgram::LayoutUniformValues{
                uniforms::matrix::Value(tile.translatedMatrix(
                    evaluated.get<CircleTranslate>(), evaluated.get<CircleTranslateAnchor>(), parameters.state)),
                uniforms::scale_with_map::Value(scaleWithMap),
                uniforms::extrude_scale::Value(
                    pitchWithMap ? std::array<float, 2>{{tile.id.pixelsToTileUnits(1, parameters.state.getZoom()),
                                                         tile.id.pixelsToTileUnits(1, parameters.state.getZoom())}}
                                 : parameters.pixelsToGLUnits),
                uniforms::device_pixel_ratio::Value(parameters.pixelRatio),
                uniforms::camera_to_center_distance::Value(parameters.state.getCameraToCenterDistance()),
                uniforms::pitch_with_map::Value(pitchWithMap)},
            paintPropertyBinders,
            evaluated,
            parameters.state.getZoom());
        const auto allAttributeBindings =
            CircleProgram::computeAllAttributeBindings(*bucket.vertexBuffer, paintPropertyBinders, evaluated);

        checkRenderability(parameters, CircleProgram::activeBindingCount(allAttributeBindings));

        if (sortFeaturesByKey) {
            for (auto& segment : bucket.segments) {
                renderableSegments.emplace(
                    segment, programInstance, bucket, allUniformValues, allAttributeBindings, segment.sortKey);
            }
        } else {
            programInstance.draw(parameters.context,
                                 *parameters.renderPass,
                                 gfx::Triangles(),
                                 parameters.depthModeForSublayer(0, gfx::DepthMaskType::ReadOnly),
                                 gfx::StencilMode::disabled(),
                                 parameters.colorModeForRenderPass(),
                                 gfx::CullFaceMode::disabled(),
                                 *bucket.indexBuffer,
                                 bucket.segments,
                                 allUniformValues,
                                 allAttributeBindings,
                                 CircleProgram::TextureBindings{},
                                 getID());
        }
    }

    if (sortFeaturesByKey) {
        for (const auto& renderable : renderableSegments) {
            renderable.programInstance.draw(parameters.context,
                                            *parameters.renderPass,
                                            gfx::Triangles(),
                                            parameters.depthModeForSublayer(0, gfx::DepthMaskType::ReadOnly),
                                            gfx::StencilMode::disabled(),
                                            parameters.colorModeForRenderPass(),
                                            gfx::CullFaceMode::disabled(),
                                            *renderable.bucket.indexBuffer,
                                            renderable.segment,
                                            renderable.allUniformValues,
                                            renderable.allAttributeBindings,
                                            CircleProgram::TextureBindings{},
                                            getID());
        }
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

bool RenderCircleLayer::queryIntersectsFeature(const GeometryCoordinates& queryGeometry,
                                               const GeometryTileFeature& feature, const float zoom,
                                               const TransformState& transformState, const float pixelsToTileUnits,
                                               const mat4& posMatrix, const FeatureState& featureState) const {
    const auto& evaluated = static_cast<const CircleLayerProperties&>(*evaluatedProperties).evaluated;
    // Translate query geometry
    const GeometryCoordinates& translatedQueryGeometry = FeatureIndex::translateQueryGeometry(
            queryGeometry,
            evaluated.get<style::CircleTranslate>(),
            evaluated.get<style::CircleTranslateAnchor>(),
            transformState.getBearing(),
            pixelsToTileUnits).value_or(queryGeometry);

    // Evaluate functions
    auto radius = evaluated.evaluate<style::CircleRadius>(zoom, feature, featureState);
    auto stroke = evaluated.evaluate<style::CircleStrokeWidth>(zoom, feature, featureState);
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

    const auto& geometry = feature.getGeometries();
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
