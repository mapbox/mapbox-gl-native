#include <llmr/renderer/painter.hpp>
#include <llmr/renderer/fill_bucket.hpp>
#include <llmr/map/map.hpp>
#include <llmr/map/source.hpp>
#include <llmr/util/clip_ids.hpp>

using namespace llmr;

void Painter::drawClippingMasks(const std::set<std::shared_ptr<Source>> &sources) {
    gl::group group("clipping masks");

    useProgram(plainShader->program);
    glDisable(GL_DEPTH_TEST);
    depthMask(false);
    glDepthRange(1.0f, 1.0f);
    glStencilMask(0xFF);

//    std::shared_ptr<StyleLayer> background_layer = map.getStyle()->background;
//    const BackgroundProperties &properties = background_layer ? background_layer->getProperties<BackgroundProperties>() : defaultStyleProperties<BackgroundProperties>();
    const BackgroundProperties &properties = defaultStyleProperties<BackgroundProperties>();
    plainShader->setColor(properties.color);

    coveringPlainArray.bind(*plainShader, tileStencilBuffer, BUFFER_OFFSET(0));

    for (const std::shared_ptr<Source> &source : sources) {
        source->drawClippingMasks(*this);
    }

    glEnable(GL_DEPTH_TEST);
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
