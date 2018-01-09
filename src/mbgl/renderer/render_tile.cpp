#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/buckets/debug_bucket.hpp>
#include <mbgl/renderer/render_static_data.hpp>
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

void RenderTile::setMask(TileMask&& mask) {
    tile.setMask(std::move(mask));
}

void RenderTile::startRender(PaintParameters& parameters) {
    tile.upload(parameters.context);

    // Calculate two matrices for this tile: matrix is the standard tile matrix; nearClippedMatrix
    // clips the near plane to 100 to save depth buffer precision
    parameters.state.matrixFor(matrix, id);
    parameters.state.matrixFor(nearClippedMatrix, id);
    matrix::multiply(matrix, parameters.projMatrix, matrix);
    matrix::multiply(nearClippedMatrix, parameters.nearClippedProjMatrix, nearClippedMatrix);
}

void RenderTile::finishRender(PaintParameters& parameters) {
    if (!used || parameters.debugOptions == MapDebugOptions::NoDebug)
        return;

    static const style::Properties<>::PossiblyEvaluated properties {};
    static const DebugProgram::PaintPropertyBinders paintAttributeData(properties, 0);

    if (parameters.debugOptions & (MapDebugOptions::Timestamps | MapDebugOptions::ParseStatus)) {
        if (!tile.debugBucket || tile.debugBucket->renderable != tile.isRenderable() ||
            tile.debugBucket->complete != tile.isComplete() ||
            !(tile.debugBucket->modified == tile.modified) ||
            !(tile.debugBucket->expires == tile.expires) ||
            tile.debugBucket->debugMode != parameters.debugOptions) {
            tile.debugBucket = std::make_unique<DebugBucket>(
                tile.id, tile.isRenderable(), tile.isComplete(), tile.modified,
                tile.expires, parameters.debugOptions, parameters.context);
        }

        parameters.programs.debug.draw(
            parameters.context,
            gl::Lines { 4.0f * parameters.pixelRatio },
            gl::DepthMode::disabled(),
            parameters.stencilModeForClipping(clip),
            gl::ColorMode::unblended(),
            DebugProgram::UniformValues {
                uniforms::u_matrix::Value{ matrix },
                uniforms::u_color::Value{ Color::white() }
            },
            *tile.debugBucket->vertexBuffer,
            *tile.debugBucket->indexBuffer,
            tile.debugBucket->segments,
            paintAttributeData,
            properties,
            parameters.state.getZoom(),
            "debug"
        );

        parameters.programs.debug.draw(
            parameters.context,
            gl::Lines { 2.0f * parameters.pixelRatio },
            gl::DepthMode::disabled(),
            parameters.stencilModeForClipping(clip),
            gl::ColorMode::unblended(),
            DebugProgram::UniformValues {
                uniforms::u_matrix::Value{ matrix },
                uniforms::u_color::Value{ Color::black() }
            },
            *tile.debugBucket->vertexBuffer,
            *tile.debugBucket->indexBuffer,
            tile.debugBucket->segments,
            paintAttributeData,
            properties,
            parameters.state.getZoom(),
            "debug"
        );
    }

    if (parameters.debugOptions & MapDebugOptions::TileBorders) {
        parameters.programs.debug.draw(
            parameters.context,
            gl::LineStrip { 4.0f * parameters.pixelRatio },
            gl::DepthMode::disabled(),
            parameters.stencilModeForClipping(clip),
            gl::ColorMode::unblended(),
            DebugProgram::UniformValues {
                uniforms::u_matrix::Value{ matrix },
                uniforms::u_color::Value{ Color::red() }
            },
            parameters.staticData.tileVertexBuffer,
            parameters.staticData.tileBorderIndexBuffer,
            parameters.staticData.tileBorderSegments,
            paintAttributeData,
            properties,
            parameters.state.getZoom(),
            "debug"
        );
    }
}

} // namespace mbgl
