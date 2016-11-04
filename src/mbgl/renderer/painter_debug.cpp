#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/debug_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/shader/fill_uniforms.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/gl/debugging.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {

void Painter::renderTileDebug(const RenderTile& renderTile) {
    if (frame.debugOptions == MapDebugOptions::NoDebug)
        return;

    MBGL_DEBUG_GROUP(std::string { "debug " } + util::toString(renderTile.id));

    auto draw = [&] (Color color, auto subject) {
        context.draw({
            gl::DepthMode::disabled(),
            stencilModeForClipping(renderTile.clip),
            gl::ColorMode::unblended(),
            shaders->fill,
            FillColorUniforms::values(
                renderTile.matrix,
                1.0f,
                color,
                color,
                context.viewport.getCurrentValue().size
            ),
            subject
        });
    };

    if (frame.debugOptions & (MapDebugOptions::Timestamps | MapDebugOptions::ParseStatus)) {
        Tile& tile = renderTile.tile;
        if (!tile.debugBucket || tile.debugBucket->renderable != tile.isRenderable() ||
            tile.debugBucket->complete != tile.isComplete() ||
            !(tile.debugBucket->modified == tile.modified) ||
            !(tile.debugBucket->expires == tile.expires) ||
            tile.debugBucket->debugMode != frame.debugOptions) {
            tile.debugBucket = std::make_unique<DebugBucket>(
                tile.id, tile.isRenderable(), tile.isComplete(), tile.modified,
                tile.expires, frame.debugOptions, context);
        }

        const auto& vertexBuffer = tile.debugBucket->vertexBuffer;

        draw(Color::white(), gl::Unindexed<gl::Lines>(vertexBuffer, 4.0f * frame.pixelRatio));
        draw(Color::black(), gl::Unindexed<gl::Points>(vertexBuffer, 2.0f));
        draw(Color::black(), gl::Unindexed<gl::Lines>(vertexBuffer, 2.0f * frame.pixelRatio));
    }

    if (frame.debugOptions & MapDebugOptions::TileBorders) {
        draw(Color::red(), gl::Unindexed<gl::LineStrip>(tileLineStripVertexBuffer, 4.0f * frame.pixelRatio));
    }
}

#ifndef NDEBUG
void Painter::renderClipMasks(PaintParameters&) {
    context.setStencilMode(gl::StencilMode::disabled());
    context.setDepthMode(gl::DepthMode::disabled());
    context.setColorMode(gl::ColorMode::unblended());
    context.program = 0;

#if not MBGL_USE_GLES2

    // When reading data from the framebuffer, make sure that we are storing the depth values
    // tightly packed into the buffer to avoid buffer overruns. Also see unpacking adjustment below.
    context.pixelStorePack = { 1 };

    // Read the stencil buffer
    const auto viewport = context.viewport.getCurrentValue();
    auto pixels = std::make_unique<uint8_t[]>(viewport.size.width * viewport.size.height);
    MBGL_CHECK_ERROR(glReadPixels(
                viewport.x,           // GLint x
                viewport.y,           // GLint y
                viewport.size.width,  // GLsizei width
                viewport.size.height, // GLsizei height
                GL_STENCIL_INDEX,     // GLenum format
                GL_UNSIGNED_BYTE,     // GLenum type
                pixels.get()          // GLvoid * data
                ));

    // Scale the Stencil buffer to cover the entire color space.
    auto it = pixels.get();
    auto end = it + viewport.size.width * viewport.size.height;
    const auto factor = 255.0f / *std::max_element(it, end);
    for (; it != end; ++it) {
        *it *= factor;
    }

    context.pixelZoom = { 1, 1 };
    context.rasterPos = { -1, -1, 0, 1 };
    context.pixelStoreUnpack = { 1 };
    MBGL_CHECK_ERROR(glDrawPixels(viewport.size.width, viewport.size.height, GL_LUMINANCE,
                                  GL_UNSIGNED_BYTE, pixels.get()));
#endif // MBGL_USE_GLES2
}

void Painter::renderDepthBuffer(PaintParameters&) {
    context.setStencilMode(gl::StencilMode::disabled());
    context.setDepthMode(gl::DepthMode::disabled());
    context.setColorMode(gl::ColorMode::unblended());
    context.program = 0;

#if not MBGL_USE_GLES2

    // When reading data from the framebuffer, make sure that we are storing the depth values
    // tightly packed into the buffer to avoid buffer overruns. Also see unpacking adjustment below.
    context.pixelStorePack = { 1 };

    // Scales the values in the depth buffer so that they cover the entire grayscale range. This
    // makes it easier to spot tiny differences.
    const double base = 1.0 / (1.0 - depthRangeSize);
    MBGL_CHECK_ERROR(glPixelTransferf(GL_DEPTH_SCALE, base));
    MBGL_CHECK_ERROR(glPixelTransferf(GL_DEPTH_BIAS, 1.0 - base));

    // Read the stencil buffer
    auto viewport = context.viewport.getCurrentValue();
    auto pixels = std::make_unique<uint8_t[]>(viewport.size.width * viewport.size.height);

    MBGL_CHECK_ERROR(glReadPixels(
                viewport.x,           // GLint x
                viewport.y,           // GLint y
                viewport.size.width,  // GLsizei width
                viewport.size.height, // GLsizei height
                GL_DEPTH_COMPONENT,   // GLenum format
                GL_UNSIGNED_BYTE,     // GLenum type
                pixels.get()          // GLvoid * data
                ));

    context.pixelZoom = { 1, 1 };
    context.rasterPos = { -1, -1, 0, 1 };
    context.pixelStoreUnpack = { 1 };
    MBGL_CHECK_ERROR(glDrawPixels(viewport.size.width, viewport.size.height, GL_LUMINANCE,
                                  GL_UNSIGNED_BYTE, pixels.get()));
#endif // MBGL_USE_GLES2
}
#endif // NDEBUG

} // namespace mbgl
