#include <mbgl/renderer/painter.hpp>
#include <mbgl/style/style_properties.hpp>
#include <mbgl/renderer/prerendered_texture.hpp>
#include <mbgl/renderer/raster_bucket.hpp>

using namespace mbgl;

void Painter::preparePrerender(RasterBucket &bucket) {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

// Render the actual tile.
#if GL_EXT_discard_framebuffer
    const GLenum discards[] = {GL_COLOR_ATTACHMENT0};
    glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, discards);
#endif
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(0, 0, bucket.properties.size, bucket.properties.size);
}

void Painter::renderPrerenderedTexture(RasterBucket &bucket, const mat4 &matrix, const RasterProperties& properties) {
    const int buffer = bucket.properties.buffer * 4096.0f;

    // draw the texture on a quad
    useProgram(rasterShader->program);
    rasterShader->setMatrix(matrix);
    rasterShader->setOpacity(1);

    depthRange(strata, 1.0f);

    glActiveTexture(GL_TEXTURE0);
    rasterShader->setImage(0);
    rasterShader->setBuffer(buffer);
    rasterShader->setOpacity(properties.opacity);
    rasterShader->setBrightness(properties.brightness[0], properties.brightness[1]);
    rasterShader->setSaturation(properties.saturation);
    rasterShader->setContrast(properties.contrast);
    rasterShader->setSpin(spinWeights(properties.hue_rotate));
    bucket.texture.bindTexture();
    coveringRasterArray.bind(*rasterShader, tileStencilBuffer, BUFFER_OFFSET(0));
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)tileStencilBuffer.index());
}
