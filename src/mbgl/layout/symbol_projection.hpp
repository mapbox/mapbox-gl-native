#pragma once

#include <mbgl/util/mat4.hpp>
#include <mbgl/gfx/vertex_buffer.hpp>
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
        PlacedGlyph() = default;

        PlacedGlyph(Point<float> point_, float angle_, optional<TileDistance> tileDistance_)
            : point(point_), angle(angle_), tileDistance(std::move(tileDistance_))
        {}
        PlacedGlyph(PlacedGlyph&& other) noexcept
            : point(other.point), angle(other.angle), tileDistance(std::move(other.tileDistance)) {}
        PlacedGlyph(const PlacedGlyph& other)
            : point(other.point), angle(other.angle), tileDistance(other.tileDistance) {}
        Point<float> point;
        float angle;
        optional<TileDistance> tileDistance;
    };

    float evaluateSizeForFeature(const ZoomEvaluatedSize& zoomEvaluatedSize, const PlacedSymbol& placedSymbol);
    mat4 getLabelPlaneMatrix(const mat4& posMatrix, const bool pitchWithMap, const bool rotateWithMap, const TransformState& state, const float pixelsToTileUnits);
    mat4 getGlCoordMatrix(const mat4& posMatrix, const bool pitchWithMap, const bool rotateWithMap, const TransformState& state, const float pixelsToTileUnits);
    
    using PointAndCameraDistance = std::pair<Point<float>,float>;
    PointAndCameraDistance project(const Point<float>& point, const mat4& matrix);

    void reprojectLineLabels(gfx::VertexVector<gfx::Vertex<SymbolDynamicLayoutAttributes>>&, const std::vector<PlacedSymbol>&,
            const mat4& posMatrix, bool pitchWithMap, bool rotateWithMap, bool keepUpright,
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

    void hideGlyphs(std::size_t numGlyphs, gfx::VertexVector<gfx::Vertex<SymbolDynamicLayoutAttributes>>& dynamicVertices);
    void addDynamicAttributes(const Point<float>& anchorPoint,
                              const float angle,
                              gfx::VertexVector<gfx::Vertex<SymbolDynamicLayoutAttributes>>& dynamicVertices);

} // end namespace mbgl
