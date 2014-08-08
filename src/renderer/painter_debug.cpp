#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/debug_bucket.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/util/string.hpp>

using namespace mbgl;

void Painter::renderTileDebug(const Tile& tile) {
    gl::group group(util::sprintf<32>("debug %d/%d/%d", tile.id.z, tile.id.y, tile.id.z));
    assert(tile.data);
    if (debug) {
        prepareTile(tile);
        renderDebugText(tile.data->debugBucket, tile.matrix);
        renderDebugFrame(tile.matrix);
    }
}

void Painter::renderDebugText(DebugBucket& bucket, const mat4 &matrix) {
    gl::group group("debug text");

    glDisable(GL_DEPTH_TEST);

    useProgram(plainShader->program);
    plainShader->setMatrix(matrix);

    // Draw white outline
    plainShader->setColor(1.0f, 1.0f, 1.0f, 1.0f);
    lineWidth(4.0f * map.getState().getPixelRatio());
    bucket.drawLines(*plainShader);

#ifndef GL_ES_VERSION_2_0
    // Draw line "end caps"
    glPointSize(2);
    bucket.drawPoints(*plainShader);
#endif

    // Draw black text.
    plainShader->setColor(0.0f, 0.0f, 0.0f, 1.0f);
    lineWidth(2.0f * map.getState().getPixelRatio());
    bucket.drawLines(*plainShader);

    glEnable(GL_DEPTH_TEST);
}

void Painter::renderDebugFrame(const mat4 &matrix) {
    gl::group group("debug frame");

    // Disable depth test and don't count this towards the depth buffer,
    // but *don't* disable stencil test, as we want to clip the red tile border
    // to the tile viewport.
    glDisable(GL_DEPTH_TEST);

    useProgram(plainShader->program);
    plainShader->setMatrix(matrix);

    // draw tile outline
    tileBorderArray.bind(*plainShader, tileBorderBuffer, BUFFER_OFFSET(0));
    plainShader->setColor(1.0f, 0.0f, 0.0f, 1.0f);
    lineWidth(4.0f * map.getState().getPixelRatio());
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)tileBorderBuffer.index());

    glEnable(GL_DEPTH_TEST);
}

void Painter::renderDebugText(const std::vector<std::string> &strings) {
    if (strings.empty()) {
        return;
    }

    gl::group group("debug text");

    glDisable(GL_DEPTH_TEST);
    glStencilFunc(GL_ALWAYS, 0xFF, 0xFF);

    useProgram(plainShader->program);
    plainShader->setMatrix(nativeMatrix);

    DebugFontBuffer debugFontBuffer;
    int line = 25;
    for (const std::string &str : strings) {
        debugFontBuffer.addText(str.c_str(), 10, line, 0.75);
        line += 20;
    }

    if (!debugFontBuffer.empty()) {
        // draw debug info
        VertexArrayObject debugFontArray;
        debugFontArray.bind(*plainShader, debugFontBuffer, BUFFER_OFFSET(0));
        plainShader->setColor(1.0f, 1.0f, 1.0f, 1.0f);
        lineWidth(4.0f * map.getState().getPixelRatio());
        glDrawArrays(GL_LINES, 0, (GLsizei)debugFontBuffer.index());
    #ifndef GL_ES_VERSION_2_0
        glPointSize(2);
        glDrawArrays(GL_POINTS, 0, (GLsizei)debugFontBuffer.index());
    #endif
        plainShader->setColor(0.0f, 0.0f, 0.0f, 1.0f);
        lineWidth(2.0f * map.getState().getPixelRatio());
        glDrawArrays(GL_LINES, 0, (GLsizei)debugFontBuffer.index());
    }

    glEnable(GL_DEPTH_TEST);
}
