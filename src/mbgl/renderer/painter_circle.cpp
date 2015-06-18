#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/circle_bucket.hpp>

#include <mbgl/style/style.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/style/style_layout.hpp>

#include <mbgl/map/sprite.hpp>
#include <mbgl/map/tile_id.hpp>

#include <mbgl/shader/circle_shader.hpp>

using namespace mbgl;

void Painter::renderCircle(CircleBucket& bucket,
                           const StyleLayer& layer_desc,
                           const TileID& id,
                           const mat4& matrix) {
    // Abort early.
    if (pass == RenderPass::Opaque) return;

    config.stencilTest = false;

    const CircleProperties& properties = layer_desc.getProperties<CircleProperties>();
    mat4 vtxMatrix = translatedMatrix(matrix, {{ 0, 0 }}, id, TranslateAnchorType::Map);

    // Antialiasing factor: this is a minimum blur distance that serves as
    // a faux-antialiasing for the circle. since blur is a ratio of the circle's
    // size and the intent is to keep the blur at roughly 1px, the two
    // are inversely related.
    float antialiasing = 1 / pixelRatio / properties.radius;

    useProgram(circleShader->program);

    circleShader->u_matrix = vtxMatrix;
    circleShader->u_exmatrix = projMatrix;
    circleShader->u_color = properties.color;
    circleShader->u_blur = std::max(properties.blur, antialiasing);
    circleShader->u_size = properties.radius;

    bucket.drawCircles(*circleShader);
}
