#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/circle_bucket.hpp>

#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/circle_layer_impl.hpp>

#include <mbgl/shader/circle_shader.hpp>

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

    config.program = isOverdraw() ? circleShader->getOverdrawID() : circleShader->getID();

    circleShader->u_matrix = vtxMatrix;
    circleShader->u_extrude_scale = extrudeScale;
    circleShader->u_devicepixelratio = frame.pixelRatio;
    circleShader->u_color = properties.circleColor;
    circleShader->u_radius = properties.circleRadius;
    circleShader->u_blur = properties.circleBlur;
    circleShader->u_opacity = properties.circleOpacity;

    bucket.drawCircles(*circleShader, store);
}

} // namespace mbgl
