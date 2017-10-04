#include <mbgl/text/collision_index.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/math/log2.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/math/minmax.hpp>
#include <mbgl/util/intersection_tests.hpp>

#include <mbgl/layout/symbol_projection.hpp>

#include <mapbox/geometry/envelope.hpp>
#include <mapbox/geometry/multi_point.hpp>

#include <cmath>

namespace mbgl {

// When a symbol crosses the edge that causes it to be included in
// collision detection, it will cause changes in the symbols around
// it. This constant specifies how many pixels to pad the edge of
// the viewport for collision detection so that the bulk of the changes
// occur offscreen. Making this constant greater increases label
// stability, but it's expensive.
static const float viewportPadding = 100;

CollisionIndex::CollisionIndex(const TransformState& transformState_) : transformState(transformState_) {
    pitchFactor = std::cos(transformState.getPitch()) * transformState.getCameraToCenterDistance();
}

float CollisionIndex::approximateTileDistance(const TileDistance& tileDistance, const float lastSegmentAngle, const float pixelsToTileUnits, const float cameraToAnchorDistance, const bool pitchWithMap) {
    // This is a quick and dirty solution for chosing which collision circles to use (since collision circles are
    // laid out in tile units). Ideally, I think we should generate collision circles on the fly in viewport coordinates
    // at the time we do collision detection.

    // incidenceStretch is the ratio of how much y space a label takes up on a tile while drawn perpendicular to the viewport vs
    //  how much space it would take up if it were drawn flat on the tile
    // Using law of sines, camera_to_anchor/sin(ground_angle) = camera_to_center/sin(incidence_angle)
    // Incidence angle 90 -> head on, sin(incidence_angle) = 1, no stretch
    // Incidence angle 1 -> very oblique, sin(incidence_angle) =~ 0, lots of stretch
    // ground_angle = u_pitch + PI/2 -> sin(ground_angle) = cos(u_pitch)
    // incidenceStretch = 1 / sin(incidenceAngle)

    const float incidenceStretch = pitchWithMap ? 1 : cameraToAnchorDistance / pitchFactor;
    const float lastSegmentTile = tileDistance.lastSegmentViewportDistance * pixelsToTileUnits;
    return tileDistance.prevTileDistance +
        lastSegmentTile +
        (incidenceStretch - 1) * lastSegmentTile * std::abs(std::sin(lastSegmentAngle));
}

Box CollisionIndex::getTreeBox(const CollisionBox& box, const mat4& posMatrix, const float textPixelRatio) const {
    const auto projectedPoint = projectAndGetPerspectiveRatio(posMatrix, box.anchor);
    const float tileToViewport = textPixelRatio * projectedPoint.second;
    const float tlX = box.x1 / tileToViewport + projectedPoint.first.x;
    const float tlY = box.y1 / tileToViewport + projectedPoint.first.y;
    const float brX = box.x2 / tileToViewport + projectedPoint.first.x;
    const float brY = box.y2 / tileToViewport + projectedPoint.first.y;

    return Box{
        CollisionPoint{ tlX, tlY },
        CollisionPoint{ brX, brY }
    };
}

// TODO: add circle support
bool CollisionIndex::placeFeature(const CollisionFeature& feature, bool allowOverlap, const mat4& posMatrix, const float textPixelRatio) {
    for (auto& box : feature.boxes) {
        const auto projectedBox = getTreeBox(box, posMatrix, textPixelRatio);

        if (!allowOverlap) {
            for (auto it = tree.qbegin(bgi::intersects(projectedBox)); it != tree.qend(); ++it) {
                return false;
            }
        }
    }

    return true;
}

// TODO: don't duplicate projection that just happened in placeFeature
void CollisionIndex::insertFeature(CollisionFeature& feature, bool ignorePlacement, const mat4& posMatrix, const float textPixelRatio) {
    std::vector<CollisionTreeBox> treeBoxes;
    for (auto& box : feature.boxes) {
        treeBoxes.emplace_back(getTreeBox(box, posMatrix, textPixelRatio), box, feature.indexedFeature);
    }
    
    if (ignorePlacement) {
        ignoredTree.insert(treeBoxes.begin(), treeBoxes.end());
    } else {
        tree.insert(treeBoxes.begin(), treeBoxes.end());
    }
}

std::vector<IndexedSubfeature> CollisionIndex::queryRenderedSymbols(const GeometryCoordinates& queryGeometry, const UnwrappedTileID& tileID, const float textPixelRatio) const {
    std::vector<IndexedSubfeature> result;
    if (queryGeometry.empty() || (tree.empty() && ignoredTree.empty())) {
        return result;
    }

    mat4 posMatrix;
    transformState.matrixFor(posMatrix, tileID);

    GeometryCoordinates polygon;
    for (const auto& point : queryGeometry) {
        auto projected = projectPoint(posMatrix, convertPoint<float>(point));
        polygon.push_back(convertPoint<int16_t>(projected));
    }

    // Predicate for ruling out already seen features.
    std::unordered_map<std::string, std::unordered_set<std::size_t>> sourceLayerFeatures;
    auto seenFeature = [&] (const CollisionTreeBox& treeBox) -> bool {
        const IndexedSubfeature& feature = std::get<2>(treeBox);
        const auto& seenFeatures = sourceLayerFeatures[feature.sourceLayerName];
        return seenFeatures.find(feature.index) == seenFeatures.end();
    };


    // Check if query polygon intersects with the feature box at current scale.
    auto intersectsAtScale = [&] (const CollisionTreeBox& treeBox) -> bool {
        const CollisionBox& collisionBox = std::get<1>(treeBox);
        const auto projectedAnchor = projectAndGetPerspectiveRatio(posMatrix, collisionBox.anchor);
        const float tileToViewport = textPixelRatio * projectedAnchor.second;

        const int16_t x1 = projectedAnchor.first.x + (collisionBox.x1 / tileToViewport);
        const int16_t y1 = projectedAnchor.first.y + (collisionBox.y1 / tileToViewport);
        const int16_t x2 = projectedAnchor.first.x + (collisionBox.x2 / tileToViewport);
        const int16_t y2 = projectedAnchor.first.y + (collisionBox.y2 / tileToViewport);
        auto bbox = GeometryCoordinates {
            { x1, y1 }, { x2, y1 }, { x2, y2 }, { x1, y2 }
        };
        return util::polygonIntersectsPolygon(polygon, bbox);
    };

    auto predicates = bgi::satisfies(seenFeature) && bgi::satisfies(intersectsAtScale);

    auto queryTree = [&](const auto& tree_) {
        for (auto it = tree_.qbegin(predicates); it != tree_.qend(); ++it) {
            const IndexedSubfeature& feature = std::get<2>(*it);
            auto& seenFeatures = sourceLayerFeatures[feature.sourceLayerName];
            seenFeatures.insert(feature.index);
            result.push_back(feature);
        }
    };

    queryTree(tree);
    queryTree(ignoredTree);

    return result;
}

std::pair<Point<float>,float> CollisionIndex::projectAndGetPerspectiveRatio(const mat4& posMatrix, const Point<float>& point) const {
    vec4 p = {{ point.x, point.y, 0, 1 }};
    matrix::transformMat4(p, p, posMatrix);
    return std::make_pair(
        Point<float>(
            (((p[0]  / p[3] + 1) / 2) * transformState.getSize().width) + viewportPadding,
            (((-p[1] / p[3] + 1) / 2) * transformState.getSize().height) + viewportPadding
        ),
        0.5 + 0.5 * (p[3] / transformState.getCameraToCenterDistance())
    );
}

Point<float> CollisionIndex::projectPoint(const mat4& posMatrix, const Point<float>& point) const {
    vec4 p = {{ point.x, point.y, 0, 1 }};
    matrix::transformMat4(p, p, posMatrix);
    return Point<float>(
        (((p[0]  / p[3] + 1) / 2) * transformState.getSize().width) + viewportPadding,
        (((-p[1] / p[3] + 1) / 2) * transformState.getSize().height) + viewportPadding
    );
}

} // namespace mbgl
