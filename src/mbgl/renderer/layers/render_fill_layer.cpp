#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/gfx/context.hpp>
#include <mbgl/gfx/cull_face_mode.hpp>
#include <mbgl/gfx/renderable.hpp>
#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/programs/fill_program.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/renderer/buckets/fill_bucket.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/renderer/layers/render_fill_layer.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/style/expression/image.hpp>
#include <mbgl/style/layers/fill_layer_impl.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/util/intersection_tests.hpp>
#include <mbgl/util/math.hpp>

namespace mbgl {

using namespace style;

namespace {

inline const FillLayer::Impl& impl(const Immutable<style::Layer::Impl>& impl) {
    assert(impl->getTypeInfo() == FillLayer::Impl::staticTypeInfo());
    return static_cast<const FillLayer::Impl&>(*impl);
}

} // namespace

RenderFillLayer::RenderFillLayer(Immutable<style::FillLayer::Impl> _impl)
    : RenderLayer(makeMutable<FillLayerProperties>(std::move(_impl))),
      unevaluated(impl(baseImpl).paint.untransitioned()) {
}

RenderFillLayer::~RenderFillLayer() = default;

void RenderFillLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl(baseImpl).paint.transitioned(parameters, std::move(unevaluated));
}

void RenderFillLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    auto properties = makeMutable<FillLayerProperties>(
        staticImmutableCast<FillLayer::Impl>(baseImpl),
        parameters.getCrossfadeParameters(),
        unevaluated.evaluate(parameters));
    auto& evaluated = properties->evaluated;

    if (unevaluated.get<style::FillOutlineColor>().isUndefined()) {
        evaluated.get<style::FillOutlineColor>() = evaluated.get<style::FillColor>();
    }

    passes = RenderPass::Translucent;

    if (!(!unevaluated.get<style::FillPattern>().isUndefined()
        || evaluated.get<style::FillColor>().constantOr(Color()).a < 1.0f
        || evaluated.get<style::FillOpacity>().constantOr(0) < 1.0f)) {
        // Supply both - evaluated based on opaquePassCutoff in render().
        passes |= RenderPass::Opaque;
    }
    properties->renderPasses = mbgl::underlying_type(passes);
    evaluatedProperties = std::move(properties);
}

bool RenderFillLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

bool RenderFillLayer::hasCrossfade() const {
    return getCrossfade<FillLayerProperties>(evaluatedProperties).t != 1;
}

