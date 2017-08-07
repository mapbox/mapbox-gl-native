#include <mbgl/text/collision_tile.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/math/log2.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/math/minmax.hpp>
#include <mbgl/util/intersection_tests.hpp>

#include <mapbox/geometry/envelope.hpp>
#include <mapbox/geometry/multi_point.hpp>

#include <cmath>

namespace mbgl {

CollisionTile::CollisionTile(PlacementConfig config_) : config(std::move(config_)) {
    // Compute the transformation matrix.
    const float angle_sin = std::sin(config.angle);
    const float angle_cos = std::cos(config.angle);
    rotationMatrix = { { angle_cos, -angle_sin, angle_sin, angle_cos } };
    reverseRotationMatrix = { { angle_cos, angle_sin, -angle_sin, angle_cos } };

    perspectiveRatio =
        1.0f +
        0.5f * (util::division(config.cameraToTileDistance, config.cameraToCenterDistance, 1.0f) -
                1.0f);

    minScale /= perspectiveRatio;
    maxScale /= perspectiveRatio;

    // We can only approximate here based on the y position of the tile
    // The shaders calculate a more accurate "incidence_stretch"
    // at render time to calculate an effective scale for collision
    // purposes, but we still want to use the yStretch approximation
    // here because we can't adjust the aspect ratio of the collision
    // boxes at render time.
    yStretch = util::max(
        1.0f, util::division(config.cameraToTileDistance,
                             config.cameraToCenterDistance * std::cos(config.pitch), 1.0f));
}

float CollisionTile::findPlacementScale(const Point<float>& anchor, const CollisionBox& box, const float boxMaxScale, const Point<float>& blockingAnchor, const CollisionBox& blocking) {
    float minPlacementScale = minScale;

    // Find the lowest scale at which the two boxes can fit side by side without overlapping.
    // Original algorithm:

    const float s1 = util::division(blocking.x1 - box.x2, anchor.x - blockingAnchor.x,
                                    1.0f); // scale at which new box is to the left of old box
    const float s2 = util::division(blocking.x2 - box.x1, anchor.x - blockingAnchor.x,
                                    1.0f); // scale at which new box is to the right of old box
    const float s3 = util::division((blocking.y1 - box.y2) * yStretch, anchor.y - blockingAnchor.y,
                                    1.0f); // scale at which new box is to the top of old box
    const float s4 = util::division((blocking.y2 - box.y1) * yStretch, anchor.y - blockingAnchor.y,
                                    1.0f); // scale at which new box is to the bottom of old box

    float collisionFreeScale = util::min(util::max(s1, s2), util::max(s3, s4));

    if (collisionFreeScale > blocking.maxScale) {
        // After a box's maxScale the label has shrunk enough that the box is no longer needed to cover it,
        // so unblock the new box at the scale that the old box disappears.
        collisionFreeScale = blocking.maxScale;
    }

    if (collisionFreeScale > boxMaxScale) {
        // If the box can only be shown after it is visible, then the box can never be shown.
        // But the label can be shown after this box is not visible.
        collisionFreeScale = boxMaxScale;
    }

    if (collisionFreeScale > minPlacementScale &&
            collisionFreeScale >= blocking.placementScale) {
        // If this collision occurs at a lower scale than previously found collisions
        // and the collision occurs while the other label is visible

        // this this is the lowest scale at which the label won't collide with anything
        minPlacementScale = collisionFreeScale;
    }

    return minPlacementScale;
}

float CollisionTile::placeFeature(const CollisionFeature& feature, bool allowOverlap, bool avoidEdges) {
    static const float infinity = std::numeric_limits<float>::infinity();
    static const std::array<CollisionBox, 4> edges {{
        // left
        CollisionBox(Point<float>(0, 0), { 0, 0 }, 0, -infinity, 0, infinity, infinity),
        // right
        CollisionBox(Point<float>(util::EXTENT, 0), { 0, 0 }, 0, -infinity, 0, infinity, infinity),
        // top
        CollisionBox(Point<float>(0, 0), { 0, 0 }, -infinity, 0, infinity, 0, infinity),
        // bottom
        CollisionBox(Point<float>(0, util::EXTENT), { 0, 0 }, -infinity, 0, infinity, 0, infinity)
    }};

    float minPlacementScale = minScale;

    for (auto& box : feature.boxes) {
        const auto anchor = util::matrixMultiply(rotationMatrix, box.anchor);

        const float boxMaxScale = box.adjustedMaxScale(rotationMatrix, yStretch);

        if (!allowOverlap) {
            for (auto it = tree.qbegin(bgi::intersects(getTreeBox(anchor, box))); it != tree.qend(); ++it) {
                const CollisionBox& blocking = std::get<1>(*it);
                Point<float> blockingAnchor = util::matrixMultiply(rotationMatrix, blocking.anchor);

                minPlacementScale = util::max(minPlacementScale, findPlacementScale(anchor, box, boxMaxScale, blockingAnchor, blocking));
                if (minPlacementScale >= maxScale) return minPlacementScale;
            }
        }

        if (avoidEdges) {
            const Point<float> rtl = util::matrixMultiply(reverseRotationMatrix, { box.x1, box.y1 });
            const Point<float> rtr = util::matrixMultiply(reverseRotationMatrix, { box.x2, box.y1 });
            const Point<float> rbl = util::matrixMultiply(reverseRotationMatrix, { box.x1, box.y2 });
            const Point<float> rbr = util::matrixMultiply(reverseRotationMatrix, { box.x2, box.y2 });
            CollisionBox rotatedBox(box.anchor,
                    box.offset,
                    util::min(rtl.x, rtr.x, rbl.x, rbr.x),
                    util::min(rtl.y, rtr.y, rbl.y, rbr.y),
                    util::max(rtl.x, rtr.x, rbl.x, rbr.x),
                    util::max(rtl.y, rtr.y, rbl.y, rbr.y),
                    boxMaxScale);

            for (auto& blocking : edges) {
                minPlacementScale = util::max(minPlacementScale, findPlacementScale(box.anchor, rotatedBox, boxMaxScale, blocking.anchor, blocking));
                if (minPlacementScale >= maxScale) return minPlacementScale;
            }
        }
    }

    return minPlacementScale;
}

void CollisionTile::insertFeature(CollisionFeature& feature, float minPlacementScale, bool ignorePlacement) {
    for (auto& box : feature.boxes) {
        box.placementScale = minPlacementScale;
    }

    if (minPlacementScale < maxScale) {
        std::vector<CollisionTreeBox> treeBoxes;
        for (auto& box : feature.boxes) {
            CollisionBox adjustedBox = box;
            box.maxScale = box.adjustedMaxScale(rotationMatrix, yStretch);
            treeBoxes.emplace_back(getTreeBox(util::matrixMultiply(rotationMatrix, box.anchor), box), std::move(adjustedBox), feature.indexedFeature);
        }
        if (ignorePlacement) {
            ignoredTree.insert(treeBoxes.begin(), treeBoxes.end());
        } else {
            tree.insert(treeBoxes.begin(), treeBoxes.end());
        }
    }

}

// +---------------------------+ As you zoom, the size of the symbol changes
// |(x1,y1)      |             | relative to the tile e.g. when zooming in,
// |             |             | the symbol gets smaller relative to the tile.
// |  (x1',y1')  v             |
// |     +-------+-------+     | The boxes inserted into the tree represents
// |     |       |       |     | the bounds at the integer zoom level (where
// |     |       |       |     | the symbol is biggest relative to the tile).
// |     |       |       |     |
// |---->+-------+-------+<----| This happens because placement is updated
// |     |       |(xa,ya)|     | once every new integer zoom level e.g.
// |     |       |       |     | std::floor(oldZoom) != std::floor(newZoom).
// |     |       |       |     |
// |     +-------+-------+     | Thus, they don't represent the exact bounds
// |             ^  (x2',y2')  | of the symbol at the current zoom level. For
// |             |             | calculating the bounds at current zoom level
// |             |      (x2,y2)| we must unscale the box using its center as
// +---------------------------+ transform origin.
Box CollisionTile::getTreeBox(const Point<float>& anchor, const CollisionBox& box, const float scale) {
    assert(box.x1 <= box.x2 && box.y1 <= box.y2);
    return Box{
        // When the 'perspectiveRatio' is high, we're effectively underzooming
        // the tile because it's in the distance.
        // In order to detect collisions that only happen while underzoomed,
        // we have to query a larger portion of the grid.
        // This extra work is offset by having a lower 'maxScale' bound
        // Note that this adjustment ONLY affects the bounding boxes
        // in the grid. It doesn't affect the boxes used for the
        // minPlacementScale calculations.
        CollisionPoint{
            anchor.x + box.x1 / scale * perspectiveRatio,
            anchor.y + box.y1 / scale * yStretch * perspectiveRatio,
        },
        CollisionPoint{
            anchor.x + box.x2 / scale * perspectiveRatio,
            anchor.y + box.y2 / scale * yStretch * perspectiveRatio
        }
    };
}

std::vector<IndexedSubfeature> CollisionTile::queryRenderedSymbols(const GeometryCoordinates& queryGeometry, float scale) const {
    std::vector<IndexedSubfeature> result;
    if (queryGeometry.empty() || (tree.empty() && ignoredTree.empty())) {
        return result;
    }

    // Generate a rotated geometry out of the original query geometry.
    // Scale has already been handled by the prior conversions.
    GeometryCoordinates polygon;
    for (const auto& point : queryGeometry) {
        auto rotated = util::matrixMultiply(rotationMatrix, convertPoint<float>(point));
        polygon.push_back(convertPoint<int16_t>(rotated));
    }

    // Predicate for ruling out already seen features.
    std::unordered_map<std::string, std::unordered_set<std::size_t>> sourceLayerFeatures;
    auto seenFeature = [&] (const CollisionTreeBox& treeBox) -> bool {
        const IndexedSubfeature& feature = std::get<2>(treeBox);
        const auto& seenFeatures = sourceLayerFeatures[feature.sourceLayerName];
        return seenFeatures.find(feature.index) == seenFeatures.end();
    };

    // "perspectiveRatio" is a tile-based approximation of how much larger symbols will
    // be in the distance. It won't line up exactly with the actually rendered symbols
    // Being exact would require running the collision detection logic in symbol_sdf.vertex
    // in the CPU
    const float perspectiveScale = scale / perspectiveRatio;

    // Account for the rounding done when updating symbol shader variables.
    const float roundedScale = std::pow(2.0f, std::ceil(util::log2(perspectiveScale) * 10.0f) / 10.0f);

    // Check if feature is rendered (collision free) at current scale.
    auto visibleAtScale = [&] (const CollisionTreeBox& treeBox) -> bool {
        const CollisionBox& box = std::get<1>(treeBox);
        return roundedScale >= box.placementScale && roundedScale <= box.adjustedMaxScale(rotationMatrix, yStretch);
    };

    // Check if query polygon intersects with the feature box at current scale.
    auto intersectsAtScale = [&] (const CollisionTreeBox& treeBox) -> bool {
        const CollisionBox& collisionBox = std::get<1>(treeBox);
        const auto anchor = util::matrixMultiply(rotationMatrix, collisionBox.anchor);

        const int16_t x1 = anchor.x + (collisionBox.x1 / perspectiveScale);
        const int16_t y1 = anchor.y + (collisionBox.y1 / perspectiveScale) * yStretch;
        const int16_t x2 = anchor.x + (collisionBox.x2 / perspectiveScale);
        const int16_t y2 = anchor.y + (collisionBox.y2 / perspectiveScale) * yStretch;
        auto bbox = GeometryCoordinates {
            { x1, y1 }, { x2, y1 }, { x2, y2 }, { x1, y2 }
        };
        return util::polygonIntersectsPolygon(polygon, bbox);
    };

    auto predicates = bgi::satisfies(seenFeature)
                   && bgi::satisfies(visibleAtScale)
                   && bgi::satisfies(intersectsAtScale);

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

} // namespace mbgl
