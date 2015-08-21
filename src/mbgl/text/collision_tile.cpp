#include <mbgl/text/collision_tile.hpp>
#include <cmath>

namespace mbgl {

void CollisionTile::reset(const float _angle, const float pitch) {
    tree.clear();
    angle = _angle;

     // Compute the transformation matrix.
    float angle_sin = std::sin(_angle);
    float angle_cos = std::cos(_angle);
    rotationMatrix = {{angle_cos, -angle_sin, angle_sin, angle_cos}};

    // Stretch boxes in y direction to account for the map tilt.
    const float _yStretch = 1.0f / std::cos(pitch / 180 * M_PI);

    // The amount the map is squished depends on the y position.
    // Sort of account for this by making all boxes a bit bigger.
    yStretch = std::pow(_yStretch, 1.3);
}

float CollisionTile::placeFeature(const CollisionFeature &feature) {

    float minPlacementScale = minScale;

    for (auto& box : feature.boxes) {
        const auto anchor = box.anchor.matMul(rotationMatrix);

        std::vector<CollisionTreeBox> blockingBoxes;
        tree.query(bgi::intersects(getTreeBox(anchor, box)), std::back_inserter(blockingBoxes));

        for (auto& blockingTreeBox : blockingBoxes) {
            const auto& blocking = std::get<1>(blockingTreeBox);
            auto blockingAnchor = blocking.anchor.matMul(rotationMatrix);

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

            if (minPlacementScale >= maxScale) return minPlacementScale;
        }
    }

    return minPlacementScale;
}

void CollisionTile::insertFeature(CollisionFeature &feature, const float minPlacementScale) {
    for (auto& box : feature.boxes) {
        box.placementScale = minPlacementScale;
    }

    if (minPlacementScale < maxScale) {
        std::vector<CollisionTreeBox> treeBoxes;
        for (auto& box : feature.boxes) {
            treeBoxes.emplace_back(getTreeBox(box.anchor.matMul(rotationMatrix), box), box);
        }
        tree.insert(treeBoxes.begin(), treeBoxes.end());
    }

}

Box CollisionTile::getTreeBox(const vec2<float> &anchor, const CollisionBox &box) {
    return Box{
        CollisionPoint{
            anchor.x + box.x1,
            anchor.y + box.y1 * yStretch
        },
        CollisionPoint{
            anchor.x + box.x2,
            anchor.y + box.y2 * yStretch
        }
    };
}

}
