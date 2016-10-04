#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/gl/gl.hpp>

#include <mbgl/style/source.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/util/clip_id.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/gl/debugging.hpp>

namespace mbgl {


void Painter::drawClippingMasks(PaintParameters& parameters, const std::map<UnwrappedTileID, ClipID>& stencils) {
    MBGL_DEBUG_GROUP("clipping masks");

    auto& plainShader = parameters.shaders.fill;
    auto& arrayCoveringPlain = parameters.shaders.coveringPlainArray;

    mat4 matrix;
    const GLuint mask = 0b11111111;

    context.program = plainShader.getID();
    context.stencilOp = { gl::StencilTestOperation::Keep, gl::StencilTestOperation::Keep,
                          gl::StencilTestOperation::Replace };
    context.stencilTest = true;
    context.depthTest = false;
    context.depthMask = false;
    context.colorMask = { false, false, false, false };
    context.stencilMask = mask;

    arrayCoveringPlain.bind(plainShader, tileTriangleVertexBuffer, BUFFER_OFFSET_0, context);

    for (const auto& stencil : stencils) {
        const auto& id = stencil.first;
        const auto& clip = stencil.second;

        MBGL_DEBUG_GROUP(std::string{ "mask: " } + util::toString(id));
        state.matrixFor(matrix, id);
        matrix::multiply(matrix, projMatrix, matrix);
        plainShader.u_matrix = matrix;

        const GLint ref = (GLint)(clip.reference.to_ulong());
        context.stencilFunc = { gl::StencilTestFunction::Always, ref, mask };
        MBGL_CHECK_ERROR(glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(tileTriangleVertexBuffer.vertexCount)));
    }
}

} // namespace mbgl
