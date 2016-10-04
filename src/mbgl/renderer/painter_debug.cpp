#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/debug_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/gl/debugging.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {

void Painter::renderTileDebug(const RenderTile& tile) {
    MBGL_DEBUG_GROUP(std::string { "debug " } + util::toString(tile.id));
    if (frame.debugOptions != MapDebugOptions::NoDebug) {
        setClipping(tile.clip);
        if (frame.debugOptions & (MapDebugOptions::Timestamps | MapDebugOptions::ParseStatus)) {
            renderDebugText(tile.tile, tile.matrix);
        }
        if (frame.debugOptions & MapDebugOptions::TileBorders) {
            renderDebugFrame(tile.matrix);
        }
    }
}

void Painter::renderDebugText(Tile& tile, const mat4 &matrix) {
    MBGL_DEBUG_GROUP("debug text");

    context.depthTest = false;

    if (!tile.debugBucket || tile.debugBucket->renderable != tile.isRenderable() ||
        tile.debugBucket->complete != tile.isComplete() ||
        !(tile.debugBucket->modified == tile.modified) ||
        !(tile.debugBucket->expires == tile.expires) ||
        tile.debugBucket->debugMode != frame.debugOptions) {
        tile.debugBucket = std::make_unique<DebugBucket>(
            tile.id, tile.isRenderable(), tile.isComplete(), tile.modified,
            tile.expires, frame.debugOptions, context);
    }

    auto& plainShader = shaders->fill;
    context.program = plainShader.getID();
    plainShader.u_matrix = matrix;
    plainShader.u_opacity = 1.0f;

    // Draw white outline
    plainShader.u_color = Color::white();
    context.lineWidth = 4.0f * frame.pixelRatio;
    tile.debugBucket->drawLines(plainShader, context);

#if not MBGL_USE_GLES2
    // Draw line "end caps"
    MBGL_CHECK_ERROR(glPointSize(2));
    tile.debugBucket->drawPoints(plainShader, context);
#endif // MBGL_USE_GLES2

    // Draw black text.
    plainShader.u_color = Color::black();
    context.lineWidth = 2.0f * frame.pixelRatio;
    tile.debugBucket->drawLines(plainShader, context);

    context.depthFunc = gl::DepthTestFunction::LessEqual;
    context.depthTest = true;
}

void Painter::renderDebugFrame(const mat4 &matrix) {
    MBGL_DEBUG_GROUP("debug frame");

    // Disable depth test and don't count this towards the depth buffer,
    // but *don't* disable stencil test, as we want to clip the red tile border
    // to the tile viewport.
    context.depthTest = false;
    context.stencilOp = { gl::StencilTestOperation::Keep, gl::StencilTestOperation::Keep,
                          gl::StencilTestOperation::Replace };
    context.stencilTest = true;

    auto& plainShader = shaders->fill;
    context.program = plainShader.getID();
    plainShader.u_matrix = matrix;
    plainShader.u_opacity = 1.0f;

    // draw tile outline
    tileBorderArray.bind(plainShader, tileLineStripVertexBuffer, BUFFER_OFFSET_0, context);
    plainShader.u_color = { 1.0f, 0.0f, 0.0f, 1.0f };
    context.lineWidth = 4.0f * frame.pixelRatio;
    MBGL_CHECK_ERROR(glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(tileLineStripVertexBuffer.vertexCount)));
}

#ifndef NDEBUG
void Painter::renderClipMasks() {
    context.stencilTest = false;
    context.depthTest = false;
    context.program = 0;
    context.colorMask = { true, true, true, true };

#if not MBGL_USE_GLES2
    context.pixelZoom = { 1, 1 };
    context.rasterPos = { -1, -1, 0, 0 };

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
#endif // MBGL_USE_GLES2
}
#endif // NDEBUG

#ifndef NDEBUG
void Painter::renderDepthBuffer() {
    context.stencilTest = false;
    context.depthTest = false;
    context.program = 0;
    context.colorMask = { true, true, true, true };

#if not MBGL_USE_GLES2
    context.pixelZoom = { 1, 1 };
    context.rasterPos = { -1, -1, 0, 0 };

    // Read the stencil buffer
    const auto& fbSize = frame.framebufferSize;
    auto pixels = std::make_unique<GLubyte[]>(fbSize[0] * fbSize[1]);

    const double base = 1.0 / (1.0 - depthRangeSize);
    glPixelTransferf(GL_DEPTH_SCALE, base);
    glPixelTransferf(GL_DEPTH_BIAS, 1.0 - base);

    MBGL_CHECK_ERROR(glReadPixels(
                0,                  // GLint x
                0,                  // GLint y
                fbSize[0],          // GLsizei width
                fbSize[1],          // GLsizei height
                GL_DEPTH_COMPONENT, // GLenum format
                GL_UNSIGNED_BYTE,   // GLenum type
                pixels.get()        // GLvoid * data
                ));

    MBGL_CHECK_ERROR(glWindowPos2i(0, 0));
    MBGL_CHECK_ERROR(glDrawPixels(fbSize[0], fbSize[1], GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels.get()));
#endif // MBGL_USE_GLES2
}
#endif // NDEBUG

} // namespace mbgl
