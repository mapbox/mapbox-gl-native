#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/fill_bucket.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/source.hpp>
#include <mbgl/util/clip_ids.hpp>

using namespace mbgl;

void Painter::drawClippingMasks(const std::set<util::ptr<StyleSource>> &sources) {
    gl::group group("clipping masks");

    useProgram(plainShader->program);
    glDisable(GL_DEPTH_TEST);
    depthMask(false);
    glColorMask(false, false, false, false);
    depthRange(1.0f, 1.0f);

    coveringPlainArray.bind(*plainShader, tileStencilBuffer, BUFFER_OFFSET(0));

    for (const util::ptr<StyleSource> &source : sources) {
        source->source->drawClippingMasks(*this);
    }

    glEnable(GL_DEPTH_TEST);
    glColorMask(true, true, true, true);
    depthMask(true);
    glStencilMask(0x0);
}

void Painter::drawClippingMask(const mat4& matrix, const ClipID &clip) {
    plainShader->u_matrix = matrix;

    const GLint ref = (GLint)(clip.reference.to_ulong());
    const GLuint mask = (GLuint)(clip.mask.to_ulong());
    glStencilFunc(GL_ALWAYS, ref, mask);
    glStencilMask(mask);

    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)tileStencilBuffer.index());
}
