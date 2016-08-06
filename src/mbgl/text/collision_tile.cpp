#include <mbgl/text/collision_tile.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/math.hpp>

#include <cmath>

namespace mbgl {

auto infinity = std::numeric_limits<float>::infinity();

CollisionTile::CollisionTile(PlacementConfig config_) : config(std::move(config_)),
    edges({{
        // left
        CollisionBox(Point<float>(0, 0), 0, -infinity, 0, infinity, infinity),
        // right
        CollisionBox(Point<float>(util::EXTENT, 0), 0, -infinity, 0, infinity, infinity),
        // top
        CollisionBox(Point<float>(0, 0), -infinity, 0, infinity, 0, infinity),
        // bottom
        CollisionBox(Point<float>(0, util::EXTENT), -infinity, 0, infinity, 0, infinity),
    }}) {
    tree.clear();

    // Compute the transformation matrix.
    const float angle_sin = std::sin(config.angle);
    const float angle_cos = std::cos(config.angle);
    rotationMatrix = { { angle_cos, -angle_sin, angle_sin, angle_cos } };
    reverseRotationMatrix = { { angle_cos, angle_sin, -angle_sin, angle_cos } };

    // Stretch boxes in y direction to account for the map tilt.
    const float _yStretch = 1.0f / std::cos(config.pitch);

    // The amount the map is squished depends on the y position.
    // Sort of account for this by making all boxes a bit bigger.
    yStretch = std::pow(_yStretch, 1.3);
}


float CollisionTile::findPlacementScale(float minPlacementScale, const Point<float>& anchor,
        const CollisionBox& box, const Point<float>& blockingAnchor, const CollisionBox& blocking) {


    // Find the lowest scale at which the two boxes can fit side by side without overlapping.
    // Original algorithm:
    float s1 = (blocking.x1 - box.x2) / (anchor.x - blockingAnchor.x); // scale at which new box is to the left of old box
    float s2 = (blocking.x2 - box.x1) / (anchor.x - blockingAnchor.x); // scale at which new box is to the right of old box
    float s3 = (blocking.y1 - box.y2) * yStretch / (anchor.y - blockingAnchor.y); // scale at which new box is to the top of old box
    float s4 = (blocking.y2 - box.y1) * yStretch / (anchor.y - blockingAnchor.y); // scale at which new box is to the bottom of old box

    if (std::isnan(s1) || std::isnan(s2)) s1 = s2 = 1;
    if (std::isnan(s3) || std::isnan(s4)) s3 = s4 = 1;

    float collisionFreeScale = ::fmin(::fmax(s1, s2), ::fmax(s3, s4));

    if (collisionFreeScale > blocking.maxScale) {
        // After a box's maxScale the label has shrunk enough that the box is no longer needed to cover it,
        // so unblock the new box at the scale that the old box disappears.
        collisionFreeScale = blocking.maxScale;
    }

    if (collisionFreeScale > box.maxScale) {
        // If the box can only be shown after it is visible, then the box can never be shown.
        // But the label can be shown after this box is not visible.
        collisionFreeScale = box.maxScale;
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

float CollisionTile::placeFeature(const CollisionFeature& feature, const bool allowOverlap, const bool avoidEdges) {

    float minPlacementScale = minScale;

    for (auto& box : feature.boxes) {
        const auto anchor = util::matrixMultiply(rotationMatrix, box.anchor);

        if (!allowOverlap) {
            for (auto it = tree.qbegin(bgi::intersects(getTreeBox(anchor, box))); it != tree.qend(); ++it) {
                const CollisionBox& blocking = std::get<1>(*it);
                Point<float> blockingAnchor = util::matrixMultiply(rotationMatrix, blocking.anchor);

                minPlacementScale = findPlacementScale(minPlacementScale, anchor, box, blockingAnchor, blocking);
                if (minPlacementScale >= maxScale) return minPlacementScale;
            }
        }

        if (avoidEdges) {
            const Point<float> tl = { box.x1, box.y1 };
            const Point<float> tr = { box.x2, box.y1 };
            const Point<float> bl = { box.x1, box.y2 };
            const Point<float> br = { box.x2, box.y2 };
            const Point<float> rtl = util::matrixMultiply(reverseRotationMatrix, tl);
            const Point<float> rtr = util::matrixMultiply(reverseRotationMatrix, tr);
            const Point<float> rbl = util::matrixMultiply(reverseRotationMatrix, bl);
            const Point<float> rbr = util::matrixMultiply(reverseRotationMatrix, br);
            CollisionBox rotatedBox(box.anchor,
                    ::fmin(::fmin(rtl.x, rtr.x), ::fmin(rbl.x, rbr.x)),
                    ::fmin(::fmin(rtl.y, rtr.y), ::fmin(rbl.y, rbr.y)),
                    ::fmax(::fmax(rtl.x, rtr.x), ::fmax(rbl.x, rbr.x)),
                    ::fmax(::fmax(rtl.y, rtr.y), ::fmax(rbl.y, rbr.y)),
                    box.maxScale);

            for (auto& blocking : edges) {
                minPlacementScale = findPlacementScale(minPlacementScale, box.anchor, rotatedBox, blocking.anchor, blocking);

                if (minPlacementScale >= maxScale) return minPlacementScale;
            }
        }
    }

    return minPlacementScale;
}

void CollisionTile::insertFeature(CollisionFeature& feature, const float minPlacementScale, const bool ignorePlacement) {
    for (auto& box : feature.boxes) {
        box.placementScale = minPlacementScale;
    }

    if (minPlacementScale < maxScale) {
        std::vector<CollisionTreeBox> treeBoxes;
        for (auto& box : feature.boxes) {
            treeBoxes.emplace_back(getTreeBox(util::matrixMultiply(rotationMatrix, box.anchor), box), box, feature.indexedFeature);
        }
        if (ignorePlacement) {
            ignoredTree.insert(treeBoxes.begin(), treeBoxes.end());
        } else {
            tree.insert(treeBoxes.begin(), treeBoxes.end());
        }
    }

}

Box CollisionTile::getTreeBox(const Point<float>& anchor, const CollisionBox& box, const float scale) {
    return Box{
        CollisionPoint{
            anchor.x + box.x1 / scale,
            anchor.y + box.y1 / scale * yStretch
        },
        CollisionPoint{
            anchor.x + box.x2 / scale,
            anchor.y + box.y2 / scale * yStretch
        }
    };
}

std::vector<IndexedSubfeature> CollisionTile::queryRenderedSymbols(const mapbox::geometry::box<int16_t>& box, const float scale) {

    std::vector<IndexedSubfeature> result;
    std::unordered_map<std::string, std::set<std::size_t>> sourceLayerFeatures;

    auto anchor = util::matrixMultiply(rotationMatrix, convertPoint<float>(box.min));
    CollisionBox queryBox(anchor, 0, 0, box.max.x - box.min.x, box.max.y - box.min.y, scale);
    auto predicates = bgi::intersects(getTreeBox(anchor, queryBox));

    auto fn = [&] (const Tree& tree_) {
        for (auto it = tree_.qbegin(predicates); it != tree_.qend(); ++it) {
            const CollisionBox& blocking = std::get<1>(*it);
            const IndexedSubfeature& indexedFeature = std::get<2>(*it);

            auto& seenFeatures = sourceLayerFeatures[indexedFeature.sourceLayerName];
            if (seenFeatures.find(indexedFeature.index) == seenFeatures.end()) {
                auto blockingAnchor = util::matrixMultiply(rotationMatrix, blocking.anchor);
                float minPlacementScale = findPlacementScale(minScale, anchor, queryBox, blockingAnchor, blocking);
                if (minPlacementScale >= scale) {
                    seenFeatures.insert(indexedFeature.index);
                    result.push_back(indexedFeature);
                }
            }
        }
    };

    fn(tree);
    fn(ignoredTree);

    return result;
}

} // namespace mbgl
