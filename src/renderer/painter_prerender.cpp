#include <mbgl/renderer/painter.hpp>
#include <mbgl/style/style_properties.hpp>

using namespace mbgl;

void Painter::preparePrerender(PrerenderedTexture &texture) {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

// Render the actual tile.
#if GL_EXT_discard_framebuffer
    const GLenum discards[] = {GL_COLOR_ATTACHMENT0};
    glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, discards);
#endif
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, texture.properties.size, texture.properties.size);
}

void Painter::finishPrerender(PrerenderedTexture &/*texture*/) {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    glViewport(0, 0, gl_viewport[0], gl_viewport[1]);
}

template <typename Properties>
void Painter::renderPrerenderedTexture(PrerenderedTexture &texture, const Properties &properties) {
    const int buffer = texture.properties.buffer * 4096.0f;

    // draw the texture on a quad
    useProgram(rasterShader->program);
    rasterShader->setMatrix(matrix);
    rasterShader->setOpacity(1);

    glDepthRange(strata, 1.0f);

    glActiveTexture(GL_TEXTURE0);
    rasterShader->setImage(0);
    rasterShader->setBuffer(buffer);
    rasterShader->setOpacity(properties.opacity);
    texture.bindTexture();
    coveringRasterArray.bind(*rasterShader, tileStencilBuffer, BUFFER_OFFSET(0));
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)tileStencilBuffer.index());
}

template void Painter::renderPrerenderedTexture(PrerenderedTexture &, const FillProperties &);
