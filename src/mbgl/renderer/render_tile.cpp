#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/map/transform_state.hpp>

namespace mbgl {

using namespace style;

mat4 RenderTile::translateVtxMatrix(const mat4& tileMatrix,
                                    const std::array<float, 2>& translation,
                                    TranslateAnchorType anchor,
                                    const TransformState& state) const {
    if (translation[0] == 0 && translation[1] == 0) {
        return tileMatrix;
    }

    mat4 vtxMatrix;

    if (anchor == TranslateAnchorType::Viewport) {
        const double sin_a = std::sin(-state.getAngle());
        const double cos_a = std::cos(-state.getAngle());
        matrix::translate(vtxMatrix, tileMatrix,
                          id.pixelsToTileUnits(translation[0] * cos_a - translation[1] * sin_a, state.getZoom()),
                          id.pixelsToTileUnits(translation[0] * sin_a + translation[1] * cos_a, state.getZoom()),
                          0);
    } else {
        matrix::translate(vtxMatrix, tileMatrix,
                          id.pixelsToTileUnits(translation[0], state.getZoom()),
                          id.pixelsToTileUnits(translation[1], state.getZoom()),
                          0);
    }

    return vtxMatrix;
}

mat4 RenderTile::translatedMatrix(const std::array<float, 2>& translation,
                                  TranslateAnchorType anchor,
                                  const TransformState& state) const {
    return translateVtxMatrix(matrix, translation, anchor, state);
}

mat4 RenderTile::translatedClipMatrix(const std::array<float, 2>& translation,
                                      TranslateAnchorType anchor,
                                      const TransformState& state) const {
    return translateVtxMatrix(nearClippedMatrix, translation, anchor, state);
}

void RenderTile::calculateMatrices(const mat4& projMatrix,
                                   const mat4& projClipMatrix,
                                   const TransformState& transform) {
    // Calculate two matrices for this tile: matrix is the standard tile matrix; nearClippedMatrix
    // clips the near plane to 100 to save depth buffer precision
    transform.matrixFor(matrix, id);
    transform.matrixFor(nearClippedMatrix, id);
    matrix::multiply(matrix, projMatrix, matrix);
    matrix::multiply(nearClippedMatrix, projClipMatrix, nearClippedMatrix);
}

} // namespace mbgl
