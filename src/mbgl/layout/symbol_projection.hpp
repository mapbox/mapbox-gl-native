#pragma once

#include <mbgl/util/mat4.hpp>

namespace mbgl {

    class TransformState;
    class SymbolBucket;
    class RenderTile;
    namespace style {
        class SymbolPropertyValues;
    }

    mat4 getLabelPlaneMatrix(const mat4& posMatrix, const bool pitchWithMap, const bool rotateWithMap, const TransformState& state, const float pixelsToTileUnits);
    mat4 getGlCoordMatrix(const mat4& posMatrix, const bool pitchWithMap, const bool rotateWithMap, const TransformState& state, const float pixelsToTileUnits);

    void reprojectLineLabels(SymbolBucket& bucket, const mat4& posMatrix, const bool isText, const style::SymbolPropertyValues& values, const RenderTile& tile);

} // end namespace mbgl
