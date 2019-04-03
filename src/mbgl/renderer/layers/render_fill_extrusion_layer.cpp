#include <mbgl/renderer/layers/render_fill_extrusion_layer.hpp>
#include <mbgl/renderer/buckets/fill_extrusion_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/renderer/render_static_data.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/fill_extrusion_program.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/intersection_tests.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/gfx/render_pass.hpp>
#include <mbgl/gfx/cull_face_mode.hpp>

namespace mbgl {

using namespace style;

RenderFillExtrusionLayer::RenderFillExtrusionLayer(Immutable<style::FillExtrusionLayer::Impl> _impl)
    : RenderLayer(std::move(_impl)),
      unevaluated(impl().paint.untransitioned()) {
}

const style::FillExtrusionLayer::Impl& RenderFillExtrusionLayer::impl() const {
    return static_cast<const style::FillExtrusionLayer::Impl&>(*baseImpl);
}

void RenderFillExtrusionLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl().paint.transitioned(parameters, std::move(unevaluated));
}

void RenderFillExtrusionLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    evaluated = unevaluated.evaluate(parameters);
    crossfade = parameters.getCrossfadeParameters();

    passes = (evaluated.get<style::FillExtrusionOpacity>() > 0)
                 ? (RenderPass::Translucent | RenderPass::Pass3D)
                 : RenderPass::None;
}

bool RenderFillExtrusionLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

bool RenderFillExtrusionLayer::hasCrossfade() const {
    return crossfade.t != 1;
}

void RenderFillExtrusionLayer::render(PaintParameters& parameters, RenderSource*) {
    if (parameters.pass == RenderPass::Opaque) {
        return;
    }

    if (parameters.pass == RenderPass::Pass3D) {
        const auto& size = parameters.staticData.backendSize;

        if (!renderTexture || renderTexture->getSize() != size) {
            renderTexture.reset();
            renderTexture = parameters.context.createOffscreenTexture(size, *parameters.staticData.depthRenderbuffer);
        }

        optional<float> depthClearValue = {};
        if (parameters.staticData.depthRenderbuffer->needsClearing()) depthClearValue = 1.0;
        // Flag the depth buffer as no longer needing to be cleared for the remainder of this pass.
        parameters.staticData.depthRenderbuffer->setShouldClear(false);

        auto renderPass = parameters.encoder->createRenderPass(
            "fill extrusion",
            { *renderTexture, Color{ 0.0f, 0.0f, 0.0f, 0.0f }, depthClearValue, {} });

        auto draw = [&](auto& programInstance, const auto& tileBucket, auto&& uniformValues,
                        const optional<ImagePosition>& patternPositionA,
                        const optional<ImagePosition>& patternPositionB, auto&& textureBindings) {
            const auto& paintPropertyBinders = tileBucket.paintPropertyBinders.at(getID());
            paintPropertyBinders.setPatternParameters(patternPositionA, patternPositionB, crossfade);

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
                *renderPass,
                gfx::Triangles(),
                parameters.depthModeFor3D(gfx::DepthMaskType::ReadWrite),
                gfx::StencilMode::disabled(),
                parameters.colorModeForRenderPass(),
                gfx::CullFaceMode::backCCW(),
                *tileBucket.indexBuffer,
                tileBucket.triangleSegments,
                allUniformValues,
                allAttributeBindings,
                std::move(textureBindings),
                getID());
        };

        if (unevaluated.get<FillExtrusionPattern>().isUndefined()) {
            for (const RenderTile& tile : renderTiles) {
                auto bucket_ = tile.tile.getBucket<FillExtrusionBucket>(*baseImpl);
                if (!bucket_) {
                    continue;
                }
                FillExtrusionBucket& bucket = *bucket_;

                draw(
                    parameters.programs.getFillExtrusionLayerPrograms().fillExtrusion,
                    bucket,
                    FillExtrusionProgram::layoutUniformValues(
                        tile.translatedClipMatrix(evaluated.get<FillExtrusionTranslate>(),
                                                  evaluated.get<FillExtrusionTranslateAnchor>(),
                                                  parameters.state),
                        parameters.state,
                        parameters.evaluatedLight
                    ),
                    {},
                    {},
                    FillExtrusionProgram::TextureBindings{}
                );
            }
        } else {
            for (const RenderTile& tile : renderTiles) {
                auto bucket_ = tile.tile.getBucket<FillExtrusionBucket>(*baseImpl);
                if (!bucket_) {
                    continue;
                }
                const auto fillPatternValue = evaluated.get<FillExtrusionPattern>().constantOr(mbgl::Faded<std::basic_string<char> >{"", ""});
                auto& geometryTile = static_cast<GeometryTile&>(tile.tile);
                optional<ImagePosition> patternPosA = geometryTile.getPattern(fillPatternValue.from);
                optional<ImagePosition> patternPosB = geometryTile.getPattern(fillPatternValue.to);
                FillExtrusionBucket& bucket = *bucket_;

                draw(
                    parameters.programs.getFillExtrusionLayerPrograms().fillExtrusionPattern,
                    bucket,
                    FillExtrusionPatternProgram::layoutUniformValues(
                        tile.translatedClipMatrix(evaluated.get<FillExtrusionTranslate>(),
                                                  evaluated.get<FillExtrusionTranslateAnchor>(),
                                                  parameters.state),
                        geometryTile.iconAtlasTexture->size,
                        crossfade,
                        tile.id,
                        parameters.state,
                        -std::pow(2, tile.id.canonical.z) / util::tileSize / 8.0f,
                        parameters.pixelRatio,
                        parameters.evaluatedLight
                    ),
                    patternPosA,
                    patternPosB,
                    FillExtrusionPatternProgram::TextureBindings{
                        textures::image::Value{ geometryTile.iconAtlasTexture->getResource(), gfx::TextureFilterType::Linear },
                    }
                );
            }
        }

    } else if (parameters.pass == RenderPass::Translucent) {
        const auto& size = parameters.staticData.backendSize;

        mat4 viewportMat;
        matrix::ortho(viewportMat, 0, size.width, size.height, 0, 0, 1);

        const Properties<>::PossiblyEvaluated properties;
        const ExtrusionTextureProgram::Binders paintAttributeData{ properties, 0 };
        
        auto& programInstance = parameters.programs.getFillExtrusionLayerPrograms().extrusionTexture;

        const auto allUniformValues = programInstance.computeAllUniformValues(
            ExtrusionTextureProgram::LayoutUniformValues{
                uniforms::matrix::Value( viewportMat ),
                uniforms::world::Value( size ),
                uniforms::opacity::Value( evaluated.get<FillExtrusionOpacity>() )
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
            *parameters.renderPass,
            gfx::Triangles(),
            gfx::DepthMode::disabled(),
            gfx::StencilMode::disabled(),
            parameters.colorModeForRenderPass(),
            gfx::CullFaceMode::disabled(),
            parameters.staticData.quadTriangleIndexBuffer,
            parameters.staticData.extrusionTextureSegments,
            allUniformValues,
            allAttributeBindings,
            ExtrusionTextureProgram::TextureBindings{
                textures::image::Value{ renderTexture->getTexture().getResource() },
            },
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
            transformState.getBearing(),
            pixelsToTileUnits);

    return util::polygonIntersectsMultiPolygon(translatedQueryGeometry.value_or(queryGeometry), feature.getGeometries());
}

} // namespace mbgl
