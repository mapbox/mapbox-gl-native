#ifndef MBGL_TEXT_ROTATION_RANGE
#define MBGL_TEXT_ROTATION_RANGE

#include <mbgl/util/math.hpp>
#include <mbgl/text/types.hpp>

#include <vector>
#include <cassert>

namespace mbgl {

/*
 * Combine an array of collision ranges to form a continuous
 * range that includes 0. Collisions within the ignoreRange are ignored
 */
CollisionRange mergeCollisions(const CollisionList &collisions,
                               PlacementRange ignoreRange);

/*
 *  Calculate collision ranges for two rotating boxes.e
 */
CollisionList rotatingRotatingCollisions(const CollisionRect &a,
                                         const CollisionRect &b,
                                         const CollisionAnchor &anchorToAnchor);

/*
 * Return the intersection points of a circle and a line segment;
 */
void circleEdgeCollisions(std::back_insert_iterator<CollisionAngles> angles,
                          const CollisionPoint &corner, float radius,
                          const CollisionPoint &p1, const CollisionPoint &p2);

/*
 *  Calculate the ranges for which the corner,
 *  rotatated around the anchor, is within the box;
 */
void cornerBoxCollisions(std::back_insert_iterator<CollisionList> collisions,
                         const CollisionPoint &corner,
                         const CollisionCorners &boxCorners, bool flip = false);

/*
 *  Calculate collision ranges for a rotating box and a fixed box;
 */
CollisionList rotatingFixedCollisions(const CollisionRect &rotating,
                                      const CollisionRect &fixed);

/*
 * Calculate the range a box conflicts with a second box
 */
CollisionRange rotationRange(const GlyphBox &inserting,
                             const PlacementBox &blocker, float scale);
}

#endif
