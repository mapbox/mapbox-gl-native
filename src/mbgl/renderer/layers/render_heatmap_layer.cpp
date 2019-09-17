#include <mbgl/renderer/layers/render_heatmap_layer.hpp>
#include <mbgl/renderer/buckets/heatmap_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/render_static_data.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/heatmap_program.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/style/layers/heatmap_layer.hpp>
#include <mbgl/style/layers/heatmap_layer_impl.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/gfx/cull_face_mode.hpp>
#include <mbgl/gfx/render_pass.hpp>
#include <mbgl/gfx/context.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/intersection_tests.hpp>

namespace mbgl {

using namespace style;

namespace {

inline const HeatmapLayer::Impl& impl(const Immutable<Layer::Impl>& impl) {
    assert(impl->getTypeInfo() == HeatmapLayer::Impl::staticTypeInfo());
    return static_cast<const HeatmapLayer::Impl&>(*impl);
}

} // namespace

RenderHeatmapLayer::RenderHeatmapLayer(Immutable<HeatmapLayer::Impl> _impl)
    : RenderLayer(makeMutable<HeatmapLayerProperties>(std::move(_impl))),
    unevaluated(impl(baseImpl).paint.untransitioned()), colorRamp({256, 1}) {
}

RenderHeatmapLayer::~RenderHeatmapLayer() = default;

void RenderHeatmapLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl(baseImpl).paint.transitioned(parameters, std::move(unevaluated));
    updateColorRamp();
}

void RenderHeatmapLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    auto properties = makeMutable<HeatmapLayerProperties>(
        staticImmutableCast<HeatmapLayer::Impl>(baseImpl),
        unevaluated.evaluate(parameters));

    passes = (properties->evaluated.get<style::HeatmapOpacity>() > 0)
            ? (RenderPass::Translucent | RenderPass::Pass3D)
            : RenderPass::None;
    properties->renderPasses = mbgl::underlying_type(passes);
    evaluatedProperties = std::move(properties);
}

bool RenderHeatmapLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

bool RenderHeatmapLayer::hasCrossfade() const {
    return false;
}

void RenderHeatmapLayer::upload(gfx::UploadPass& uploadPass) {
    if (!colorRampTexture) {
        colorRampTexture =
            uploadPass.createTexture(colorRamp, gfx::TextureChannelDataType::UnsignedByte);
    }
}

