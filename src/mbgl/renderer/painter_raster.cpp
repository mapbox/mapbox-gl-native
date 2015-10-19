#include <mbgl/renderer/painter.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/layer/raster_layer.hpp>
#include <mbgl/shader/raster_shader.hpp>

using namespace mbgl;

void Painter::renderRaster(RasterBucket& bucket, const RasterLayer& layer, const TileID&, const mat4& matrix) {
    if (pass != RenderPass::Translucent) return;

    const RasterPaintProperties& properties = layer.properties;

    if (bucket.hasData()) {
        useProgram(rasterShader->program);
        rasterShader->u_matrix = matrix;
        rasterShader->u_buffer = 0;
        rasterShader->u_opacity = properties.opacity;
        rasterShader->u_brightness_low = properties.brightness[0];
        rasterShader->u_brightness_high = properties.brightness[1];
        rasterShader->u_saturation_factor = saturationFactor(properties.saturation);
        rasterShader->u_contrast_factor = contrastFactor(properties.contrast);
        rasterShader->u_spin_weights = spinWeights(properties.hue_rotate);

        config.stencilTest = true;
        config.depthTest = true;
        setDepthSublayer(0);
        bucket.drawRaster(*rasterShader, tileStencilBuffer, coveringRasterArray);
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
    spin *= M_PI / 180;
    float s = std::sin(spin);
    float c = std::cos(spin);
    std::array<float, 3> spin_weights = {{
        (2 * c + 1) / 3,
        (-std::sqrt(3.0f) * s - c + 1) / 3,
        (std::sqrt(3.0f) * s - c + 1) / 3
    }};
    return spin_weights;
}
