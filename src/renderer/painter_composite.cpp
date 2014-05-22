#include <llmr/renderer/painter.hpp>

#include <iostream>

using namespace llmr;

void Painter::drawComposite(GLuint texture, const CompositeProperties &properties) {
    // We're doing full-screen framebuffer blending, so no need to do stencil testing here.
    glDisable(GL_STENCIL_TEST);

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
}
