#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/circle_bucket.hpp>

#include <mbgl/layer/circle_layer.hpp>

#include <mbgl/map/sprite.hpp>
#include <mbgl/map/tile_id.hpp>
#include <mbgl/map/map_data.hpp>

#include <mbgl/shader/circle_shader.hpp>

using namespace mbgl;

void Painter::renderCircle(CircleBucket& bucket,
                           const CircleLayer& layer,
                           const TileID& id,
                           const mat4& matrix) {
    // Abort early.
    if (pass == RenderPass::Opaque) return;

    config.stencilTest = false;

    const CirclePaintProperties& properties = layer.properties;
    mat4 vtxMatrix = translatedMatrix(matrix, properties.translate, id, properties.translateAnchor);

    Color color = properties.color;
    color[0] *= properties.opacity;
    color[1] *= properties.opacity;
    color[2] *= properties.opacity;
    color[3] *= properties.opacity;

    // Antialiasing factor: this is a minimum blur distance that serves as
    // a faux-antialiasing for the circle. since blur is a ratio of the circle's
    // size and the intent is to keep the blur at roughly 1px, the two
    // are inversely related.
    float antialiasing = 1 / data.pixelRatio / properties.radius;

    config.program = circleShader->program;

    circleShader->u_matrix = vtxMatrix;
    circleShader->u_exmatrix = extrudeMatrix;
    circleShader->u_color = color;
    circleShader->u_blur = std::max(properties.blur, antialiasing);
    circleShader->u_size = properties.radius;

    bucket.drawCircles(*circleShader);
}
