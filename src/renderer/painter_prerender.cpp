#include <llmr/renderer/painter.hpp>

using namespace llmr;

void Painter::renderPrerenderedTexture(Bucket &bucket, const GenericProperties &properties) {
    const int buffer = 4096 * properties.prerenderBuffer;

    // draw the texture on a quad
    depthMask(false);

    useProgram(rasterShader->program);
    rasterShader->setMatrix(matrix);
    rasterShader->setOpacity(1);

    glDepthRange(strata, 1.0f);

    glActiveTexture(GL_TEXTURE0);
    rasterShader->setImage(0);
    rasterShader->setBuffer(buffer);
    rasterShader->setOpacity(properties.opacity);
    bucket.prerendered->bindTexture();
    coveringRasterArray.bind(*rasterShader, tileStencilBuffer, BUFFER_OFFSET(0));
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)tileStencilBuffer.index());

    depthMask(true);
}
