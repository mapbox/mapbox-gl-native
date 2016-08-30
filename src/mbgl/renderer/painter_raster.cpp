#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/raster_layer_impl.hpp>
#include <mbgl/shader/shaders.hpp>

namespace mbgl {

using namespace style;

void Painter::renderRaster(PaintParameters& parameters,
                           RasterBucket& bucket,
                           const RasterLayer& layer,
                           const RenderTile& tile) {
    if (pass != RenderPass::Translucent) return;

    const RasterPaintProperties& properties = layer.impl->paint;

    if (bucket.hasData()) {
        auto& rasterShader = parameters.shaders.raster;
        auto& rasterVAO = parameters.shaders.coveringRasterArray;

        config.program = rasterShader.getID();
        rasterShader.u_matrix = tile.matrix;
        rasterShader.u_buffer_scale = 1.0f;
        rasterShader.u_opacity0 = properties.rasterOpacity;
        rasterShader.u_opacity1 = 0;

        rasterShader.u_brightness_low = properties.rasterBrightnessMin;
        rasterShader.u_brightness_high = properties.rasterBrightnessMax;
        rasterShader.u_saturation_factor = saturationFactor(properties.rasterSaturation);
        rasterShader.u_contrast_factor = contrastFactor(properties.rasterContrast);
        rasterShader.u_spin_weights = spinWeights(properties.rasterHueRotate);

        config.stencilTest = GL_FALSE;

        rasterShader.u_image0 = 0; // GL_TEXTURE0
        rasterShader.u_image1 = 1; // GL_TEXTURE1
        rasterShader.u_tl_parent = {{ 0.0f, 0.0f }};
        rasterShader.u_scale_parent = 1.0f;

        config.depthFunc.reset();
        config.depthTest = GL_TRUE;
        config.depthMask = GL_FALSE;
        setDepthSublayer(0);

        bucket.drawRaster(rasterShader, rasterBoundsBuffer, rasterVAO, config, store);
    }
}

float Painter::saturationFactor(float saturation) {
    if (saturation > 0) {
        return 1 - 1 / (1.001 - saturation);
    } else {
        return -saturation;
    }
}

float Painter::contrastFactor(float contrast) {
    if (contrast > 0) {
        return 1 / (1 - contrast);
    } else {
        return 1 + contrast;
    }
}

std::array<float, 3> Painter::spinWeights(float spin) {
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

} // namespace mbgl
