#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/debug_bucket.hpp>
#include <mbgl/map/tile.hpp>
#include <mbgl/map/tile_data.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/shader/plain_shader.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/gl/debugging.hpp>

using namespace mbgl;

void Painter::renderTileDebug(const Tile& tile) {
    MBGL_DEBUG_GROUP(std::string { "debug " } + std::string(tile.id));
    assert(tile.data);
    if (data.getDebug()) {
        prepareTile(tile);
        renderDebugText(*tile.data, tile.matrix);
        renderDebugFrame(tile.matrix);
    }
}

void Painter::renderDebugText(TileData& tileData, const mat4 &matrix) {
    MBGL_DEBUG_GROUP("debug text");

    config.depthTest = false;

    if (!tileData.debugBucket || tileData.debugBucket->state != tileData.getState()) {
        tileData.debugBucket = std::make_unique<DebugBucket>(tileData.id, tileData.getState());
    }

    config.program = plainShader->program;
    plainShader->u_matrix = matrix;

    // Draw white outline
    plainShader->u_color = {{ 1.0f, 1.0f, 1.0f, 1.0f }};
    config.lineWidth = 4.0f * data.pixelRatio;
    tileData.debugBucket->drawLines(*plainShader);

#ifndef GL_ES_VERSION_2_0
    // Draw line "end caps"
    MBGL_CHECK_ERROR(glPointSize(2));
    tileData.debugBucket->drawPoints(*plainShader);
#endif

    // Draw black text.
    plainShader->u_color = {{ 0.0f, 0.0f, 0.0f, 1.0f }};
    config.lineWidth = 2.0f * data.pixelRatio;
    tileData.debugBucket->drawLines(*plainShader);

    config.depthTest = true;
}

void Painter::renderDebugFrame(const mat4 &matrix) {
    MBGL_DEBUG_GROUP("debug frame");

    // Disable depth test and don't count this towards the depth buffer,
    // but *don't* disable stencil test, as we want to clip the red tile border
    // to the tile viewport.
    config.depthTest = false;
    config.stencilTest = true;

    config.program = plainShader->program;
    plainShader->u_matrix = matrix;

    // draw tile outline
    tileBorderArray.bind(*plainShader, tileBorderBuffer, BUFFER_OFFSET_0);
    plainShader->u_color = {{ 1.0f, 0.0f, 0.0f, 1.0f }};
    config.lineWidth = 4.0f * data.pixelRatio;
    MBGL_CHECK_ERROR(glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)tileBorderBuffer.index()));
}
