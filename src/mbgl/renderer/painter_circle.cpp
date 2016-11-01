#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/circle_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/circle_layer_impl.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/shader/circle_uniforms.hpp>
#include <mbgl/gl/context.hpp>

namespace mbgl {

using namespace style;

void Painter::renderCircle(PaintParameters& parameters,
                           CircleBucket& bucket,
                           const CircleLayer& layer,
                           const RenderTile& tile) {
    if (pass == RenderPass::Opaque) {
        return;
    }

    const CirclePaintProperties& properties = layer.impl->paint;

    context.draw({
        depthModeForSublayer(0, gl::DepthMode::ReadOnly),
        frame.mapMode == MapMode::Still
            ? stencilModeForClipping(tile.clip)
            : gl::StencilMode::disabled(),
        colorModeForRenderPass(),
        parameters.shaders.circle,
        CircleUniforms::values(
            tile.translatedMatrix(properties.circleTranslate.value,
                                  properties.circleTranslateAnchor.value,
                                  state),
            properties.circleOpacity.value,
            properties.circleColor.value,
            properties.circleRadius.value,
            properties.circleBlur.value,
            properties.circlePitchScale.value == CirclePitchScaleType::Map,
            properties.circlePitchScale.value == CirclePitchScaleType::Map
                ? std::array<float, 2> {{
                    pixelsToGLUnits[0] * state.getAltitude(),
                    pixelsToGLUnits[1] * state.getAltitude()
                  }}
                : pixelsToGLUnits,
            frame.pixelRatio
        ),
        gl::Segmented<gl::Triangles>(
            *bucket.vertexBuffer,
            *bucket.indexBuffer,
            bucket.segments
        )
    });
}

} // namespace mbgl
