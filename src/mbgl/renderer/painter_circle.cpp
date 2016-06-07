#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/circle_bucket.hpp>
#include <mbgl/renderer/circle_renderable.hpp>

#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/circle_layer_impl.hpp>

#include <mbgl/shader/circle_shader.hpp>

namespace mbgl {

using namespace style;

namespace {

void drawCircles(CircleRenderable& renderable, CircleShader& shader, gl::ObjectStore& store) {
    GLbyte* vertexIndex = BUFFER_OFFSET_0;
    GLbyte* elementsIndex = BUFFER_OFFSET_0;

    for (auto& group : renderable.triangleGroups) {
        assert(group);

        if (!group->elements_length) {
            continue;
        }

        group->array[0].bind(shader, renderable.vertexBuffer, renderable.elementsBuffer,
                             vertexIndex, store);

        MBGL_CHECK_ERROR(glDrawElements(GL_TRIANGLES, group->elements_length * 3, GL_UNSIGNED_SHORT,
                                        elementsIndex));

        vertexIndex += group->vertex_length * renderable.vertexBuffer.itemSize;
        elementsIndex += group->elements_length * renderable.elementsBuffer.itemSize;
    }
}

} // namespace

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

    drawCircles(bucket.getRenderable(), *circleShader, store);
}

} // namespace mbgl
