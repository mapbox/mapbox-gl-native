#pragma once

#include <mbgl/util/mat4.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/programs/symbol_program.hpp>

namespace mbgl {

    class TransformState;
    class RenderTile;
    class SymbolSizeBinder;
    class PlacedSymbol;
    namespace style {
        class SymbolPropertyValues;
    } // end namespace style
    
    struct TileDistance {
        TileDistance(float prevTileDistance_, float lastSegmentViewportDistance_)
            : prevTileDistance(prevTileDistance_), lastSegmentViewportDistance(lastSegmentViewportDistance_)
        {}
        float prevTileDistance;
        float lastSegmentViewportDistance;
    };
    
    struct PlacedGlyph {
        PlacedGlyph(Point<float> point_, float angle_, optional<TileDistance> tileDistance_)
            : point(point_), angle(angle_), tileDistance(tileDistance_)
        {}
        Point<float> point;
        float angle;
        optional<TileDistance> tileDistance;
    };

    mat4 getLabelPlaneMatrix(const mat4& posMatrix, const bool pitchWithMap, const bool rotateWithMap, const TransformState& state, const float pixelsToTileUnits);
    mat4 getGlCoordMatrix(const mat4& posMatrix, const bool pitchWithMap, const bool rotateWithMap, const TransformState& state, const float pixelsToTileUnits);
    
    typedef std::pair<Point<float>,float> PointAndCameraDistance;
    PointAndCameraDistance project(const Point<float>& point, const mat4& matrix);

    void reprojectLineLabels(gl::VertexVector<SymbolDynamicLayoutAttributes::Vertex>&, const std::vector<PlacedSymbol>&,
            const mat4& posMatrix, const style::SymbolPropertyValues&,
            const RenderTile&, const SymbolSizeBinder& sizeBinder, const TransformState&);
    
    optional<std::pair<PlacedGlyph, PlacedGlyph>> placeFirstAndLastGlyph(const float fontScale,
                                                            const float lineOffsetX,
                                                            const float lineOffsetY,
                                                            const bool flip,
                                                            const Point<float>& anchorPoint,
                                                            const Point<float>& tileAnchorPoint,
                                                            const PlacedSymbol& symbol,
                                                            const mat4& labelPlaneMatrix,
                                                            const bool returnTileDistance);

} // end namespace mbgl
