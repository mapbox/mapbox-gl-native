#include <mbgl/renderer/layers/render_fill_layer.hpp>
#include <mbgl/renderer/buckets/fill_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/fill_program.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/style/layers/fill_layer_impl.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/intersection_tests.hpp>

namespace mbgl {

using namespace style;

RenderFillLayer::RenderFillLayer(Immutable<style::FillLayer::Impl> _impl)
    : RenderLayer(style::LayerType::Fill, _impl),
      unevaluated(impl().paint.untransitioned()) {
}

const style::FillLayer::Impl& RenderFillLayer::impl() const {
    return static_cast<const style::FillLayer::Impl&>(*baseImpl);
}

std::unique_ptr<Bucket> RenderFillLayer::createBucket(const BucketParameters& parameters, const std::vector<const RenderLayer*>& layers) const {
    return std::make_unique<FillBucket>(parameters, layers);
}

void RenderFillLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl().paint.transitioned(parameters, std::move(unevaluated));
}

void RenderFillLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    evaluated = unevaluated.evaluate(parameters);

    if (unevaluated.get<style::FillOutlineColor>().isUndefined()) {
        evaluated.get<style::FillOutlineColor>() = evaluated.get<style::FillColor>();
    }

    passes = RenderPass::None;

    if (evaluated.get<style::FillAntialias>()) {
        passes |= RenderPass::Translucent;
    }

    if (!unevaluated.get<style::FillPattern>().isUndefined()
        || evaluated.get<style::FillColor>().constantOr(Color()).a < 1.0f
        || evaluated.get<style::FillOpacity>().constantOr(0) < 1.0f) {
        passes |= RenderPass::Translucent;
    } else {
        passes |= RenderPass::Opaque;
    }
}

bool RenderFillLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

void RenderFillLayer::render(PaintParameters& parameters, RenderSource*) {
    if (evaluated.get<FillPattern>().from.empty()) {
        for (const RenderTile& tile : renderTiles) {
            auto bucket_ = tile.tile.getBucket<FillBucket>(*baseImpl);
            if (!bucket_) {
                continue;
            }
            FillBucket& bucket = *bucket_;

            auto draw = [&] (auto& program,
                             const auto& drawMode,
                             const auto& depthMode,
                             const auto& indexBuffer,
                             const auto& segments) {
                auto& programInstance = program.get(evaluated);

                const auto& paintPropertyBinders = bucket.paintPropertyBinders.at(getID());

                const auto allUniformValues = programInstance.computeAllUniformValues(
                    FillProgram::UniformValues {
                        uniforms::u_matrix::Value{
                            tile.translatedMatrix(evaluated.get<FillTranslate>(),
                                                  evaluated.get<FillTranslateAnchor>(),
                                                  parameters.state)
                        },
                        uniforms::u_world::Value{ parameters.context.viewport.getCurrentValue().size },
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
                    drawMode,
                    depthMode,
                    parameters.stencilModeForClipping(tile.clip),
                    parameters.colorModeForRenderPass(),
                    indexBuffer,
                    segments,
                    allUniformValues,
                    allAttributeBindings,
                    getID()
                );
            };

            // Only draw the fill when it's opaque and we're drawing opaque fragments,
            // or when it's translucent and we're drawing translucent fragments.
            if ((evaluated.get<FillColor>().constantOr(Color()).a >= 1.0f
              && evaluated.get<FillOpacity>().constantOr(0) >= 1.0f) == (parameters.pass == RenderPass::Opaque)) {
                draw(parameters.programs.fill,
                     gl::Triangles(),
                     parameters.depthModeForSublayer(1, parameters.pass == RenderPass::Opaque
                        ? gl::DepthMode::ReadWrite
                        : gl::DepthMode::ReadOnly),
                     *bucket.triangleIndexBuffer,
                     bucket.triangleSegments);
            }

            if (evaluated.get<FillAntialias>() && parameters.pass == RenderPass::Translucent) {
                draw(parameters.programs.fillOutline,
                     gl::Lines{ 2.0f },
                     parameters.depthModeForSublayer(
                         unevaluated.get<FillOutlineColor>().isUndefined() ? 2 : 0,
                         gl::DepthMode::ReadOnly),
                     *bucket.lineIndexBuffer,
                     bucket.lineSegments);
            }
        }
    } else {
        if (parameters.pass != RenderPass::Translucent) {
            return;
        }

        optional<ImagePosition> imagePosA = parameters.imageManager.getPattern(evaluated.get<FillPattern>().from);
        optional<ImagePosition> imagePosB = parameters.imageManager.getPattern(evaluated.get<FillPattern>().to);

        if (!imagePosA || !imagePosB) {
            return;
        }

        parameters.imageManager.bind(parameters.context, 0);

        for (const RenderTile& tile : renderTiles) {
            auto bucket_ = tile.tile.getBucket<FillBucket>(*baseImpl);
            if (!bucket_) {
                continue;
            }
            FillBucket& bucket = *bucket_;

            auto draw = [&] (auto& program,
                             const auto& drawMode,
                             const auto& depthMode,
                             const auto& indexBuffer,
                             const auto& segments) {
                auto& programInstance = program.get(evaluated);

                const auto& paintPropertyBinders = bucket.paintPropertyBinders.at(getID());

                const auto allUniformValues = programInstance.computeAllUniformValues(
                    FillPatternUniforms::values(
                        tile.translatedMatrix(evaluated.get<FillTranslate>(),
                                              evaluated.get<FillTranslateAnchor>(),
                                              parameters.state),
                        parameters.context.viewport.getCurrentValue().size,
                        parameters.imageManager.getPixelSize(),
                        *imagePosA,
                        *imagePosB,
                        evaluated.get<FillPattern>(),
                        tile.id,
                        parameters.state
                    ),
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
                    drawMode,
                    depthMode,
                    parameters.stencilModeForClipping(tile.clip),
                    parameters.colorModeForRenderPass(),
                    indexBuffer,
                    segments,
                    allUniformValues,
                    allAttributeBindings,
                    getID()
                );
            };

            draw(parameters.programs.fillPattern,
                 gl::Triangles(),
                 parameters.depthModeForSublayer(1, gl::DepthMode::ReadWrite),
                 *bucket.triangleIndexBuffer,
                 bucket.triangleSegments);

            if (evaluated.get<FillAntialias>() && unevaluated.get<FillOutlineColor>().isUndefined()) {
                draw(parameters.programs.fillOutlinePattern,
                     gl::Lines { 2.0f },
                     parameters.depthModeForSublayer(2, gl::DepthMode::ReadOnly),
                     *bucket.lineIndexBuffer,
                     bucket.lineSegments);
            }
        }
    }
}

bool RenderFillLayer::queryIntersectsFeature(
        const GeometryCoordinates& queryGeometry,
        const GeometryTileFeature& feature,
        const float,
        const TransformState& transformState,
        const float pixelsToTileUnits,
        const mat4&) const {

    auto translatedQueryGeometry = FeatureIndex::translateQueryGeometry(
            queryGeometry,
            evaluated.get<style::FillTranslate>(),
            evaluated.get<style::FillTranslateAnchor>(),
            transformState.getAngle(),
            pixelsToTileUnits);

    return util::polygonIntersectsMultiPolygon(translatedQueryGeometry.value_or(queryGeometry), feature.getGeometries());
}


} // namespace mbgl
