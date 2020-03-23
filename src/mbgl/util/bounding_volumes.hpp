#pragma once

#include <mbgl/util/geometry.hpp>
#include <mbgl/util/mat3.hpp>
#include <mbgl/util/mat4.hpp>

namespace mbgl {
namespace util {

enum class IntersectionResult : int {
    Separate,
    Intersects,
    Contains,
};

class AABB {
public:
    AABB();
    AABB(const vec3& min_, const vec3& max_);

    vec3 closestPoint(const vec3& point) const;

    // Computes the shortest manhattan distance to the provided point
    vec3 distanceXYZ(const vec3& point) const;

    // Creates an aabb covering one quadrant of the aabb on XZ-plane.
    AABB quadrant(int idx) const;

    bool intersects(const AABB& aabb) const;

    bool operator==(const AABB& aabb) const;
    bool operator!=(const AABB& aabb) const;

    vec3 min;
    vec3 max;
};

class Frustum {
public:
    Frustum(const std::array<vec3, 8>& points_, const std::array<vec4, 6>& planes_);

    static Frustum fromInvProjMatrix(const mat4& invProj, double worldSize, double zoom, bool flippedY = false);

    // Performs conservative intersection test using separating axis theorem.
    // Some accuracy is traded for better performance. False positive rate is < 1%
    IntersectionResult intersects(const AABB& aabb) const;

    // Performs precise intersection test using separating axis theorem.
    // It is possible run only edge cases that were not covered in intersects()
    IntersectionResult intersectsPrecise(const AABB& aabb, bool edgeCasesOnly = false) const;

    const std::array<vec3, 8>& getPoints() const { return points; }
    const std::array<vec4, 6>& getPlanes() const { return planes; }

private:
    struct Projection {
        vec3 axis;
        Point<double> projection;
    };

    AABB bounds;
    std::array<vec3, 8> points;
    std::array<vec4, 6> planes;
    std::array<Projection, 12> projections;
};

} // namespace util
} // namespace mbgl