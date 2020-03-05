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

using CollisionBoundaries = std::array<float, 4>; // [x1, y1, x2, y2]

class CollisionIndex {
public:
    using CollisionGrid = GridIndex<IndexedSubfeature>;

    explicit CollisionIndex(const TransformState&, MapMode);
    bool intersectsTileEdges(const CollisionBox&,
                             Point<float> shift,
                             const mat4& posMatrix,
                             const float textPixelRatio,
                             const CollisionBoundaries& tileEdges) const;
    std::pair<bool, bool> placeFeature(
        const CollisionFeature& feature,
        Point<float> shift,
        const mat4& posMatrix,
        const mat4& labelPlaneMatrix,
        const float textPixelRatio,
        const PlacedSymbol& symbol,
        const float scale,
        const float fontSize,
        const bool allowOverlap,
        const bool pitchWithMap,
        const bool collisionDebug,
        const optional<CollisionBoundaries>& avoidEdges,
        const optional<std::function<bool(const IndexedSubfeature&)>> collisionGroupPredicate,
        std::vector<ProjectedCollisionBox>& /*out*/);

    void insertFeature(const CollisionFeature& feature, const std::vector<ProjectedCollisionBox>&, bool ignorePlacement, uint32_t bucketInstanceId, uint16_t collisionGroupId);

    std::unordered_map<uint32_t, std::vector<IndexedSubfeature>> queryRenderedSymbols(const ScreenLineString&) const;

    CollisionBoundaries projectTileBoundaries(const mat4& posMatrix) const;

    const TransformState& getTransformState() const { return transformState; }

private:
    bool isOffscreen(const CollisionBoundaries&) const;
    bool isInsideGrid(const CollisionBoundaries&) const;
    bool isInsideTile(const CollisionBoundaries& box, const CollisionBoundaries& tileBoundaries) const;
    bool overlapsTile(const CollisionBoundaries& box, const CollisionBoundaries& tileBoundaries) const;

    std::pair<bool, bool> placeLineFeature(
        const CollisionFeature& feature,
        const mat4& posMatrix,
        const mat4& labelPlaneMatrix,
        const float textPixelRatio,
        const PlacedSymbol& symbol,
        const float scale,
        const float fontSize,
        const bool allowOverlap,
        const bool pitchWithMap,
        const bool collisionDebug,
        const optional<CollisionBoundaries>& avoidEdges,
        const optional<std::function<bool(const IndexedSubfeature&)>> collisionGroupPredicate,
        std::vector<ProjectedCollisionBox>& /*out*/);

    float approximateTileDistance(const TileDistance& tileDistance, const float lastSegmentAngle, const float pixelsToTileUnits, const float cameraToAnchorDistance, const bool pitchWithMap);
    
    std::pair<float,float> projectAnchor(const mat4& posMatrix, const Point<float>& point) const;
    std::pair<Point<float>,float> projectAndGetPerspectiveRatio(const mat4& posMatrix, const Point<float>& point) const;
    Point<float> projectPoint(const mat4& posMatrix, const Point<float>& point) const;
    CollisionBoundaries getProjectedCollisionBoundaries(const mat4& posMatrix,
                                                        Point<float> shift,
                                                        float textPixelRatio,
                                                        const CollisionBox& box) const;

    const TransformState transformState;

    const float viewportPadding;
    CollisionGrid collisionGrid;
    CollisionGrid ignoredGrid;
    
    const float screenRightBoundary;
    const float screenBottomBoundary;
    const float gridRightBoundary;
    const float gridBottomBoundary;
    
    const float pitchFactor;
};

} // namespace mbgl
