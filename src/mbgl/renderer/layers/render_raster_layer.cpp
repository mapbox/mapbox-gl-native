#include <mbgl/renderer/layers/render_raster_layer.hpp>
#include <mbgl/renderer/buckets/raster_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/sources/render_image_source.hpp>
#include <mbgl/renderer/render_static_data.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/raster_program.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/style/layers/raster_layer_impl.hpp>

namespace mbgl {

using namespace style;

RenderRasterLayer::RenderRasterLayer(Immutable<style::RasterLayer::Impl> _impl)
    : RenderLayer(style::LayerType::Raster, _impl),
      unevaluated(impl().paint.untransitioned()) {
}

const style::RasterLayer::Impl& RenderRasterLayer::impl() const {
    return static_cast<const style::RasterLayer::Impl&>(*baseImpl);
}

std::unique_ptr<Bucket> RenderRasterLayer::createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const {
    assert(false);
    return nullptr;
}

void RenderRasterLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl().paint.transitioned(parameters, std::move(unevaluated));
}

void RenderRasterLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    evaluated = unevaluated.evaluate(parameters);

    passes = evaluated.get<style::RasterOpacity>() > 0 ? RenderPass::Translucent : RenderPass::None;
}

bool RenderRasterLayer::hasTransition() const {
    return unevaluated.hasTransition();
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

void RenderRasterLayer::render(PaintParameters& parameters, RenderSource* source) {
    if (parameters.pass != RenderPass::Translucent)
        return;

    auto draw = [&] (const mat4& matrix,
                     const auto& vertexBuffer,
                     const auto& indexBuffer,
                     const auto& segments) {
        parameters.programs.raster.draw(
            parameters.context,
            gl::Triangles(),
            parameters.depthModeForSublayer(0, gl::DepthMode::ReadOnly),
            gl::StencilMode::disabled(),
            parameters.colorModeForRenderPass(),
            RasterProgram::UniformValues {
                uniforms::u_matrix::Value{ matrix },
                uniforms::u_image0::Value{ 0 },
                uniforms::u_image1::Value{ 1 },
                uniforms::u_opacity::Value{ evaluated.get<RasterOpacity>() },
                uniforms::u_fade_t::Value{ 1 },
                uniforms::u_brightness_low::Value{ evaluated.get<RasterBrightnessMin>() },
                uniforms::u_brightness_high::Value{ evaluated.get<RasterBrightnessMax>() },
                uniforms::u_saturation_factor::Value{ saturationFactor(evaluated.get<RasterSaturation>()) },
                uniforms::u_contrast_factor::Value{ contrastFactor(evaluated.get<RasterContrast>()) },
                uniforms::u_spin_weights::Value{ spinWeights(evaluated.get<RasterHueRotate>()) },
                uniforms::u_buffer_scale::Value{ 1.0f },
                uniforms::u_scale_parent::Value{ 1.0f },
                uniforms::u_tl_parent::Value{ std::array<float, 2> {{ 0.0f, 0.0f }} },
            },
            vertexBuffer,
            indexBuffer,
            segments,
            RasterProgram::PaintPropertyBinders { evaluated, 0 },
            evaluated,
            parameters.state.getZoom(),
            getID()
        );
    };

    if (RenderImageSource* imageSource = source->as<RenderImageSource>()) {
        if (imageSource->isEnabled() && imageSource->isLoaded() && !imageSource->bucket->needsUpload()) {
            RasterBucket& bucket = *imageSource->bucket;

            assert(bucket.texture);
            parameters.context.bindTexture(*bucket.texture, 0, gl::TextureFilter::Linear);
            parameters.context.bindTexture(*bucket.texture, 1, gl::TextureFilter::Linear);

            for (auto matrix_ : imageSource->matrices) {
                draw(matrix_,
                     *bucket.vertexBuffer,
                     *bucket.indexBuffer,
                     bucket.segments);
            }
        }
    } else {
        for (const RenderTile& tile : renderTiles) {
            assert(dynamic_cast<RasterBucket*>(tile.tile.getBucket(*baseImpl)));
            RasterBucket& bucket = *reinterpret_cast<RasterBucket*>(tile.tile.getBucket(*baseImpl));

            if (!bucket.hasData())
                continue;

            assert(bucket.texture);
            parameters.context.bindTexture(*bucket.texture, 0, gl::TextureFilter::Linear);
            parameters.context.bindTexture(*bucket.texture, 1, gl::TextureFilter::Linear);

            if (bucket.vertexBuffer && bucket.indexBuffer && !bucket.segments.empty()) {
                // Draw only the parts of the tile that aren't drawn by another tile in the layer.
                draw(parameters.matrixForTile(tile.id, true),
                     *bucket.vertexBuffer,
                     *bucket.indexBuffer,
                     bucket.segments);
            } else {
                // Draw the full tile.
                draw(parameters.matrixForTile(tile.id, true),
                     parameters.staticData.rasterVertexBuffer,
                     parameters.staticData.quadTriangleIndexBuffer,
                     parameters.staticData.rasterSegments);
            }
        }
    }
}

} // namespace mbgl
