#include <mbgl/renderer/layers/render_raster_layer.hpp>
#include <mbgl/renderer/buckets/raster_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/render_static_data.hpp>
#include <mbgl/renderer/sources/render_image_source.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/raster_program.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/gfx/cull_face_mode.hpp>
#include <mbgl/style/layers/raster_layer_impl.hpp>

namespace mbgl {

using namespace style;

namespace {

inline const RasterLayer::Impl& impl(const Immutable<style::Layer::Impl>& impl) {
    assert(impl->getTypeInfo() == RasterLayer::Impl::staticTypeInfo());
    return static_cast<const RasterLayer::Impl&>(*impl);
}

} // namespace

RenderRasterLayer::RenderRasterLayer(Immutable<style::RasterLayer::Impl> _impl)
    : RenderLayer(makeMutable<RasterLayerProperties>(std::move(_impl))),
      unevaluated(impl(baseImpl).paint.untransitioned()) {
}

RenderRasterLayer::~RenderRasterLayer() = default;

void RenderRasterLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl(baseImpl).paint.transitioned(parameters, std::move(unevaluated));
}

void RenderRasterLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    auto properties = makeMutable<RasterLayerProperties>(
        staticImmutableCast<RasterLayer::Impl>(baseImpl),
        unevaluated.evaluate(parameters));
    passes = properties->evaluated.get<style::RasterOpacity>() > 0 ? RenderPass::Translucent : RenderPass::None;
    properties->renderPasses = mbgl::underlying_type(passes);
    evaluatedProperties = std::move(properties);
}

bool RenderRasterLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

bool RenderRasterLayer::hasCrossfade() const {
    return false;
}

static float saturationFactor(float saturation) {
    if (saturation > 0) {
        return 1 - 1 / (1.001 - saturation);
    } else {
        return -saturation;
    }
}

static float contrastFactor(float contrast) {
    if (contrast > 0) {
        return 1 / (1 - contrast);
    } else {
        return 1 + contrast;
    }
}

static std::array<float, 3> spinWeights(float spin) {
    spin *= util::DEG2RAD;
    float s = std::sin(spin);
    float c = std::cos(spin);
    std::array<float, 3> spin_weights = {{
        (2 * c + 1) / 3,
        (-std::sqrt(3.0f) * s - c + 1) / 3,
        (std::sqrt(3.0f) * s - c + 1) / 3
    }};
    return spin_weights;
}

void RenderRasterLayer::prepare(const LayerPrepareParameters& params) {
    renderTiles = params.source->getRenderTiles();
    imageData = params.source->getImageRenderData();
    // It is possible image data is not available until the source loads it.
    assert(renderTiles || imageData || !params.source->isEnabled());
}

void RenderRasterLayer::render(PaintParameters& parameters) {
    if (parameters.pass != RenderPass::Translucent || (!renderTiles && !imageData)) {
        return;
    }
    const auto& evaluated = static_cast<const RasterLayerProperties&>(*evaluatedProperties).evaluated;
    RasterProgram::Binders paintAttributeData{ evaluated, 0 };

    auto draw = [&] (const mat4& matrix,
                     const auto& vertexBuffer,
                     const auto& indexBuffer,
                     const auto& segments,
                     const auto& textureBindings,
                     const std::string& drawScopeID) {
        auto& programInstance = parameters.programs.getRasterLayerPrograms().raster;

        const auto allUniformValues = programInstance.computeAllUniformValues(
            RasterProgram::LayoutUniformValues {
                uniforms::matrix::Value( matrix ),
                uniforms::opacity::Value( evaluated.get<RasterOpacity>() ),
                uniforms::fade_t::Value( 1 ),
                uniforms::brightness_low::Value( evaluated.get<RasterBrightnessMin>() ),
                uniforms::brightness_high::Value( evaluated.get<RasterBrightnessMax>() ),
                uniforms::saturation_factor::Value( saturationFactor(evaluated.get<RasterSaturation>()) ),
                uniforms::contrast_factor::Value( contrastFactor(evaluated.get<RasterContrast>()) ),
                uniforms::spin_weights::Value( spinWeights(evaluated.get<RasterHueRotate>()) ),
                uniforms::buffer_scale::Value( 1.0f ),
                uniforms::scale_parent::Value( 1.0f ),
                uniforms::tl_parent::Value( std::array<float, 2> {{ 0.0f, 0.0f }} ),
            },
            paintAttributeData,
            evaluated,
            parameters.state.getZoom()
        );
        const auto allAttributeBindings = programInstance.computeAllAttributeBindings(
            vertexBuffer,
            paintAttributeData,
            evaluated
        );

        checkRenderability(parameters, programInstance.activeBindingCount(allAttributeBindings));

        programInstance.draw(
            parameters.context,
            *parameters.renderPass,
            gfx::Triangles(),
            parameters.depthModeForSublayer(0, gfx::DepthMaskType::ReadOnly),
            gfx::StencilMode::disabled(),
            parameters.colorModeForRenderPass(),
            gfx::CullFaceMode::disabled(),
            indexBuffer,
            segments,
            allUniformValues,
            allAttributeBindings,
            textureBindings,
            getID() + "/" + drawScopeID
        );
    };

    const gfx::TextureFilterType filter = evaluated.get<RasterResampling>() == RasterResamplingType::Nearest ? gfx::TextureFilterType::Nearest : gfx::TextureFilterType::Linear;

    if (imageData && !imageData->bucket->needsUpload()) {
        RasterBucket& bucket = *imageData->bucket;
        assert(bucket.texture);

        size_t i = 0;
        for (const auto& matrix_ : imageData->matrices) {
            draw(matrix_,
                *bucket.vertexBuffer,
                *bucket.indexBuffer,
                bucket.segments,
                RasterProgram::TextureBindings{
                    textures::image0::Value{ bucket.texture->getResource(), filter },
                    textures::image1::Value{ bucket.texture->getResource(), filter },
                },
                bucket.drawScopeID + std::to_string(i++));
        }
    } else if (renderTiles) {
        for (const RenderTile& tile : *renderTiles) {
            auto* bucket_ = tile.getBucket(*baseImpl);
            if (!bucket_) {
                continue;
            }
            auto& bucket = static_cast<RasterBucket&>(*bucket_);

            if (!bucket.hasData())
                continue;

            assert(bucket.texture);
            if (bucket.vertexBuffer && bucket.indexBuffer && !bucket.segments.empty()) {
                // Draw only the parts of the tile that aren't drawn by another tile in the layer.
                draw(parameters.matrixForTile(tile.id, true),
                     *bucket.vertexBuffer,
                     *bucket.indexBuffer,
                     bucket.segments,
                     RasterProgram::TextureBindings{
                         textures::image0::Value{ bucket.texture->getResource(), filter },
                         textures::image1::Value{ bucket.texture->getResource(), filter },
                     },
                     bucket.drawScopeID);
            } else {
                // Draw the full tile.
                draw(parameters.matrixForTile(tile.id, true),
                     *parameters.staticData.rasterVertexBuffer,
                     *parameters.staticData.quadTriangleIndexBuffer,
                     parameters.staticData.rasterSegments,
                     RasterProgram::TextureBindings{
                         textures::image0::Value{ bucket.texture->getResource(), filter },
                         textures::image1::Value{ bucket.texture->getResource(), filter },
                     },
                     bucket.drawScopeID);
            }
        }
    }
}

} // namespace mbgl
