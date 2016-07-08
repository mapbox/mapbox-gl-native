#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/circle_bucket.hpp>

#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/circle_layer_impl.hpp>

#include <mbgl/shader/shaders.hpp>

namespace mbgl {

using namespace style;

void Painter::renderCircle(CircleBucket& bucket,
                           const CircleLayer& layer,
                           const UnwrappedTileID& tileID,
                           const mat4& matrix) {
    // Abort early.
    if (pass == RenderPass::Opaque) return;

    config.stencilTest = frame.mapMode == MapMode::Still ? GL_TRUE : GL_FALSE;
    config.depthFunc.reset();
    config.depthTest = GL_TRUE;
    config.depthMask = GL_FALSE;
    setDepthSublayer(0);

    const CirclePaintProperties& properties = layer.impl->paint;
    mat4 vtxMatrix = translatedMatrix(matrix, properties.circleTranslate, tileID,
                                      properties.circleTranslateAnchor);

    auto& circleShader = isOverdraw() ? overdrawShaders->circle : shaders->circle;

    config.program = circleShader.getID();

    circleShader.u_matrix = vtxMatrix;

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

    bucket.drawCircles(circleShader, store);
}

} // namespace mbgl
