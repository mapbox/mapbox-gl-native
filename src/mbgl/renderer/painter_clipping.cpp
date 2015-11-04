#include <mbgl/renderer/painter.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/shader/plain_shader.hpp>
#include <mbgl/util/clip_id.hpp>
#include <mbgl/gl/debugging.hpp>

using namespace mbgl;

void Painter::drawClippingMasks(const std::set<Source*>& sources) {
    MBGL_DEBUG_GROUP("clipping masks");

    config.program = plainShader->program;
    config.stencilTest = true;
    config.depthTest = true;
    config.depthMask = GL_FALSE;
    config.colorMask = { false, false, false, false };
    config.depthRange = { 1.0f, 1.0f };

    coveringPlainArray.bind(*plainShader, tileStencilBuffer, BUFFER_OFFSET_0);

    for (const auto& source : sources) {
        source->drawClippingMasks(*this);
    }

    config.depthTest = true;
    config.colorMask = { true, true, true, true };
    config.depthMask = GL_TRUE;
    config.stencilMask = 0x0;
}

void Painter::drawClippingMask(const mat4& matrix, const ClipID &clip) {
    plainShader->u_matrix = matrix;

    const GLint ref = (GLint)(clip.reference.to_ulong());
    const GLuint mask = (GLuint)(clip.mask.to_ulong());
    config.stencilFunc = { GL_ALWAYS, ref, mask };
    config.stencilMask = mask;
    MBGL_CHECK_ERROR(glDrawArrays(GL_TRIANGLES, 0, (GLsizei)tileStencilBuffer.index()));
}
