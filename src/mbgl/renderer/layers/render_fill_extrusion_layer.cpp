#include <mbgl/renderer/layers/render_fill_extrusion_layer.hpp>
#include <mbgl/renderer/buckets/fill_extrusion_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/renderer/render_static_data.hpp>
#include <mbgl/renderer/renderer_backend.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/fill_extrusion_program.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/intersection_tests.hpp>

namespace mbgl {

using namespace style;

RenderFillExtrusionLayer::RenderFillExtrusionLayer(Immutable<style::FillExtrusionLayer::Impl> _impl)
    : RenderLayer(style::LayerType::FillExtrusion, _impl),
      unevaluated(impl().paint.untransitioned()) {
}

const style::FillExtrusionLayer::Impl& RenderFillExtrusionLayer::impl() const {
    return static_cast<const style::FillExtrusionLayer::Impl&>(*baseImpl);
}

std::unique_ptr<Bucket> RenderFillExtrusionLayer::createBucket(const BucketParameters& parameters, const std::vector<const RenderLayer*>& layers) const {
    return std::make_unique<FillExtrusionBucket>(parameters, layers);
}

void RenderFillExtrusionLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl().paint.transitioned(parameters, std::move(unevaluated));
}

void RenderFillExtrusionLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    evaluated = unevaluated.evaluate(parameters);

    passes = (evaluated.get<style::FillExtrusionOpacity>() > 0)
                 ? (RenderPass::Translucent | RenderPass::Pass3D)
                 : RenderPass::None;
}

bool RenderFillExtrusionLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

