#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/circle_bucket.hpp>

#include <mbgl/layer/circle_layer.hpp>

#include <mbgl/shader/circle_shader.hpp>

using namespace mbgl;

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

    const CirclePaintProperties& properties = layer.paint;
    mat4 vtxMatrix = translatedMatrix(matrix, properties.circleTranslate, tileID,
                                      properties.circleTranslateAnchor);

    // Antialiasing factor: this is a minimum blur distance that serves as
    // a faux-antialiasing for the circle. since blur is a ratio of the circle's
    // size and the intent is to keep the blur at roughly 1px, the two
    // are inversely related.
    float antialiasing = 1 / frame.pixelRatio / properties.circleRadius;

    config.program = circleShader->getID();

    circleShader->u_matrix = vtxMatrix;
    circleShader->u_extrude_scale = extrudeScale;
    circleShader->u_devicepixelratio = frame.pixelRatio;
    circleShader->u_color = properties.circleColor;
    circleShader->u_radius = properties.circleRadius;
    circleShader->u_blur = std::max<float>(properties.circleBlur, antialiasing);
    circleShader->u_opacity = properties.circleOpacity;

    bucket.drawCircles(*circleShader, glObjectStore);
}
