#pragma once

#include <mbgl/util/mat4.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/programs/symbol_program.hpp>

namespace mbgl {

    class TransformState;
    class RenderTile;
    class FrameHistory;
    class SymbolSizeBinder;
    class PlacedSymbol;
    namespace style {
        class SymbolPropertyValues;
    } // end namespace style

    mat4 getLabelPlaneMatrix(const mat4& posMatrix, const bool pitchWithMap, const bool rotateWithMap, const TransformState& state, const float pixelsToTileUnits);
    mat4 getGlCoordMatrix(const mat4& posMatrix, const bool pitchWithMap, const bool rotateWithMap, const TransformState& state, const float pixelsToTileUnits);

    void reprojectLineLabels(gl::VertexVector<SymbolDynamicLayoutAttributes::Vertex>&, const std::vector<PlacedSymbol>&,
            const mat4& posMatrix, const style::SymbolPropertyValues&,
            const RenderTile&, const SymbolSizeBinder& sizeBinder, const TransformState&, const FrameHistory& frameHistory);

} // end namespace mbgl
