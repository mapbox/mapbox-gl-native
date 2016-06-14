#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/debug_bucket.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/tile_data.hpp>
#include <mbgl/shader/plain_shader.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/gl/debugging.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/gl_values.hpp>
#include <mbgl/gl/gl_helper.hpp>

using namespace mbgl;

void Painter::renderTileDebug(const Tile& tile) {
    MBGL_DEBUG_GROUP(std::string { "debug " } + util::toString(tile.id));
    if (frame.debugOptions != MapDebugOptions::NoDebug) {
        setClipping(tile.clip);
        if (frame.debugOptions & (MapDebugOptions::Timestamps | MapDebugOptions::ParseStatus)) {
            renderDebugText(tile.data, tile.matrix);
        }
        if (frame.debugOptions & MapDebugOptions::TileBorders) {
            renderDebugFrame(tile.matrix);
        }
    }
}

void Painter::renderDebugText(TileData& tileData, const mat4 &matrix) {
    MBGL_DEBUG_GROUP("debug text");

    config.depthTest = GL_FALSE;

    if (!tileData.debugBucket || tileData.debugBucket->renderable != tileData.isRenderable() ||
        tileData.debugBucket->complete != tileData.isComplete() ||
        !(tileData.debugBucket->modified == tileData.modified) ||
        !(tileData.debugBucket->expires == tileData.expires) ||
        tileData.debugBucket->debugMode != frame.debugOptions) {
        tileData.debugBucket = std::make_unique<DebugBucket>(
            tileData.id, tileData.isRenderable(), tileData.isComplete(), tileData.modified,
            tileData.expires, frame.debugOptions);
    }

    config.program = plainShader->getID();
    plainShader->u_matrix = matrix;

    // Draw white outline
    plainShader->u_color = {{ 1.0f, 1.0f, 1.0f, 1.0f }};
    config.lineWidth = 4.0f * frame.pixelRatio;
    tileData.debugBucket->drawLines(*plainShader, store);

#ifndef GL_ES_VERSION_2_0
    // Draw line "end caps"
    MBGL_CHECK_ERROR(glPointSize(2));
    tileData.debugBucket->drawPoints(*plainShader, store);
#endif

    // Draw black text.
    plainShader->u_color = {{ 0.0f, 0.0f, 0.0f, 1.0f }};
    config.lineWidth = 2.0f * frame.pixelRatio;
    tileData.debugBucket->drawLines(*plainShader, store);

    config.depthFunc.reset();
    config.depthTest = GL_TRUE;
}

void Painter::renderDebugFrame(const mat4 &matrix) {
    MBGL_DEBUG_GROUP("debug frame");

    // Disable depth test and don't count this towards the depth buffer,
    // but *don't* disable stencil test, as we want to clip the red tile border
    // to the tile viewport.
    config.depthTest = GL_FALSE;
    config.stencilOp.reset();
    config.stencilTest = GL_TRUE;

    config.program = plainShader->getID();
    plainShader->u_matrix = matrix;

    // draw tile outline
    tileBorderArray.bind(*plainShader, tileBorderBuffer, BUFFER_OFFSET_0, store);
    plainShader->u_color = {{ 1.0f, 0.0f, 0.0f, 1.0f }};
    config.lineWidth = 4.0f * frame.pixelRatio;
    MBGL_CHECK_ERROR(glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)tileBorderBuffer.index()));
}

void Painter::renderClipMasks() {
    config.stencilTest = GL_FALSE;
    config.depthTest = GL_FALSE;
    config.program = 0;
    config.colorMask = { GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE };

#ifndef GL_ES_VERSION_2_0
    config.pixelZoom = { 1, 1 };
    config.rasterPos = {{ -1, -1, 0, 0 }};

    // Read the stencil buffer
    const auto& fbSize = frame.framebufferSize;
    auto pixels = std::make_unique<uint8_t[]>(fbSize[0] * fbSize[1]);
    MBGL_CHECK_ERROR(glReadPixels(
                0,                // GLint x
                0,                // GLint y
                fbSize[0],        // GLsizei width
                fbSize[1],        // GLsizei height
                GL_STENCIL_INDEX, // GLenum format
                GL_UNSIGNED_BYTE, // GLenum type
                pixels.get()      // GLvoid * data
                ));

    // Scale the Stencil buffer to cover the entire color space.
    auto it = pixels.get();
    auto end = it + fbSize[0] * fbSize[1];
    const auto factor = 255.0f / *std::max_element(it, end);
    for (; it != end; ++it) {
        *it *= factor;
    }

    MBGL_CHECK_ERROR(glWindowPos2i(0, 0));
    MBGL_CHECK_ERROR(glDrawPixels(fbSize[0], fbSize[1], GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels.get()));
#endif // GL_ES_VERSION_2_0
}
