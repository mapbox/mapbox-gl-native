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

    const auto& shader = isOverdraw() ? circleOverdrawShader : circleShader;

    config.program = shader->getID();

    shader->u_matrix = vtxMatrix;
    shader->u_extrude_scale = extrudeScale;
    shader->u_devicepixelratio = frame.pixelRatio;
    shader->u_color = properties.circleColor;
    shader->u_radius = properties.circleRadius;
    shader->u_blur = properties.circleBlur;
    shader->u_opacity = properties.circleOpacity;

    bucket.drawCircles(*shader, store);
}

} // namespace mbgl
