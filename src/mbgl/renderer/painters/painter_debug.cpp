#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/buckets/debug_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/fill_program.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/gl/debugging.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {

using namespace style;

void Painter::renderTileDebug(const RenderTile& renderTile) {
    if (frame.debugOptions == MapDebugOptions::NoDebug)
        return;

    MBGL_DEBUG_GROUP(context, std::string { "debug " } + util::toString(renderTile.id));

    static const style::Properties<>::PossiblyEvaluated properties {};
    static const DebugProgram::PaintPropertyBinders paintAttibuteData(properties, 0);

    auto draw = [&] (Color color, const auto& vertexBuffer, const auto& indexBuffer, const auto& segments, auto drawMode) {
        programs->debug.draw(
            context,
            drawMode,
            gl::DepthMode::disabled(),
            stencilModeForClipping(renderTile.clip),
            gl::ColorMode::unblended(),
            DebugProgram::UniformValues {
                uniforms::u_matrix::Value{ renderTile.matrix },
                uniforms::u_color::Value{ color }
            },
            vertexBuffer,
            indexBuffer,
            segments,
            paintAttibuteData,
            properties,
            state.getZoom(),
            "debug"
        );
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

        draw(Color::white(),
             *tile.debugBucket->vertexBuffer,
             *tile.debugBucket->indexBuffer,
             tile.debugBucket->segments,
             gl::Lines { 4.0f * frame.pixelRatio });

        draw(Color::black(),
             *tile.debugBucket->vertexBuffer,
             *tile.debugBucket->indexBuffer,
             tile.debugBucket->segments,
             gl::Lines { 2.0f * frame.pixelRatio });
    }

    if (frame.debugOptions & MapDebugOptions::TileBorders) {
        draw(Color::red(),
             tileVertexBuffer,
             tileBorderIndexBuffer,
             tileBorderSegments,
             gl::LineStrip { 4.0f * frame.pixelRatio });
    }
}

void Painter::renderTileDebug(const mat4& matrix) {
    if (frame.debugOptions == MapDebugOptions::NoDebug)
        return;

    static const style::Properties<>::PossiblyEvaluated properties {};
    static const DebugProgram::PaintPropertyBinders paintAttibuteData(properties, 0);

    if (frame.debugOptions & MapDebugOptions::TileBorders) {
        programs->debug.draw(
            context,
            gl::LineStrip { 4.0f * frame.pixelRatio },
            gl::DepthMode::disabled(),
            gl::StencilMode::disabled(),
            gl::ColorMode::unblended(),
            DebugProgram::UniformValues {
             uniforms::u_matrix::Value{ matrix },
             uniforms::u_color::Value{ Color::red() }
            },
            tileVertexBuffer,
            tileBorderIndexBuffer,
            tileBorderSegments,
            paintAttibuteData,
            properties,
            state.getZoom(),
            "debug"
        );
    }
}

#ifndef NDEBUG
void Painter::renderClipMasks(PaintParameters&) {
    context.setStencilMode(gl::StencilMode::disabled());
    context.setDepthMode(gl::DepthMode::disabled());
    context.setColorMode(gl::ColorMode::unblended());
    context.program = 0;

#if not MBGL_USE_GLES2
    // Reset the value in case someone else changed it, or it's dirty.
    context.pixelTransferStencil = gl::value::PixelTransferStencil::Default;

    // Read the stencil buffer
    const auto viewport = context.viewport.getCurrentValue();
    auto image =
        context.readFramebuffer<AlphaImage, gl::TextureFormat::Stencil>(viewport.size, false);

    // Scale the Stencil buffer to cover the entire color space.
    auto it = image.data.get();
    auto end = it + viewport.size.width * viewport.size.height;
    const auto factor = 255.0f / *std::max_element(it, end);
    for (; it != end; ++it) {
        *it *= factor;
    }

    context.pixelZoom = { 1, 1 };
    context.rasterPos = { -1, -1, 0, 1 };
    context.drawPixels(image);
#endif // MBGL_USE_GLES2
}

void Painter::renderDepthBuffer(PaintParameters&) {
    context.setStencilMode(gl::StencilMode::disabled());
    context.setDepthMode(gl::DepthMode::disabled());
    context.setColorMode(gl::ColorMode::unblended());
    context.program = 0;

#if not MBGL_USE_GLES2
    // Scales the values in the depth buffer so that they cover the entire grayscale range. This
    // makes it easier to spot tiny differences.
    const float base = 1.0f / (1.0f - depthRangeSize);
    context.pixelTransferDepth = { base, 1.0f - base };

    // Read the stencil buffer
    auto viewport = context.viewport.getCurrentValue();
    auto image =
        context.readFramebuffer<AlphaImage, gl::TextureFormat::Depth>(viewport.size, false);

    context.pixelZoom = { 1, 1 };
    context.rasterPos = { -1, -1, 0, 1 };
    context.drawPixels(image);
#endif // MBGL_USE_GLES2
}
#endif // NDEBUG

} // namespace mbgl
