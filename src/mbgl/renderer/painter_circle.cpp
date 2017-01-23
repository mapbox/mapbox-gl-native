#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/circle_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/circle_layer_impl.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/circle_program.hpp>
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

    const CirclePaintProperties::Evaluated& properties = layer.impl->paint.evaluated;
    const bool scaleWithMap = properties.get<CirclePitchScale>() == CirclePitchScaleType::Map;

    parameters.programs.circle.draw(
        context,
        gl::Triangles(),
        depthModeForSublayer(0, gl::DepthMode::ReadOnly),
        frame.mapMode == MapMode::Still
            ? stencilModeForClipping(tile.clip)
            : gl::StencilMode::disabled(),
        colorModeForRenderPass(),
        CircleProgram::UniformValues {
            uniforms::u_matrix::Value{
                tile.translatedMatrix(properties.get<CircleTranslate>(),
                                      properties.get<CircleTranslateAnchor>(),
                                      state)
            },
            uniforms::u_opacity::Value{ properties.get<CircleOpacity>() },
            uniforms::u_color::Value{ properties.get<CircleColor>() },
            uniforms::u_radius::Value{ properties.get<CircleRadius>() },
            uniforms::u_blur::Value{ properties.get<CircleBlur>() },
            uniforms::u_stroke_color::Value{ properties.get<CircleStrokeColor>() },
            uniforms::u_stroke_width::Value{ properties.get<CircleStrokeWidth>() },
            uniforms::u_stroke_opacity::Value{ properties.get<CircleStrokeOpacity>() },
            uniforms::u_scale_with_map::Value{ scaleWithMap },
            uniforms::u_extrude_scale::Value{ scaleWithMap
                ? std::array<float, 2> {{
                    pixelsToGLUnits[0] * state.getCameraToCenterDistance(),
                    pixelsToGLUnits[1] * state.getCameraToCenterDistance()
                  }}
                : pixelsToGLUnits }
        },
        *bucket.vertexBuffer,
        *bucket.indexBuffer,
        bucket.segments
    );
}

} // namespace mbgl
