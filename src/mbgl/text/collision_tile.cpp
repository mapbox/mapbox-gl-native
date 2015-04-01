#include <mbgl/text/collision_tile.hpp>
#include <cmath>

namespace mbgl {

void CollisionTile::reset(const float _angle, const float pitch) {
    tree.clear();
    angle = _angle;

     // Compute the transformation matrix.
    float angle_sin = std::sin(_angle);
    float angle_cos = std::cos(_angle);
    matrix = {{angle_cos, -angle_sin, angle_sin, angle_cos}};

    // Stretch boxes in y direction to account for the map tilt.
    const float _yStretch = 1.0f / std::cos(pitch / 180 * M_PI);

    // The amount the map is squished depends on the y position.
    // Sort of account for this by making all boxes a bit bigger.
    yStretch = std::pow(_yStretch, 1.3);
}

void CollisionTile::placeFeature(CollisionFeature &) {
//void CollisionTile::placeFeature(CollisionFeature &feature) {
}

void CollisionTile::insertFeature(CollisionFeature &, const float ) {
//void CollisionTile::insertFeature(CollisionFeature &feature, const float minPlacementScale) {
}

}
