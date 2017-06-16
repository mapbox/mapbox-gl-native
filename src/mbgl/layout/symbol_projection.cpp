#include <mbgl/layout/symbol_projection.hpp>

#include <mbgl/map/transform_state.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/renderer/layers/render_symbol_layer.hpp>

namespace mbgl {

    mat4 getLabelPlaneMatrix(const mat4& posMatrix, const bool pitchWithMap, const bool rotateWithMap, const TransformState& state, const float pixelsToTileUnits) {
        mat4 m;
        matrix::identity(m);
        if (pitchWithMap) {
            matrix::scale(m, m, 1 / pixelsToTileUnits, 1 / pixelsToTileUnits, 1);
            if (!rotateWithMap) {
                matrix::rotate_z(m, m, state.getAngle());
            }
        } else {
            matrix::scale(m, m, state.getSize().width / 2.0, -state.getSize().height / 2.0, 1.0);
            matrix::translate(m, m, 1, -1, 0);
            matrix::multiply(m, m, posMatrix);
        }
        return m;
    }

    mat4 getGlCoordMatrix(const mat4& posMatrix, const bool pitchWithMap, const bool rotateWithMap, const TransformState& state, const float pixelsToTileUnits) {
        mat4 m;
        matrix::identity(m);
        if (pitchWithMap) {
            matrix::multiply(m, m, posMatrix);
            matrix::scale(m, m, pixelsToTileUnits, pixelsToTileUnits, 1);
            if (!rotateWithMap) {
                matrix::rotate_z(m, m, -state.getAngle());
            }
        } else {
            matrix::scale(m, m, 1, -1, 1);
            matrix::translate(m, m, -1, -1, 0);
            matrix::scale(m, m, 2.0 / state.getSize().width, 2.0 / state.getSize().height, 1.0);
        }
        return m;
    }

    void reprojectLineLabels(SymbolBucket& bucket, const mat4& posMatrix, const bool isText, const style::SymbolPropertyValues& values, const RenderTile& tile) {
        // const sizeData = isText ? bucket.textSizeData : bucket.iconSizeData;
        // const partiallyEvaluatedSize = symbolSize.evaluateSizeForZoom(sizeData, painter.transform, layer, isText);

        const std::array<float, 2> clippingBuffer(256.0 / state.getSize().width * 2.0 + 1.0, 256.0 / state.getHeight());
        const clippingBuffer = [256 / painter.width * 2 + 1, 256 / painter.height * 2 + 1];
    }
} // end namespace mbgl
