#include <llmr/renderer/painter.hpp>

#include <iostream>

using namespace llmr;

void Painter::drawComposite(GLuint texture, const CompositeProperties &properties) {
    glDisable(GL_STENCIL_TEST);

    useProgram(compositeShader->program);
    compositeShader->setMatrix(nativeMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    compositeShader->setImage(0);

    // TODO: layerstyle
    compositeShader->setOpacity(properties.opacity);

    // Draw the clipping mask
    compositeArray.bind(*compositeShader, tileStencilBuffer, BUFFER_OFFSET(0));
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)tileStencilBuffer.index());

    glEnable(GL_STENCIL_TEST);
}
