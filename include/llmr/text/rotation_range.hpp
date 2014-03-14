#ifndef LLMR_TEXT_ROTATION_RANGE
#define LLMR_TEXT_ROTATION_RANGE

#include <llmr/util/vec.hpp>
#include <llmr/util/math.hpp>

#include <array>
#include <vector>
#include <cassert>

namespace llmr {

typedef vec2<int16_t> CollisionPoint;
typedef vec2<float> CollisionAnchor;

typedef std::array<float, 2> PlacementRange;
typedef float CollisionAngle;
typedef std::vector<CollisionAngle> CollisionAngles;
typedef std::array<CollisionAngle, 2> CollisionRange;
typedef std::vector<CollisionRange> CollisionList;
typedef std::array<CollisionPoint, 4> CollisionCorners;

struct CollisionRect {
    CollisionPoint tl;
    CollisionPoint br;
    inline CollisionRect() {}
    inline CollisionRect(CollisionPoint::Type ax, CollisionPoint::Type ay,
                         CollisionPoint::Type bx, CollisionPoint::Type by)
        : tl(ax, ay), br(bx, by) {}
    inline CollisionRect(const CollisionPoint &tl, const CollisionPoint &br)
        : tl(tl), br(br) {}
};

struct CollisionBox {
    CollisionRect box;
    CollisionPoint anchor;
    bool rotate = false;
    PlacementRange placementRange;
};

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
CollisionRange rotationRange(const CollisionBox &inserting,
                             const CollisionBox &blocker, double scale);
}

#endif
