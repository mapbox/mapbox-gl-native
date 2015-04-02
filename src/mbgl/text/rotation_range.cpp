#include <mbgl/text/rotation_range.hpp>

#include <mbgl/util/interpolate.hpp>

#include <cassert>
#include <algorithm>

namespace mbgl {

/*
 * Combine an array of collision ranges to form a continuous
 * range that includes 0. Collisions within the ignoreRange are ignored
 */
CollisionRange mergeCollisions(const CollisionList &collisions,
                               PlacementRange ignoreRange) {
    // find continuous interval including 0 that doesn't have any collisions
    float min = 2.0f * M_PI;
    float max = 0.0f;

    for (const auto& collision : collisions) {
        bool entryOutside =
            ignoreRange[0] <= collision[0] && collision[0] <= ignoreRange[1];
        bool exitOutside =
            ignoreRange[0] <= collision[1] && collision[1] <= ignoreRange[1];

        if (entryOutside && exitOutside) {
            // no collision, since blocker is out of range
        } else if (entryOutside) {
            min = util::min(min, ignoreRange[1]);
            max = util::max(max, collision[1]);
        } else if (exitOutside) {
            min = util::min(min, collision[0]);
            max = util::max(max, ignoreRange[0]);
        } else {
            min = util::min(min, collision[0]);
            max = util::max(max, collision[1]);
        }
    }

    return {{min, max}};
}

/*
 *  Calculate collision ranges for two rotating boxes.
 */
CollisionList
rotatingRotatingCollisions(const CollisionRect &a, const CollisionRect &b,
                           const CollisionAnchor &anchorToAnchor) {
    const float d = util::mag<float>(anchorToAnchor);
    const float d_sq = d * d;

    const CollisionAnchor horizontal = {1, 0};
    const float angleBetweenAnchors =
        util::angle_between<float>(anchorToAnchor, horizontal);

    // Calculate angles at which collisions may occur
    const std::array<float, 8> c = {{
        // top/bottom
        /*[0]*/ static_cast<float>(std::asin((float)(a.br.y - b.tl.y) / d)),
        /*[1]*/ static_cast<float>(std::asin((float)(a.br.y - b.tl.y) / d) + M_PI),
        /*[2]*/ static_cast<float>(2 * M_PI -
                                   std::asin((float)(-a.tl.y + b.br.y) / d)),
        /*[3]*/ static_cast<float>(M_PI - std::asin((float)(-a.tl.y + b.br.y) / d)),

        // left/right
        /*[4]*/ static_cast<float>(2 * M_PI -
                                   std::acos((float)(a.br.x - b.tl.x) / d)),
        /*[5]*/ static_cast<float>(std::acos((float)(a.br.x - b.tl.x) / d)),
        /*[6]*/ static_cast<float>(M_PI - std::acos((float)(-a.tl.x + b.br.x) / d)),
        /*[7]*/ static_cast<float>(M_PI +
                                   std::acos((float)(-a.tl.x + b.br.x) / d))}};

    const float rl = a.br.x - b.tl.x;
    const float lr = -a.tl.x + b.br.x;
    const float tb = a.br.y - b.tl.y;
    const float bt = -a.tl.y + b.br.y;

    // Calculate the distance squared of the diagonal which will be used
    // to check if the boxes are close enough for collisions to occur at each
    // angle
    // todo, triple check these
    const std::array<float, 8> e = {{
        // top/bottom
        /*[0]*/ static_cast<float>(rl * rl + tb * tb),
        /*[1]*/ static_cast<float>(lr * lr + tb * tb),
        /*[2]*/ static_cast<float>(rl * rl + bt * bt),
        /*[3]*/ static_cast<float>(lr * lr + bt * bt),

        // left/right
        /*[4]*/ static_cast<float>(rl * rl + tb * tb),
        /*[5]*/ static_cast<float>(rl * rl + bt * bt),
        /*[6]*/ static_cast<float>(lr * lr + bt * bt),
        /*[7]*/ static_cast<float>(lr * lr + tb * tb)}};

    std::vector<float> f;
    for (size_t i = 0; i < c.size(); i++) {
        // Check if they are close enough to collide
        if (!std::isnan(c[i]) && d_sq <= e[i]) {
            // So far, angles have been calulated as relative to the vector
            // between anchors.
            // Convert the angles to angles from north.
            f.push_back(
                std::fmod((c[i] + angleBetweenAnchors + 2 * M_PI), (2 * M_PI)));
        }
    }

    assert(f.size() % 2 == 0);

    // Group the collision angles by two
    // each group represents a range where the two boxes collide
    CollisionList collisions;
    std::sort(f.begin(), f.end());
    for (size_t k = 0; k < f.size(); k += 2) {
        collisions.push_back({{f[k], f[k + 1]}});
    }

    return collisions;
}

double getAngle(const CollisionPoint &p1, const CollisionPoint &p2,
                CollisionAngle d, const CollisionPoint &corner) {
    return std::fmod(util::angle_between(util::interpolate(p1.x, p2.x, d),
                                    util::interpolate(p1.y, p2.y, d), corner.x,
                                    corner.y) +
                    2 * M_PI,
                2 * M_PI);
}

/*
 * Return the intersection points of a circle and a line segment;
 */
void circleEdgeCollisions(std::back_insert_iterator<CollisionAngles> angles,
                          const CollisionPoint &corner, float radius,
                          const CollisionPoint &p1, const CollisionPoint &p2) {
    const CollisionPoint::Type edgeX = p2.x - p1.x;
    const CollisionPoint::Type edgeY = p2.y - p1.y;

    const CollisionAngle a = edgeX * edgeX + edgeY * edgeY;
    const CollisionAngle b = (edgeX * p1.x + edgeY * p1.y) * 2;
    const CollisionAngle c = p1.x * p1.x + p1.y * p1.y - radius * radius;

    const CollisionAngle discriminant = b * b - 4 * a * c;

    // a collision exists only if line intersects circle at two points
    if (discriminant > 0) {
        CollisionAngle x1 = (-b - std::sqrt(discriminant)) / (2 * a);
        CollisionAngle x2 = (-b + std::sqrt(discriminant)) / (2 * a);

        // only add points if within line segment
        // hack to handle floating point representations of 0 and 1
        if (0 < x1 && x1 < 1) {
            angles = getAngle(p1, p2, x1, corner);
        }

        if (0 < x2 && x2 < 1) {
            angles = getAngle(p1, p2, x2, corner);
        }
    }
}

/*
 *  Calculate the ranges for which the corner,
 *  rotatated around the anchor, is within the box;
 */
void cornerBoxCollisions(std::back_insert_iterator<CollisionList> collisions,
                         const CollisionPoint &corner,
                         const CollisionCorners &boxCorners, bool flip) {
    float radius = util::mag<float>(corner);

    CollisionAngles angles;

    // Calculate the points at which the corners intersect with the edges
    for (size_t i = 0, j = 3; i < 4; j = i++) {
        circleEdgeCollisions(std::back_inserter(angles), corner, radius,
                             boxCorners[j], boxCorners[i]);
    }

    if (angles.size() % 2 != 0) {
        // TODO fix
        // This could get hit when a point intersects very close to a corner
        // and floating point issues cause only one of the entry or exit to be
        // counted
        throw std::runtime_error("expecting an even number of intersections");
    }

    std::sort(angles.begin(), angles.end());

    // Group by pairs, where each represents a range where a collision occurs
    for (size_t k = 0; k < angles.size(); k += 2) {
        CollisionRange range = {{angles[k], angles[k + 1]}};
        if (flip) {
            range = util::flip(range);
        }
        collisions = range;
    }
}

CollisionCorners getCorners(const CollisionRect &a) {
    return {{{a.tl.x, a.tl.y},
             {a.tl.x, a.br.y},
             {a.br.x, a.br.y},
             {a.br.x, a.tl.y}}};
}

/*
 *  Calculate collision ranges for a rotating box and a fixed box;
 */
CollisionList rotatingFixedCollisions(const CollisionRect &rotating,
                                      const CollisionRect &fixed) {
    const auto cornersR = getCorners(rotating);
    const auto cornersF = getCorners(fixed);

    // A collision occurs when, and only at least one corner from one of the
    // boxes is within the other box. Calculate these ranges for each corner.

    CollisionList collisions;

    for (size_t i = 0; i < 4; i++) {
        cornerBoxCollisions(std::back_inserter(collisions), cornersR[i],
                            cornersF);
        cornerBoxCollisions(std::back_inserter(collisions), cornersF[i],
                            cornersR, true);
    }

    return collisions;
}

/*
 * Calculate the range a box conflicts with a second box
 */
CollisionRange rotationRange(const GlyphBox &inserting,
                             const PlacementBox &blocker, float scale) {
    CollisionList collisions;

    const GlyphBox &a = inserting;
    const PlacementBox &b = blocker;

    // Instead of scaling the boxes, we move the anchors
    CollisionAnchor relativeAnchor{
        static_cast<float>((b.anchor.x - a.anchor.x) * scale),
        static_cast<float>((b.anchor.y - a.anchor.y) * scale)};

    // Generate a list of collision interval
    if (a.hBox && b.hBox) {
        collisions = rotatingRotatingCollisions(a.box, b.box, relativeAnchor);
    } else if (a.hBox) {
        const CollisionRect box {
            b.box.tl.x + relativeAnchor.x, b.box.tl.y + relativeAnchor.y,
            b.box.br.x + relativeAnchor.x, b.box.br.y + relativeAnchor.y};
        collisions = rotatingFixedCollisions(a.box, box);
    } else if (b.hBox) {
        const CollisionRect box {
            a.box.tl.x - relativeAnchor.x, a.box.tl.y - relativeAnchor.y,
            a.box.br.x - relativeAnchor.x, a.box.br.y - relativeAnchor.y};
        collisions = rotatingFixedCollisions(b.box, box);
    } else {
        // collisions remains empty
    }

    // Find and return the continous are around 0 where there are no collisions
    return mergeCollisions(collisions, blocker.placementRange);
}
}
