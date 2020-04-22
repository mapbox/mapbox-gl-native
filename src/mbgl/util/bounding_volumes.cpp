#include <mbgl/util/bounding_volumes.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>

namespace mbgl {
namespace {

vec3 toVec3(const vec4& v) {
    return vec3{v[0], v[1], v[2]};
}

double vec4Dot(const vec4& a, const vec4& b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
}

template <size_t N>
static Point<double> ProjectPointsToAxis(const std::array<vec3, N>& points, const vec3& origin, const vec3& axis) {
    double min = std::numeric_limits<double>::max();
    double max = -std::numeric_limits<double>::max();

    for (const vec3& point : points) {
        double projectedPoint = vec3Dot(vec3Sub(point, origin), axis);
        min = std::min(projectedPoint, min);
        max = std::max(projectedPoint, max);
    }

    return {min, max};
}

} // namespace

namespace util {

AABB::AABB() : min({0, 0, 0}), max({0, 0, 0}) {}

AABB::AABB(const vec3& min_, const vec3& max_) : min(min_), max(max_) {}

vec3 AABB::closestPoint(const vec3& point) const {
    return {std::max(std::min(max[0], point[0]), min[0]),
            std::max(std::min(max[1], point[1]), min[1]),
            std::max(std::min(max[2], point[2]), min[2])};
}

vec3 AABB::distanceXYZ(const vec3& point) const {
    vec3 vec = vec3Sub(closestPoint(point), point);

    vec[0] = std::abs(vec[0]);
    vec[1] = std::abs(vec[1]);
    vec[2] = std::abs(vec[2]);

    return vec;
}

AABB AABB::quadrant(int idx) const {
    assert(idx >= 0 && idx < 4);
    vec3 quadrantMin = min;
    vec3 quadrantMax = max;
    const double xCenter = 0.5 * (max[0] + min[0]);
    const double yCenter = 0.5 * (max[1] + min[1]);

    // This aabb is split into 4 quadrants. For each axis define in which side of the split "idx" is
    // The result for indices 0, 1, 2, 3 is: { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 }
    const std::array<int, 4> xSplit = {0, 1, 0, 1};
    const std::array<int, 4> ySplit = {0, 0, 1, 1};

    quadrantMin[0] = xSplit[idx] ? xCenter : quadrantMin[0];
    quadrantMax[0] = xSplit[idx] ? quadrantMax[0] : xCenter;

    quadrantMin[1] = ySplit[idx] ? yCenter : quadrantMin[1];
    quadrantMax[1] = ySplit[idx] ? quadrantMax[1] : yCenter;

    return {quadrantMin, quadrantMax};
}

bool AABB::intersects(const AABB& aabb) const {
    if (min[0] > aabb.max[0] || aabb.min[0] > max[0]) return false;
    if (min[1] > aabb.max[1] || aabb.min[1] > max[1]) return false;
    if (min[2] > aabb.max[2] || aabb.min[2] > max[2]) return false;
    return true;
}

bool AABB::operator==(const AABB& aabb) const {
    return min == aabb.min && max == aabb.max;
}

bool AABB::operator!=(const AABB& aabb) const {
    return !(*this == aabb);
}

// Named index values for frustum::points array
enum {
    near_tl = 0,
    near_tr = 1,
    near_br = 2,
    near_bl = 3,
    far_tl = 4,
    far_tr = 5,
    far_br = 6,
    far_bl = 7,
};

Frustum::Frustum(const std::array<vec3, 8>& points_, const std::array<vec4, 6>& planes_)
    : points(points_), planes(planes_) {
    const Point<double> xBounds = ProjectPointsToAxis(points, {0, 0, 0}, {1, 0, 0});
    const Point<double> yBounds = ProjectPointsToAxis(points, {0, 0, 0}, {0, 1, 0});
    const Point<double> zBounds = ProjectPointsToAxis(points, {0, 0, 0}, {0, 0, 1});

    bounds = AABB({xBounds.x, yBounds.x, zBounds.x}, {xBounds.y, yBounds.y, zBounds.y});

    // Precompute a set of separating axis candidates for precise intersection tests.
    // Remaining axes not covered in basic intersection tests are: axis[] = (edges of aabb) x (edges of frustum)
    std::array<vec3, 6> frustumEdges = {vec3Sub(points[near_br], points[near_bl]),
                                        vec3Sub(points[near_tl], points[near_bl]),
                                        vec3Sub(points[far_tl], points[near_tl]),
                                        vec3Sub(points[far_tr], points[near_tr]),
                                        vec3Sub(points[far_br], points[near_br]),
                                        vec3Sub(points[far_bl], points[near_bl])};

    for (size_t i = 0; i < frustumEdges.size(); i++) {
        // Cross product [1, 0, 0] x [a, b, c] == [0, -c, b]
        // Cross product [0, 1, 0] x [a, b, c] == [c, 0, -a]
        const vec3 axis0 = {0.0, -frustumEdges[i][2], frustumEdges[i][1]};
        const vec3 axis1 = {frustumEdges[i][2], 0.0, -frustumEdges[i][0]};

        projections[i * 2] = {axis0, ProjectPointsToAxis(points, points[0], axis0)};
        projections[i * 2 + 1] = {axis1, ProjectPointsToAxis(points, points[0], axis1)};
    }
}

Frustum Frustum::fromInvProjMatrix(const mat4& invProj, double worldSize, double zoom, bool flippedY) {
    // Define frustum corner points in normalized clip space
    std::array<vec4, 8> cornerCoords = {vec4{-1.0, 1.0, -1.0, 1.0},
                                        vec4{1.0, 1.0, -1.0, 1.0},
                                        vec4{1.0, -1.0, -1.0, 1.0},
                                        vec4{-1.0, -1.0, -1.0, 1.0},
                                        vec4{-1.0, 1.0, 1.0, 1.0},
                                        vec4{1.0, 1.0, 1.0, 1.0},
                                        vec4{1.0, -1.0, 1.0, 1.0},
                                        vec4{-1.0, -1.0, 1.0, 1.0}};

    const double scale = std::pow(2.0, zoom);

    // Transform points to tile space
    for (auto& coord : cornerCoords) {
        matrix::transformMat4(coord, coord, invProj);
        for (auto& component : coord) component *= 1.0 / coord[3] / worldSize * scale;
    }

    std::array<vec3i, 6> frustumPlanePointIndices = {
        vec3i{near_bl, near_br, far_br},  // bottom
        vec3i{near_tl, near_bl, far_bl},  // left
        vec3i{near_br, near_tr, far_tr},  // right
        vec3i{near_tl, far_tl, far_tr},   // top
        vec3i{near_tl, near_tr, near_br}, // near
        vec3i{far_br, far_tr, far_tl}     // far
    };

    if (flippedY) {
        std::for_each(frustumPlanePointIndices.begin(), frustumPlanePointIndices.end(), [](vec3i& tri) {
            std::swap(tri[1], tri[2]);
        });
    }

    std::array<vec4, 6> frustumPlanes;

    for (std::size_t i = 0; i < frustumPlanePointIndices.size(); i++) {
        const vec3i indices = frustumPlanePointIndices[i];

        // Compute plane equation using 3 points on the plane
        const vec3 p0 = toVec3(cornerCoords[indices[0]]);
        const vec3 p1 = toVec3(cornerCoords[indices[1]]);
        const vec3 p2 = toVec3(cornerCoords[indices[2]]);

        const vec3 a = vec3Sub(p0, p1);
        const vec3 b = vec3Sub(p2, p1);
        const vec3 n = vec3Normalize(vec3Cross(a, b));

        frustumPlanes[i] = {n[0], n[1], n[2], -vec3Dot(n, p1)};
    }

    std::array<vec3, 8> frustumPoints;

    for (size_t i = 0; i < cornerCoords.size(); i++) frustumPoints[i] = toVec3(cornerCoords[i]);

    return {frustumPoints, frustumPlanes};
}

IntersectionResult Frustum::intersects(const AABB& aabb) const {
    // Execute separating axis test between two convex objects to find intersections
    // Each frustum plane together with 3 major axes define the separating axes
    // This implementation is conservative as it's not checking all possible axes.
    // False positive rate is ~0.5% of all cases (see intersectsPrecise).
    // Note: test only 4 points as both min and max points have zero elevation
    assert(aabb.min[2] == 0.0 && aabb.max[2] == 0.0);

    if (!bounds.intersects(aabb)) return IntersectionResult::Separate;

    const std::array<vec4, 4> aabbPoints = {
        vec4{aabb.min[0], aabb.min[1], 0.0, 1.0},
        vec4{aabb.max[0], aabb.min[1], 0.0, 1.0},
        vec4{aabb.max[0], aabb.max[1], 0.0, 1.0},
        vec4{aabb.min[0], aabb.max[1], 0.0, 1.0},
    };

    bool fullyInside = true;

    for (const vec4& plane : planes) {
        size_t pointsInside = 0;

        pointsInside += vec4Dot(plane, aabbPoints[0]) >= 0.0;
        pointsInside += vec4Dot(plane, aabbPoints[1]) >= 0.0;
        pointsInside += vec4Dot(plane, aabbPoints[2]) >= 0.0;
        pointsInside += vec4Dot(plane, aabbPoints[3]) >= 0.0;

        if (!pointsInside) {
            // Separating axis found, no intersection
            return IntersectionResult::Separate;
        }

        if (pointsInside != aabbPoints.size()) fullyInside = false;
    }

    return fullyInside ? IntersectionResult::Contains : IntersectionResult::Intersects;
}

IntersectionResult Frustum::intersectsPrecise(const AABB& aabb, bool edgeCasesOnly) const {
    if (!edgeCasesOnly) {
        IntersectionResult result = intersects(aabb);

        if (result == IntersectionResult::Separate) return result;
    }

    const std::array<vec3, 4> aabbPoints = {vec3{aabb.min[0], aabb.min[1], 0.0},
                                            vec3{aabb.max[0], aabb.min[1], 0.0},
                                            vec3{aabb.max[0], aabb.max[1], 0.0},
                                            vec3{aabb.min[0], aabb.max[1], 0.0}};

    // For a precise SAT-test all edge cases needs to be covered
    // Projections of the frustum on separating axis candidates have been precomputed already
    for (const Projection& proj : projections) {
        Point<double> projectedAabb = ProjectPointsToAxis(aabbPoints, points[0], proj.axis);
        const Point<double>& projectedFrustum = proj.projection;

        if (projectedFrustum.y < projectedAabb.x || projectedFrustum.x > projectedAabb.y) {
            return IntersectionResult::Separate;
        }
    }

    return IntersectionResult::Intersects;
}

} // namespace util
} // namespace mbgl