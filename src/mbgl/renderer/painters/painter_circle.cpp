#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/buckets/circle_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/layers/render_circle_layer.hpp>
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

    const CirclePaintProperties::PossiblyEvaluated& properties = layer.evaluated;
    const bool scaleWithMap = properties.get<CirclePitchScale>() == CirclePitchScaleType::Map;
    const bool pitchWithMap = properties.get<CirclePitchAlignment>() == AlignmentType::Map;

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
            uniforms::u_extrude_scale::Value{ pitchWithMap
                ? std::array<float, 2> {{
                    tile.id.pixelsToTileUnits(1, state.getZoom()),
                    tile.id.pixelsToTileUnits(1, state.getZoom()) }}
                : pixelsToGLUnits },
            uniforms::u_camera_to_center_distance::Value{ state.getCameraToCenterDistance() },
            uniforms::u_pitch_with_map::Value{ pitchWithMap }
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
