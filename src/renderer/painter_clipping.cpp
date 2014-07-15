#include <llmr/renderer/painter.hpp>
#include <llmr/renderer/fill_bucket.hpp>
#include <llmr/map/map.hpp>
#include <llmr/map/source.hpp>
#include <llmr/util/clip_ids.hpp>

using namespace llmr;

void Painter::drawClippingMasks(const std::set<std::shared_ptr<StyleSource>> &sources) {
    gl::group group("clipping masks");

    useProgram(plainShader->program);
    glDisable(GL_DEPTH_TEST);
    depthMask(false);
    glColorMask(false, false, false, false);
    glDepthRange(1.0f, 1.0f);
    glStencilMask(0xFF);

    coveringPlainArray.bind(*plainShader, tileStencilBuffer, BUFFER_OFFSET(0));

    for (const std::shared_ptr<StyleSource> &source : sources) {
        source->source->drawClippingMasks(*this);
    }

    glEnable(GL_DEPTH_TEST);
    glColorMask(true, true, true, true);
    depthMask(true);
    glStencilMask(0x0);
}

void Painter::drawClippingMask(const mat4& matrix, const ClipID &clip) {
    plainShader->setMatrix(matrix);

    GLint id = static_cast<GLint>(clip.mask.to_ulong());
    GLuint mask = clipMask[clip.length];
    glStencilFunc(GL_ALWAYS, id, mask);

    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)tileStencilBuffer.index());
}