void RenderFillLayer::render(PaintParameters& parameters) {
    assert(renderTiles);
    if (unevaluated.get<FillPattern>().isUndefined()) {
        parameters.renderTileClippingMasks(renderTiles);
        for (const RenderTile& tile : *renderTiles) {
            const LayerRenderData* renderData = getRenderDataForPass(tile, parameters.pass);
            if (!renderData) {
                continue;
            }
            auto& bucket = static_cast<FillBucket&>(*renderData->bucket);
            const auto& evaluated = getEvaluated<FillLayerProperties>(renderData->layerProperties);

            auto draw = [&] (auto& programInstance,
                             const auto& drawMode,
                             const auto& depthMode,
                             const auto& indexBuffer,
                             const auto& segments,
                             auto&& textureBindings) {
                const auto& paintPropertyBinders = bucket.paintPropertyBinders.at(getID());

                const auto allUniformValues = programInstance.computeAllUniformValues(
                    FillProgram::LayoutUniformValues {
                        uniforms::matrix::Value(
                            tile.translatedMatrix(evaluated.get<FillTranslate>(),
                                                  evaluated.get<FillTranslateAnchor>(),
                                                  parameters.state)
                        ),
                        uniforms::world::Value( parameters.backend.getDefaultRenderable().getSize() ),
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
                    *parameters.renderPass,
                    drawMode,
                    depthMode,
                    parameters.stencilModeForClipping(tile.id),
                    parameters.colorModeForRenderPass(),
                    gfx::CullFaceMode::disabled(),
                    indexBuffer,
                    segments,
                    allUniformValues,
                    allAttributeBindings,
                    std::move(textureBindings),
                    getID()
                );
            };

            auto fillRenderPass = (evaluated.get<FillColor>().constantOr(Color()).a >= 1.0f
                && evaluated.get<FillOpacity>().constantOr(0) >= 1.0f
                && parameters.currentLayer >= parameters.opaquePassCutoff) ? RenderPass::Opaque : RenderPass::Translucent;
            if (bucket.triangleIndexBuffer && parameters.pass == fillRenderPass) {
                draw(parameters.programs.getFillLayerPrograms().fill,
                     gfx::Triangles(),
                     parameters.depthModeForSublayer(1, parameters.pass == RenderPass::Opaque
                        ? gfx::DepthMaskType::ReadWrite
                        : gfx::DepthMaskType::ReadOnly),
                     *bucket.triangleIndexBuffer,
                     bucket.triangleSegments,
                     FillProgram::TextureBindings{});
            }

            if (evaluated.get<FillAntialias>() && parameters.pass == RenderPass::Translucent) {
                draw(parameters.programs.getFillLayerPrograms().fillOutline,
                     gfx::Lines{ 2.0f },
                     parameters.depthModeForSublayer(
                         unevaluated.get<FillOutlineColor>().isUndefined() ? 2 : 0,
                         gfx::DepthMaskType::ReadOnly),
                     *bucket.lineIndexBuffer,
                     bucket.lineSegments,
                     FillOutlineProgram::TextureBindings{});
            }
        }
    } else {
        if (parameters.pass != RenderPass::Translucent) {
            return;
        }

        parameters.renderTileClippingMasks(renderTiles);

        for (const RenderTile& tile : *renderTiles) {
            const LayerRenderData* renderData = getRenderDataForPass(tile, parameters.pass);
            if (!renderData) {
                continue;
            }
            auto& bucket = static_cast<FillBucket&>(*renderData->bucket);
            const auto& evaluated = getEvaluated<FillLayerProperties>(renderData->layerProperties);
            const auto& crossfade = getCrossfade<FillLayerProperties>(renderData->layerProperties);

            const auto& fillPatternValue = evaluated.get<FillPattern>().constantOr(Faded<expression::Image>{"", ""});
            optional<ImagePosition> patternPosA = tile.getPattern(fillPatternValue.from.id());
            optional<ImagePosition> patternPosB = tile.getPattern(fillPatternValue.to.id());

            auto draw = [&] (auto& programInstance,
                             const auto& drawMode,
                             const auto& depthMode,
                             const auto& indexBuffer,
                             const auto& segments,
                             auto&& textureBindings) {
                const auto& paintPropertyBinders = bucket.paintPropertyBinders.at(getID());
                paintPropertyBinders.setPatternParameters(patternPosA, patternPosB, crossfade);

                const auto allUniformValues = programInstance.computeAllUniformValues(
                    FillPatternProgram::layoutUniformValues(
                        tile.translatedMatrix(evaluated.get<FillTranslate>(),
                                              evaluated.get<FillTranslateAnchor>(),
                                              parameters.state),
                        parameters.backend.getDefaultRenderable().getSize(),
                        tile.getIconAtlasTexture().size,
                        crossfade,
                        tile.id,
                        parameters.state,
                        parameters.pixelRatio
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
                    *parameters.renderPass,
                    drawMode,
                    depthMode,
                    parameters.stencilModeForClipping(tile.id),
                    parameters.colorModeForRenderPass(),
                    gfx::CullFaceMode::disabled(),
                    indexBuffer,
                    segments,
                    allUniformValues,
                    allAttributeBindings,
                    std::move(textureBindings),
                    getID()
                );
            };

            if (bucket.triangleIndexBuffer) {
                draw(parameters.programs.getFillLayerPrograms().fillPattern,
                     gfx::Triangles(),
                     parameters.depthModeForSublayer(1, gfx::DepthMaskType::ReadWrite),
                     *bucket.triangleIndexBuffer,
                     bucket.triangleSegments,
                     FillPatternProgram::TextureBindings{
                         textures::image::Value{ tile.getIconAtlasTexture().getResource(), gfx::TextureFilterType::Linear },
                     });
            }
            if (evaluated.get<FillAntialias>() && unevaluated.get<FillOutlineColor>().isUndefined()) {
                draw(parameters.programs.getFillLayerPrograms().fillOutlinePattern,
                     gfx::Lines { 2.0f },
                     parameters.depthModeForSublayer(2, gfx::DepthMaskType::ReadOnly),
                     *bucket.lineIndexBuffer,
                     bucket.lineSegments,
                     FillOutlinePatternProgram::TextureBindings{
                         textures::image::Value{ tile.getIconAtlasTexture().getResource(), gfx::TextureFilterType::Linear },
                     });
            }
        }
    }
}

bool RenderFillLayer::queryIntersectsFeature(const GeometryCoordinates& queryGeometry,
                                             const GeometryTileFeature& feature, const float,
                                             const TransformState& transformState, const float pixelsToTileUnits,
                                             const mat4&, const FeatureState&) const {
    const auto& evaluated = getEvaluated<FillLayerProperties>(evaluatedProperties);
    auto translatedQueryGeometry = FeatureIndex::translateQueryGeometry(
            queryGeometry,
            evaluated.get<style::FillTranslate>(),
            evaluated.get<style::FillTranslateAnchor>(),
            transformState.getBearing(),
            pixelsToTileUnits);

    return util::polygonIntersectsMultiPolygon(translatedQueryGeometry.value_or(queryGeometry), feature.getGeometries());
}

} // namespace mbgl
