#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/map/transform_state.hpp>

namespace mbgl {

using namespace style;

mat4 RenderTile::translatedMatrix(const std::array<float, 2>& translation,
                                  TranslateAnchorType anchor,
                                  const TransformState& state) const {
    if (translation[0] == 0 && translation[1] == 0) {
        return matrix;
    }

    mat4 vtxMatrix;

    if (anchor == TranslateAnchorType::Viewport) {
        const double sin_a = std::sin(-state.getAngle());
        const double cos_a = std::cos(-state.getAngle());
        matrix::translate(vtxMatrix, matrix,
                id.pixelsToTileUnits(translation[0] * cos_a - translation[1] * sin_a, state.getZoom()),
                id.pixelsToTileUnits(translation[0] * sin_a + translation[1] * cos_a, state.getZoom()),
                0);
    } else {
        matrix::translate(vtxMatrix, matrix,
                id.pixelsToTileUnits(translation[0], state.getZoom()),
                id.pixelsToTileUnits(translation[1], state.getZoom()),
                0);
    }

    return vtxMatrix;
}

} // namespace mbgl
