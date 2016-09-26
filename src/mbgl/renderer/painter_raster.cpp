#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/raster_layer_impl.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/shader/raster_uniforms.hpp>

namespace mbgl {

using namespace style;

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

void Painter::renderRaster(PaintParameters& parameters,
                           RasterBucket& bucket,
                           const RasterLayer& layer,
                           const RenderTile& tile) {
    if (pass != RenderPass::Translucent)
        return;
    if (!bucket.hasData())
        return;

    const RasterPaintProperties& properties = layer.impl->paint;

    assert(bucket.texture);
    context.bindTexture(*bucket.texture, 0, gl::TextureFilter::Linear);
    context.bindTexture(*bucket.texture, 1, gl::TextureFilter::Linear);

    context.draw({
        depthModeForSublayer(0, gl::DepthMode::ReadOnly),
        gl::StencilMode::disabled(),
        colorModeForRenderPass(),
        parameters.shaders.raster,
        RasterUniforms::values(
            tile.matrix,
            0,
            1,
            properties.rasterOpacity.value,
            0,
            properties.rasterBrightnessMin.value,
            properties.rasterBrightnessMax.value,
            saturationFactor(properties.rasterSaturation.value),
            contrastFactor(properties.rasterContrast.value),
            spinWeights(properties.rasterHueRotate.value),
            1.0f,
            1.0f,
            std::array<float, 2> {{ 0.0f, 0.0f }}
        ),
        gl::Unindexed<gl::TriangleStrip>(rasterVertexBuffer)
    });
}

} // namespace mbgl
