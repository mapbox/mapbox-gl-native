#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/debug_bucket.hpp>
#include <mbgl/map/tile.hpp>
#include <mbgl/map/tile_data.hpp>
#include <mbgl/shader/plain_shader.hpp>
#include <mbgl/util/string.hpp>

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))
#endif

using namespace mbgl;

void Painter::renderTileDebug(const Tile& tile) {
    gl::group group(std::string { "debug " } + std::string(tile.id));
    assert(tile.data);
    if (debug) {
        prepareTile(tile);
        renderDebugText(tile.data->debugBucket, tile.matrix);
        renderDebugFrame(tile.matrix);
    }
}

void Painter::renderDebugText(DebugBucket& bucket, const mat4 &matrix) {
    gl::group group("debug text");

    config.depthTest = false;

    useProgram(plainShader->program);
    plainShader->u_matrix = matrix;

    // Draw white outline
    plainShader->u_color = {{ 1.0f, 1.0f, 1.0f, 1.0f }};
    lineWidth(4.0f * state.getPixelRatio());
    bucket.drawLines(*plainShader);

#ifndef GL_ES_VERSION_2_0
    // Draw line "end caps"
    MBGL_CHECK_ERROR(glPointSize(2));
    bucket.drawPoints(*plainShader);
#endif

    // Draw black text.
    plainShader->u_color = {{ 0.0f, 0.0f, 0.0f, 1.0f }};
    lineWidth(2.0f * state.getPixelRatio());
    bucket.drawLines(*plainShader);

    config.depthTest = true;
}

void Painter::renderDebugFrame(const mat4 &matrix) {
    gl::group group("debug frame");

    // Disable depth test and don't count this towards the depth buffer,
    // but *don't* disable stencil test, as we want to clip the red tile border
    // to the tile viewport.
    config.depthTest = false;
    config.stencilTest = true;

    useProgram(plainShader->program);
    plainShader->u_matrix = matrix;

    // draw tile outline
    tileBorderArray.bind(*plainShader, tileBorderBuffer, BUFFER_OFFSET(0));
    plainShader->u_color = {{ 1.0f, 0.0f, 0.0f, 1.0f }};
    lineWidth(4.0f * state.getPixelRatio());
    MBGL_CHECK_ERROR(glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)tileBorderBuffer.index()));
}

void Painter::renderDebugText(const std::vector<std::string> &strings) {
    if (strings.empty()) {
        return;
    }

    gl::group group("debug text");

    config.depthTest = false;
    config.stencilTest = true;
    config.stencilFunc = { GL_ALWAYS, 0xFF, 0xFF };

    useProgram(plainShader->program);
    plainShader->u_matrix = nativeMatrix;

    DebugFontBuffer debugFontBuffer;
    int line = 25;
    for (const auto& str : strings) {
        debugFontBuffer.addText(str.c_str(), 10, line, 0.75);
        line += 20;
    }

    if (!debugFontBuffer.empty()) {
        // draw debug info
        VertexArrayObject debugFontArray;
        debugFontArray.bind(*plainShader, debugFontBuffer, BUFFER_OFFSET(0));
        plainShader->u_color = {{ 1.0f, 1.0f, 1.0f, 1.0f }};
        lineWidth(4.0f * state.getPixelRatio());
        MBGL_CHECK_ERROR(glDrawArrays(GL_LINES, 0, (GLsizei)debugFontBuffer.index()));
    #ifndef GL_ES_VERSION_2_0
        MBGL_CHECK_ERROR(glPointSize(2));
        MBGL_CHECK_ERROR(glDrawArrays(GL_POINTS, 0, (GLsizei)debugFontBuffer.index()));
    #endif
        plainShader->u_color = {{ 0.0f, 0.0f, 0.0f, 1.0f }};
        lineWidth(2.0f * state.getPixelRatio());
        MBGL_CHECK_ERROR(glDrawArrays(GL_LINES, 0, (GLsizei)debugFontBuffer.index()));
    }
}