void RenderFillExtrusionLayer::render(PaintParameters& parameters, RenderSource*) {
    if (parameters.pass == RenderPass::Opaque) {
        return;
    }

    if (parameters.pass == RenderPass::Pass3D) {
        const auto& size = parameters.staticData.backendSize;

        if (!renderTexture || renderTexture->getSize() != size) {
            renderTexture = OffscreenTexture(parameters.context, size, *parameters.staticData.depthRenderbuffer);
        }

        renderTexture->bind();

        optional<float> depthClearValue = {};
        if (parameters.staticData.depthRenderbuffer->needsClearing()) depthClearValue = 1.0;
        // Flag the depth buffer as no longer needing to be cleared for the remainder of this pass.
        parameters.staticData.depthRenderbuffer->shouldClear(false);

        parameters.context.setStencilMode(gl::StencilMode::disabled());
        parameters.context.clear(Color{ 0.0f, 0.0f, 0.0f, 0.0f }, depthClearValue, {});

        auto draw = [&](auto& programInstance, const auto& tileBucket, auto&& uniformValues) {
            const auto& paintPropertyBinders = tileBucket.paintPropertyBinders.at(getID());

            const auto allUniformValues = programInstance.computeAllUniformValues(
                std::move(uniformValues),
                paintPropertyBinders,
                evaluated,
                parameters.state.getZoom()
            );
            const auto allAttributeBindings = programInstance.computeAllAttributeBindings(
                *tileBucket.vertexBuffer,
                paintPropertyBinders,
                evaluated
            );

            checkRenderability(parameters, programInstance.activeBindingCount(allAttributeBindings));

            programInstance.draw(
                parameters.context,
                gl::Triangles(),
                parameters.depthModeFor3D(gl::DepthMode::ReadWrite),
                gl::StencilMode::disabled(),
                parameters.colorModeForRenderPass(),
                *tileBucket.indexBuffer,
                tileBucket.triangleSegments,
                allUniformValues,
                allAttributeBindings,
                getID());
        };

        if (evaluated.get<FillExtrusionPattern>().from.empty()) {
            for (const RenderTile& tile : renderTiles) {
                auto bucket_ = tile.tile.getBucket<FillExtrusionBucket>(*baseImpl);
                if (!bucket_) {
                    continue;
                }
                FillExtrusionBucket& bucket = *bucket_;

                draw(
                    parameters.programs.fillExtrusion.get(evaluated),
                    bucket,
                    FillExtrusionUniforms::values(
                        tile.translatedClipMatrix(evaluated.get<FillExtrusionTranslate>(),
                                                  evaluated.get<FillExtrusionTranslateAnchor>(),
                                                  parameters.state),
                        parameters.state,
                        parameters.evaluatedLight
                    )
                );
            }
        } else {
            optional<ImagePosition> imagePosA =
                parameters.imageManager.getPattern(evaluated.get<FillExtrusionPattern>().from);
            optional<ImagePosition> imagePosB =
                parameters.imageManager.getPattern(evaluated.get<FillExtrusionPattern>().to);

            if (!imagePosA || !imagePosB) {
                return;
            }

            parameters.imageManager.bind(parameters.context, 0);

            for (const RenderTile& tile : renderTiles) {
                auto bucket_ = tile.tile.getBucket<FillExtrusionBucket>(*baseImpl);
                if (!bucket_) {
                    continue;
                }
                FillExtrusionBucket& bucket = *bucket_;

                draw(
                    parameters.programs.fillExtrusionPattern.get(evaluated),
                    bucket,
                    FillExtrusionPatternUniforms::values(
                        tile.translatedClipMatrix(evaluated.get<FillExtrusionTranslate>(),
                                                  evaluated.get<FillExtrusionTranslateAnchor>(),
                                                  parameters.state),
                        parameters.imageManager.getPixelSize(), *imagePosA, *imagePosB,
                        evaluated.get<FillExtrusionPattern>(), tile.id, parameters.state,
                        -std::pow(2, tile.id.canonical.z) / util::tileSize / 8.0f,
                        parameters.evaluatedLight
                    )
                );
            }
        }

    } else if (parameters.pass == RenderPass::Translucent) {
        parameters.context.bindTexture(renderTexture->getTexture());

        const auto& size = parameters.staticData.backendSize;

        mat4 viewportMat;
        matrix::ortho(viewportMat, 0, size.width, size.height, 0, 0, 1);

        const Properties<>::PossiblyEvaluated properties;
        const ExtrusionTextureProgram::PaintPropertyBinders paintAttributeData{ properties, 0 };
        
        auto& programInstance = parameters.programs.extrusionTexture;

        const auto allUniformValues = programInstance.computeAllUniformValues(
            ExtrusionTextureProgram::UniformValues{
                uniforms::u_matrix::Value{ viewportMat }, uniforms::u_world::Value{ size },
                uniforms::u_image::Value{ 0 },
                uniforms::u_opacity::Value{ evaluated.get<FillExtrusionOpacity>() }
            },
            paintAttributeData,
            properties,
            parameters.state.getZoom()
        );
        const auto allAttributeBindings = programInstance.computeAllAttributeBindings(
            parameters.staticData.extrusionTextureVertexBuffer,
            paintAttributeData,
            properties
        );

        checkRenderability(parameters, programInstance.activeBindingCount(allAttributeBindings));

        programInstance.draw(
            parameters.context,
            gl::Triangles(),
            gl::DepthMode::disabled(),
            gl::StencilMode::disabled(),
            parameters.colorModeForRenderPass(),
            parameters.staticData.quadTriangleIndexBuffer,
            parameters.staticData.extrusionTextureSegments,
            allUniformValues,
            allAttributeBindings,
            getID());
    }
}

bool RenderFillExtrusionLayer::queryIntersectsFeature(
        const GeometryCoordinates& queryGeometry,
        const GeometryTileFeature& feature,
        const float,
        const TransformState& transformState,
        const float pixelsToTileUnits,
        const mat4&) const {

    auto translatedQueryGeometry = FeatureIndex::translateQueryGeometry(
            queryGeometry,
            evaluated.get<style::FillExtrusionTranslate>(),
            evaluated.get<style::FillExtrusionTranslateAnchor>(),
            transformState.getAngle(),
            pixelsToTileUnits);

    return util::polygonIntersectsMultiPolygon(translatedQueryGeometry.value_or(queryGeometry), feature.getGeometries());
}

} // namespace mbgl
