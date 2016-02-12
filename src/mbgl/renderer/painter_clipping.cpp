#include <mbgl/renderer/painter.hpp>
#include <mbgl/source/source.hpp>
#include <mbgl/shader/plain_shader.hpp>
#include <mbgl/util/clip_id.hpp>
#include <mbgl/gl/debugging.hpp>

using namespace mbgl;


void Painter::drawClippingMasks(const std::map<TileID, ClipID>& stencils) {
    MBGL_DEBUG_GROUP("clipping masks");

    mat4 matrix;
    const GLuint mask = 0b11111111;

    config.program = plainShader->program;
    config.stencilOp.reset();
    config.stencilTest = GL_TRUE;
    config.depthTest = GL_FALSE;
    config.depthMask = GL_FALSE;
    config.colorMask = { GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE };
    config.stencilMask = mask;

    coveringPlainArray.bind(*plainShader, tileStencilBuffer, BUFFER_OFFSET_0);

    for (const auto& stencil : stencils) {
        const auto& id = stencil.first;
        const auto& clip = stencil.second;

        MBGL_DEBUG_GROUP(std::string{ "mask: " } + std::string(id));
        state.matrixFor(matrix, id, id.z);
        matrix::multiply(matrix, projMatrix, matrix);
        plainShader->u_matrix = matrix;

        const GLint ref = (GLint)(clip.reference.to_ulong());
        config.stencilFunc = { GL_ALWAYS, ref, mask };
        MBGL_CHECK_ERROR(glDrawArrays(GL_TRIANGLES, 0, (GLsizei)tileStencilBuffer.index()));
    }
}
