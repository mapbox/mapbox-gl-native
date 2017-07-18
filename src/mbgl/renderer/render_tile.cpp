#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/buckets/debug_bucket.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/util/math.hpp>

namespace mbgl {

using namespace style;

mat4 RenderTile::translateVtxMatrix(const mat4& tileMatrix,
                                    const std::array<float, 2>& translation,
                                    TranslateAnchorType anchor,
                                    const TransformState& state,
                                    const bool inViewportPixelUnits) const {
    if (translation[0] == 0 && translation[1] == 0) {
        return tileMatrix;
    }

    mat4 vtxMatrix;

    const float angle = inViewportPixelUnits ?
        (anchor == TranslateAnchorType::Map ? state.getAngle() : 0) :
        (anchor == TranslateAnchorType::Viewport ? -state.getAngle() : 0);

    Point<float> translate = util::rotate(Point<float>{ translation[0], translation[1] }, angle);

    if (inViewportPixelUnits) {
        matrix::translate(vtxMatrix, tileMatrix, translate.x, translate.y, 0);
    } else {
        matrix::translate(vtxMatrix, tileMatrix,
                          id.pixelsToTileUnits(translate.x, state.getZoom()),
                          id.pixelsToTileUnits(translate.y, state.getZoom()),
                          0);
    }

    return vtxMatrix;
}

mat4 RenderTile::translatedMatrix(const std::array<float, 2>& translation,
                                  TranslateAnchorType anchor,
                                  const TransformState& state) const {
    return translateVtxMatrix(matrix, translation, anchor, state, false);
}

mat4 RenderTile::translatedClipMatrix(const std::array<float, 2>& translation,
                                      TranslateAnchorType anchor,
                                      const TransformState& state) const {
    return translateVtxMatrix(nearClippedMatrix, translation, anchor, state, false);
}

void RenderTile::startRender(Painter& painter) {
    tile.upload(painter.context);

    // Calculate two matrices for this tile: matrix is the standard tile matrix; nearClippedMatrix
    // clips the near plane to 100 to save depth buffer precision
    painter.state.matrixFor(matrix, id);
    painter.state.matrixFor(nearClippedMatrix, id);
    matrix::multiply(matrix, painter.projMatrix, matrix);
    matrix::multiply(nearClippedMatrix, painter.nearClippedProjMatrix, nearClippedMatrix);
}

void RenderTile::finishRender(Painter& painter) {
    if (!used || painter.frame.debugOptions == MapDebugOptions::NoDebug)
        return;

    static const style::Properties<>::PossiblyEvaluated properties {};
    static const DebugProgram::PaintPropertyBinders paintAttibuteData(properties, 0);

    auto draw = [&] (Color color, const auto& vertexBuffer, const auto& indexBuffer, const auto& segments, auto drawMode) {
        painter.programs->debug.draw(
            painter.context,
            drawMode,
            gl::DepthMode::disabled(),
            painter.stencilModeForClipping(clip),
            gl::ColorMode::unblended(),
            DebugProgram::UniformValues {
                uniforms::u_matrix::Value{ matrix },
                uniforms::u_color::Value{ color }
            },
            vertexBuffer,
            indexBuffer,
            segments,
            paintAttibuteData,
            properties,
            painter.state.getZoom(),
            "debug"
        );
    };

    if (painter.frame.debugOptions & (MapDebugOptions::Timestamps | MapDebugOptions::ParseStatus)) {
        if (!tile.debugBucket || tile.debugBucket->renderable != tile.isRenderable() ||
            tile.debugBucket->complete != tile.isComplete() ||
            !(tile.debugBucket->modified == tile.modified) ||
            !(tile.debugBucket->expires == tile.expires) ||
            tile.debugBucket->debugMode != painter.frame.debugOptions) {
            tile.debugBucket = std::make_unique<DebugBucket>(
                tile.id, tile.isRenderable(), tile.isComplete(), tile.modified,
                tile.expires, painter.frame.debugOptions, painter.context);
        }

        draw(Color::white(),
             *tile.debugBucket->vertexBuffer,
             *tile.debugBucket->indexBuffer,
             tile.debugBucket->segments,
             gl::Lines { 4.0f * painter.frame.pixelRatio });

        draw(Color::black(),
             *tile.debugBucket->vertexBuffer,
             *tile.debugBucket->indexBuffer,
             tile.debugBucket->segments,
             gl::Lines { 2.0f * painter.frame.pixelRatio });
    }

    if (painter.frame.debugOptions & MapDebugOptions::TileBorders) {
        draw(Color::red(),
             painter.tileVertexBuffer,
             painter.tileBorderIndexBuffer,
             painter.tileBorderSegments,
             gl::LineStrip { 4.0f * painter.frame.pixelRatio });
    }
}

} // namespace mbgl
