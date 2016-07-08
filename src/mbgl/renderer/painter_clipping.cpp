#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/util/clip_id.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/gl/debugging.hpp>

namespace mbgl {


void Painter::drawClippingMasks(PaintParameters& parameters, const std::map<UnwrappedTileID, ClipID>& stencils) {
    MBGL_DEBUG_GROUP("clipping masks");

    auto& plainShader = parameters.shaders.plain;
    auto& arrayCoveringPlain = parameters.shaders.coveringPlainArray;

    mat4 matrix;
    const GLuint mask = 0b11111111;

    config.program = plainShader.getID();
    config.stencilOp.reset();
    config.stencilTest = GL_TRUE;
    config.depthTest = GL_FALSE;
    config.depthMask = GL_FALSE;
    config.colorMask = { GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE };
    config.stencilMask = mask;

    arrayCoveringPlain.bind(plainShader, tileStencilBuffer, BUFFER_OFFSET_0, store);

    for (const auto& stencil : stencils) {
        const auto& id = stencil.first;
        const auto& clip = stencil.second;

        MBGL_DEBUG_GROUP(std::string{ "mask: " } + util::toString(id));
        state.matrixFor(matrix, id);
        matrix::multiply(matrix, projMatrix, matrix);
        plainShader.u_matrix = matrix;

        const GLint ref = (GLint)(clip.reference.to_ulong());
        config.stencilFunc = { GL_ALWAYS, ref, mask };
        MBGL_CHECK_ERROR(glDrawArrays(GL_TRIANGLES, 0, (GLsizei)tileStencilBuffer.index()));
    }
}

} // namespace mbgl
