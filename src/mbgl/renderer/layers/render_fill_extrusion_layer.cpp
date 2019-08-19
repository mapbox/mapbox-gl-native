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

inline const FillExtrusionLayer::Impl& impl(const Immutable<style::Layer::Impl>& impl) {
    return static_cast<const FillExtrusionLayer::Impl&>(*impl);
}

RenderFillExtrusionLayer::RenderFillExtrusionLayer(Immutable<style::FillExtrusionLayer::Impl> _impl)
    : RenderLayer(makeMutable<FillExtrusionLayerProperties>(std::move(_impl))),
      unevaluated(impl(baseImpl).paint.untransitioned()) {
}

RenderFillExtrusionLayer::~RenderFillExtrusionLayer() = default;

void RenderFillExtrusionLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl(baseImpl).paint.transitioned(parameters, std::move(unevaluated));
}

void RenderFillExtrusionLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    auto properties = makeMutable<FillExtrusionLayerProperties>(
        staticImmutableCast<FillExtrusionLayer::Impl>(baseImpl),
        parameters.getCrossfadeParameters(),
        unevaluated.evaluate(parameters));

    passes = (properties->evaluated.get<style::FillExtrusionOpacity>() > 0)
                 ? (RenderPass::Translucent | RenderPass::Pass3D)
                 : RenderPass::None;
    properties->renderPasses = mbgl::underlying_type(passes);
    evaluatedProperties = std::move(properties);
}

bool RenderFillExtrusionLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

bool RenderFillExtrusionLayer::hasCrossfade() const {
    return getCrossfade<FillExtrusionLayerProperties>(evaluatedProperties).t != 1;
}

bool RenderFillExtrusionLayer::is3D() const {
    return true;
}

