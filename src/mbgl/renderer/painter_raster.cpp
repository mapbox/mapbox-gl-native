#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/renderer/render_raster_layer.hpp>
#include <mbgl/style/layers/raster_layer_impl.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/raster_program.hpp>

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
                           const RenderRasterLayer& layer,
                           const RenderTile& tile) {
    if (pass != RenderPass::Translucent)
        return;
    if (!bucket.hasData())
        return;

    const RasterPaintProperties::Evaluated& properties = layer.evaluated;
    const RasterProgram::PaintPropertyBinders paintAttributeData(properties, 0);

    assert(bucket.texture);
    context.bindTexture(*bucket.texture, 0, gl::TextureFilter::Linear);
    context.bindTexture(*bucket.texture, 1, gl::TextureFilter::Linear);

    parameters.programs.raster.draw(
        context,
        gl::Triangles(),
        depthModeForSublayer(0, gl::DepthMode::ReadOnly),
        gl::StencilMode::disabled(),
        colorModeForRenderPass(),
        RasterProgram::UniformValues {
            uniforms::u_matrix::Value{ tile.matrix },
            uniforms::u_image0::Value{ 0 },
            uniforms::u_image1::Value{ 1 },
            uniforms::u_opacity::Value{ properties.get<RasterOpacity>() },
            uniforms::u_fade_t::Value{ 1 },
            uniforms::u_brightness_low::Value{ properties.get<RasterBrightnessMin>() },
            uniforms::u_brightness_high::Value{ properties.get<RasterBrightnessMax>() },
            uniforms::u_saturation_factor::Value{ saturationFactor(properties.get<RasterSaturation>()) },
            uniforms::u_contrast_factor::Value{ contrastFactor(properties.get<RasterContrast>()) },
            uniforms::u_spin_weights::Value{ spinWeights(properties.get<RasterHueRotate>()) },
            uniforms::u_buffer_scale::Value{ 1.0f },
            uniforms::u_scale_parent::Value{ 1.0f },
            uniforms::u_tl_parent::Value{ std::array<float, 2> {{ 0.0f, 0.0f }} },
        },
        rasterVertexBuffer,
        quadTriangleIndexBuffer,
        rasterSegments,
        paintAttributeData,
        properties,
        state.getZoom(),
        layer.getID()
    );
}

} // namespace mbgl
