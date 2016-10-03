#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/circle_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>

#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/circle_layer_impl.hpp>

#include <mbgl/shader/shaders.hpp>

namespace mbgl {

using namespace style;

void Painter::renderCircle(PaintParameters& parameters,
                           CircleBucket& bucket,
                           const CircleLayer& layer,
                           const RenderTile& tile) {
    // Abort early.
    if (pass == RenderPass::Opaque) return;

    context.stencilTest = frame.mapMode == MapMode::Still;
    context.depthFunc = gl::DepthTestFunction::LessEqual;
    context.depthTest = true;
    context.depthMask = false;
    setDepthSublayer(0);

    const CirclePaintProperties& properties = layer.impl->paint;
    auto& circleShader = parameters.shaders.circle;

    context.program = circleShader.getID();

    circleShader.u_matrix = tile.translatedMatrix(properties.circleTranslate,
                                                  properties.circleTranslateAnchor,
                                                  state);

    if (properties.circlePitchScale == CirclePitchScaleType::Map) {
        circleShader.u_extrude_scale = {{
            pixelsToGLUnits[0] * state.getAltitude(),
            pixelsToGLUnits[1] * state.getAltitude()
        }};
        circleShader.u_scale_with_map = true;
    } else {
        circleShader.u_extrude_scale = pixelsToGLUnits;
        circleShader.u_scale_with_map = false;
    }

    circleShader.u_devicepixelratio = frame.pixelRatio;
    circleShader.u_color = properties.circleColor;
    circleShader.u_radius = properties.circleRadius;
    circleShader.u_blur = properties.circleBlur;
    circleShader.u_opacity = properties.circleOpacity;

    bucket.drawCircles(circleShader, context, paintMode());
}

} // namespace mbgl
