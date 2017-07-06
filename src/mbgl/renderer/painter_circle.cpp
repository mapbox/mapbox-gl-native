#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/circle_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/render_circle_layer.hpp>
#include <mbgl/style/layers/circle_layer_impl.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/circle_program.hpp>
#include <mbgl/gl/context.hpp>

namespace mbgl {

using namespace style;

void Painter::renderCircle(PaintParameters& parameters,
                           CircleBucket& bucket,
                           const RenderCircleLayer& layer,
                           const RenderTile& tile) {
    if (pass == RenderPass::Opaque) {
        return;
    }

    const CirclePaintProperties::Evaluated& properties = layer.evaluated;
    const bool scaleWithMap = properties.get<CirclePitchScale>() == CirclePitchScaleType::Map;

    parameters.programs.circle.get(properties).draw(
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
        bucket.segments,
        bucket.paintPropertyBinders.at(layer.getID()),
        properties,
        state.getZoom(),
        layer.getID()
    );
}

} // namespace mbgl
