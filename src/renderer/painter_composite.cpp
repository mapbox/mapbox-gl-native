#include <llmr/renderer/painter.hpp>

#include <iostream>

using namespace llmr;

void Painter::drawComposite(GLuint texture, const CompositeProperties &properties) {
    // We're doing full-screen framebuffer blending, so no need to do stencil testing here.
    glDisable(GL_STENCIL_TEST);

    // Normally, we're using premultiplied alpha, but the resulting image we've
    // just drawn is non-premultiplied. That means when blending, we have to make
    // sure that we account for that.
    glBlendFunc(GL_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    useProgram(compositeShader->program);
    compositeShader->setMatrix(nativeMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    compositeShader->setImage(0);

    // TODO: translate
    compositeShader->setOpacity(properties.opacity);

    // Draw the full screen texture.
    // TODO: Do more blend effects like multiply + blur
    compositeArray.bind(*compositeShader, tileStencilBuffer, BUFFER_OFFSET(0));
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)tileStencilBuffer.index());

    glBindTexture(GL_TEXTURE_2D, 0);

    // Reset back.
    glEnable(GL_STENCIL_TEST);

    // Reset back to use premultiplied alpha blending.
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}
