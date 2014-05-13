#include <llmr/renderer/painter.hpp>
#include <llmr/renderer/fill_bucket.hpp>
#include <llmr/map/map.hpp>
#include <llmr/util/clip_ids.hpp>

using namespace llmr;

void Painter::prepareClippingMask() {
    gl::start_group("clipping masks");
    useProgram(plainShader->program);
    glDisable(GL_DEPTH_TEST);
    depthMask(false);
    glDepthRange(1.0f, 1.0f);

    coveringPlainArray.bind(*plainShader, tileStencilBuffer, BUFFER_OFFSET(0));
}

void Painter::drawClippingMask(const mat4& matrix, const ClipID &clip) {
    plainShader->setMatrix(matrix);

    GLint id = clip.mask.to_ulong();
    GLuint mask = clipMask[clip.length];
    glStencilFunc(GL_ALWAYS, id, mask);
    glStencilMask(0xFF);

    Color background = map.getStyle().computed.background.color;
    background[0] *= map.getStyle().computed.background.opacity;
    background[1] *= map.getStyle().computed.background.opacity;
    background[2] *= map.getStyle().computed.background.opacity;
    background[3] *= map.getStyle().computed.background.opacity;

    plainShader->setColor(background);

    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)tileStencilBuffer.index());
}

void Painter::finishClippingMask() {
    glEnable(GL_DEPTH_TEST);
    depthMask(true);
    glStencilMask(0x0);
    gl::end_group();
}