void RenderFillExtrusionLayer::render(PaintParameters& parameters) {
    assert(renderTiles);
    if (parameters.pass != RenderPass::Translucent) {
        return;
    }

    const auto& evaluated = static_cast<const FillExtrusionLayerProperties&>(*evaluatedProperties).evaluated;
    const auto& crossfade = static_cast<const FillExtrusionLayerProperties&>(*evaluatedProperties).crossfade;
    if (evaluatedProperties->renderPasses == mbgl::underlying_type(RenderPass::None)) {
        return;
    }

    const auto depthMode = parameters.depthModeFor3D();

    auto draw = [&](auto& programInstance,
                    const auto& evaluated_,
                    const auto& crossfade_,
                    const gfx::StencilMode& stencilMode,
                    const gfx::ColorMode& colorMode,
                    const auto& tileBucket,
                    const auto& uniformValues,
                    const optional<ImagePosition>& patternPositionA,
                    const optional<ImagePosition>& patternPositionB,
                    const auto& textureBindings,
                    const std::string& uniqueName) {
        const auto& paintPropertyBinders = tileBucket.paintPropertyBinders.at(getID());
        paintPropertyBinders.setPatternParameters(patternPositionA, patternPositionB, crossfade_);

        const auto allUniformValues = programInstance.computeAllUniformValues(
            uniformValues,
            paintPropertyBinders,
            evaluated_,
            parameters.state.getZoom()
        );
        const auto allAttributeBindings = programInstance.computeAllAttributeBindings(
            *tileBucket.vertexBuffer,
            paintPropertyBinders,
            evaluated_
        );

        checkRenderability(parameters, programInstance.activeBindingCount(allAttributeBindings));

        programInstance.draw(
            parameters.context,
            *parameters.renderPass,
            gfx::Triangles(),
            depthMode,
            stencilMode,
            colorMode,
            gfx::CullFaceMode::backCCW(),
            *tileBucket.indexBuffer,
            tileBucket.triangleSegments,
            allUniformValues,
            allAttributeBindings,
            textureBindings,
            getID() + "/" + uniqueName);
    };

    if (unevaluated.get<FillExtrusionPattern>().isUndefined()) {
        // Draw solid color extrusions
        auto drawTiles = [&](const gfx::StencilMode& stencilMode_, const gfx::ColorMode& colorMode_, const std::string& name) {
            for (const RenderTile& tile : *renderTiles) {
                const LayerRenderData* renderData = getRenderDataForPass(tile, parameters.pass);
                if (!renderData) {
                    continue;
                }
                auto& bucket = static_cast<FillExtrusionBucket&>(*renderData->bucket);
                draw(
                    parameters.programs.getFillExtrusionLayerPrograms().fillExtrusion,
                    evaluated,
                    crossfade,
                    stencilMode_,
                    colorMode_,
                    bucket,
                    FillExtrusionProgram::layoutUniformValues(
                        tile.translatedClipMatrix(evaluated.get<FillExtrusionTranslate>(),
                                                  evaluated.get<FillExtrusionTranslateAnchor>(),
                                                  parameters.state),
                        parameters.state,
                        evaluated.get<FillExtrusionOpacity>(),
                        parameters.evaluatedLight,
                        evaluated.get<FillExtrusionVerticalGradient>()
                    ),
                    {},
                    {},
                    FillExtrusionProgram::TextureBindings{},
                    name
                );
            }
        };

        if (evaluated.get<FillExtrusionOpacity>() == 1) {
            // Draw opaque extrusions
            drawTiles(gfx::StencilMode::disabled(), parameters.colorModeForRenderPass(), "color");
        } else {
            // Draw transparent buildings in two passes so that only the closest surface is drawn.
            // First draw all the extrusions into only the depth buffer. No colors are drawn.
            drawTiles(gfx::StencilMode::disabled(), gfx::ColorMode::disabled(), "depth");

            // Then draw all the extrusions a second time, only coloring fragments if they have the
            // same depth value as the closest fragment in the previous pass. Use the stencil buffer
            // to prevent the second draw in cases where we have coincident polygons.
            drawTiles(parameters.stencilModeFor3D(), parameters.colorModeForRenderPass(), "color");
        }
    } else {
        // Draw textured extrusions
        const auto fillPatternValue = evaluated.get<FillExtrusionPattern>().constantOr(mbgl::Faded<std::basic_string<char> >{"", ""});
        auto drawTiles = [&](const gfx::StencilMode& stencilMode_, const gfx::ColorMode& colorMode_, const std::string& name) {
            for (const RenderTile& tile : *renderTiles) {
                const LayerRenderData* renderData = getRenderDataForPass(tile, parameters.pass);
                if (!renderData) {
                    continue;
                }
                auto& bucket = static_cast<FillExtrusionBucket&>(*renderData->bucket);
                optional<ImagePosition> patternPosA = tile.getPattern(fillPatternValue.from);
                optional<ImagePosition> patternPosB = tile.getPattern(fillPatternValue.to);

                draw(
                    parameters.programs.getFillExtrusionLayerPrograms().fillExtrusionPattern,
                    evaluated,
                    crossfade,
                    stencilMode_,
                    colorMode_,
                    bucket,
                    FillExtrusionPatternProgram::layoutUniformValues(
                        tile.translatedClipMatrix(evaluated.get<FillExtrusionTranslate>(),
                                                  evaluated.get<FillExtrusionTranslateAnchor>(),
                                                  parameters.state),
                        tile.getIconAtlasTexture().size,
                        crossfade,
                        tile.id,
                        parameters.state,
                        evaluated.get<FillExtrusionOpacity>(),
                        -std::pow(2, tile.id.canonical.z) / util::tileSize / 8.0f,
                        parameters.pixelRatio,
                        parameters.evaluatedLight,
                        evaluated.get<FillExtrusionVerticalGradient>()
                    ),
                    patternPosA,
                    patternPosB,
                    FillExtrusionPatternProgram::TextureBindings{
                        textures::image::Value{ tile.getIconAtlasTexture().getResource(), gfx::TextureFilterType::Linear },
                    },
                    name
                );
            }
        };

        // Draw transparent buildings in two passes so that only the closest surface is drawn.
        // First draw all the extrusions into only the depth buffer. No colors are drawn.
        drawTiles(gfx::StencilMode::disabled(), gfx::ColorMode::disabled(), "depth");

        // Then draw all the extrusions a second time, only coloring fragments if they have the
        // same depth value as the closest fragment in the previous pass. Use the stencil buffer
        // to prevent the second draw in cases where we have coincident polygons.
        drawTiles(parameters.stencilModeFor3D(), parameters.colorModeForRenderPass(), "color");
    }
}

bool RenderFillExtrusionLayer::queryIntersectsFeature(
        const GeometryCoordinates& queryGeometry,
        const GeometryTileFeature& feature,
        const float,
        const TransformState& transformState,
        const float pixelsToTileUnits,
        const mat4&) const {
    const auto& evaluated = static_cast<const FillExtrusionLayerProperties&>(*evaluatedProperties).evaluated;
    auto translatedQueryGeometry = FeatureIndex::translateQueryGeometry(
            queryGeometry,
            evaluated.get<style::FillExtrusionTranslate>(),
            evaluated.get<style::FillExtrusionTranslateAnchor>(),
            transformState.getBearing(),
            pixelsToTileUnits);

    return util::polygonIntersectsMultiPolygon(translatedQueryGeometry.value_or(queryGeometry), feature.getGeometries());
}

} // namespace mbgl