void RenderHeatmapLayer::render(PaintParameters& parameters) {
    assert(renderTiles);
    if (parameters.pass == RenderPass::Opaque) {
        return;
    }

    if (parameters.pass == RenderPass::Pass3D) {
        const auto& viewportSize = parameters.staticData.backendSize;
        const auto size = Size{viewportSize.width / 4, viewportSize.height / 4};

        assert(colorRampTexture);

        if (!renderTexture || renderTexture->getSize() != size) {
            renderTexture.reset();
            if (parameters.context.supportsHalfFloatTextures) {
                renderTexture = parameters.context.createOffscreenTexture(size, gfx::TextureChannelDataType::HalfFloat);

                if (!renderTexture->isRenderable()) {
                    // can't render to a half-float texture; falling back to unsigned byte one
                    renderTexture.reset();
                    parameters.context.supportsHalfFloatTextures = false;
                }
            }

            if (!renderTexture) {
                renderTexture = parameters.context.createOffscreenTexture(size, gfx::TextureChannelDataType::UnsignedByte);
            }
        }

        auto renderPass = parameters.encoder->createRenderPass(
            "heatmap texture", { *renderTexture, Color{ 0.0f, 0.0f, 0.0f, 1.0f }, {}, {} });

        for (const RenderTile& tile : *renderTiles) {
            const LayerRenderData* renderData = getRenderDataForPass(tile, parameters.pass);
            if (!renderData) {
                continue;
            }
            auto& bucket = static_cast<HeatmapBucket&>(*renderData->bucket);
            const auto& evaluated = getEvaluated<HeatmapLayerProperties>(renderData->layerProperties);

            const auto extrudeScale = tile.id.pixelsToTileUnits(1, parameters.state.getZoom());

            const auto& paintPropertyBinders = bucket.paintPropertyBinders.at(getID());

            auto& programInstance = parameters.programs.getHeatmapLayerPrograms().heatmap;

            const auto allUniformValues = programInstance.computeAllUniformValues(
                HeatmapProgram::LayoutUniformValues {
                    uniforms::intensity::Value( evaluated.get<style::HeatmapIntensity>() ),
                    uniforms::matrix::Value( tile.matrix ),
                    uniforms::heatmap::extrude_scale::Value( extrudeScale )
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
                *renderPass,
                gfx::Triangles(),
                parameters.depthModeForSublayer(0, gfx::DepthMaskType::ReadOnly),
                gfx::StencilMode::disabled(),
                gfx::ColorMode::additive(),
                gfx::CullFaceMode::disabled(),
                *bucket.indexBuffer,
                bucket.segments,
                allUniformValues,
                allAttributeBindings,
                HeatmapProgram::TextureBindings{},
                getID()
            );
        }

    } else if (parameters.pass == RenderPass::Translucent) {
        const auto& size = parameters.staticData.backendSize;

        mat4 viewportMat;
        matrix::ortho(viewportMat, 0, size.width, size.height, 0, 0, 1);

        const Properties<>::PossiblyEvaluated properties;
        const HeatmapTextureProgram::Binders paintAttributeData{ properties, 0 };

        auto& programInstance = parameters.programs.getHeatmapLayerPrograms().heatmapTexture;

        const auto allUniformValues = programInstance.computeAllUniformValues(
            HeatmapTextureProgram::LayoutUniformValues{
                uniforms::matrix::Value( viewportMat ),
                uniforms::world::Value( size ),
                uniforms::opacity::Value( getEvaluated<HeatmapLayerProperties>(evaluatedProperties).get<HeatmapOpacity>() )
            },
            paintAttributeData,
            properties,
            parameters.state.getZoom()
        );
        const auto allAttributeBindings = programInstance.computeAllAttributeBindings(
            *parameters.staticData.heatmapTextureVertexBuffer,
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
            *parameters.staticData.quadTriangleIndexBuffer,
            parameters.staticData.heatmapTextureSegments,
            allUniformValues,
            allAttributeBindings,
            HeatmapTextureProgram::TextureBindings{
                textures::image::Value{ renderTexture->getTexture().getResource(), gfx::TextureFilterType::Linear },
                textures::color_ramp::Value{ colorRampTexture->getResource(), gfx::TextureFilterType::Linear },
            },
            getID()
        );
    }
}

void RenderHeatmapLayer::updateColorRamp() {
    auto colorValue = unevaluated.get<HeatmapColor>().getValue();
    if (colorValue.isUndefined()) {
        colorValue = HeatmapLayer::getDefaultHeatmapColor();
    }

    const auto length = colorRamp.bytes();

    for (uint32_t i = 0; i < length; i += 4) {
        const auto color = colorValue.evaluate(static_cast<double>(i) / length);
        colorRamp.data[i + 0] = std::floor(color.r * 255);
        colorRamp.data[i + 1] = std::floor(color.g * 255);
        colorRamp.data[i + 2] = std::floor(color.b * 255);
        colorRamp.data[i + 3] = std::floor(color.a * 255);
    }

    if (colorRampTexture) {
        colorRampTexture = nullopt;
    }
}

bool RenderHeatmapLayer::queryIntersectsFeature(const GeometryCoordinates& queryGeometry,
                                                const GeometryTileFeature& feature, const float zoom,
                                                const TransformState&, const float pixelsToTileUnits, const mat4&,
                                                const FeatureState&) const {
    (void) queryGeometry;
    (void) feature;
    (void) zoom;
    (void) pixelsToTileUnits;
    return false;
}

} // namespace mbgl
