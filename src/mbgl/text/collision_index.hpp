#pragma once

#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/text/collision_feature.hpp>
#include <mbgl/util/grid_index.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/map/transform_state.hpp>

#include <array>

namespace mbgl {

class PlacedSymbol;

struct TileDistance;
    
using CollisionTileBoundaries = std::array<float,4>;

class CollisionIndex {
public:
    using CollisionGrid = GridIndex<IndexedSubfeature>;

    explicit CollisionIndex(const TransformState&);

    std::pair<bool,bool> placeFeature(CollisionFeature& feature,
                                      Point<float> shift,
                                      const mat4& posMatrix,
                                      const mat4& labelPlaneMatrix,
                                      const float textPixelRatio,
                                      PlacedSymbol& symbol,
                                      const float scale,
                                      const float fontSize,
                                      const bool allowOverlap,
                                      const bool pitchWithMap,
                                      const bool collisionDebug,
                                      const optional<CollisionTileBoundaries>& avoidEdges,
                                      const optional<std::function<bool(const IndexedSubfeature&)>> collisionGroupPredicate);

    void insertFeature(CollisionFeature& feature, bool ignorePlacement, uint32_t bucketInstanceId, uint16_t collisionGroupId);

    std::unordered_map<uint32_t, std::vector<IndexedSubfeature>> queryRenderedSymbols(const ScreenLineString&) const;
    
    CollisionTileBoundaries projectTileBoundaries(const mat4& posMatrix) const;

private:
    bool isOffscreen(const CollisionBox&) const;
    bool isInsideGrid(const CollisionBox&) const;
    bool isInsideTile(const CollisionBox&, const CollisionTileBoundaries& tileBoundaries) const;

    std::pair<bool,bool> placeLineFeature(CollisionFeature& feature,
                                  const mat4& posMatrix,
                                  const mat4& labelPlaneMatrix,
                                  const float textPixelRatio,
                                  PlacedSymbol& symbol,
                                  const float scale,
                                  const float fontSize,
                                  const bool allowOverlap,
                                  const bool pitchWithMap,
                                  const bool collisionDebug,
                                  const optional<CollisionTileBoundaries>& avoidEdges,
                                  const optional<std::function<bool(const IndexedSubfeature&)>> collisionGroupPredicate);
    
    float approximateTileDistance(const TileDistance& tileDistance, const float lastSegmentAngle, const float pixelsToTileUnits, const float cameraToAnchorDistance, const bool pitchWithMap);
    
    std::pair<float,float> projectAnchor(const mat4& posMatrix, const Point<float>& point) const;
    std::pair<Point<float>,float> projectAndGetPerspectiveRatio(const mat4& posMatrix, const Point<float>& point) const;
    Point<float> projectPoint(const mat4& posMatrix, const Point<float>& point) const;

    const TransformState transformState;

    CollisionGrid collisionGrid;
    CollisionGrid ignoredGrid;
    
    const float screenRightBoundary;
    const float screenBottomBoundary;
    const float gridRightBoundary;
    const float gridBottomBoundary;
    
    const float pitchFactor;
};

} // namespace mbgl
