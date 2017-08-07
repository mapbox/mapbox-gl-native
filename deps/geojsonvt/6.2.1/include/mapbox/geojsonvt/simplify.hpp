#pragma once

#include <mapbox/geojsonvt/types.hpp>

namespace mapbox {
namespace geojsonvt {
namespace detail {

// square distance from a point to a segment
inline double getSqSegDist(const vt_point& p, const vt_point& a, const vt_point& b) {
    double x = a.x;
    double y = a.y;
    double dx = b.x - a.x;
    double dy = b.y - a.y;

    if ((dx != 0.0) || (dy != 0.0)) {

        const double t = ((p.x - a.x) * dx + (p.y - a.y) * dy) / (dx * dx + dy * dy);

        if (t > 1) {
            x = b.x;
            y = b.y;

        } else if (t > 0) {
            x += dx * t;
            y += dy * t;
        }
    }

    dx = p.x - x;
    dy = p.y - y;

    return dx * dx + dy * dy;
}

// calculate simplification data using optimized Douglas-Peucker algorithm
inline void simplify(std::vector<vt_point>& points, size_t first, size_t last, double sqTolerance) {
    double maxSqDist = sqTolerance;
    size_t index = 0;

    for (auto i = first + 1; i < last; i++) {
        const double sqDist = getSqSegDist(points[i], points[first], points[last]);

        if (sqDist > maxSqDist) {
            index = i;
            maxSqDist = sqDist;
        }
    }

    if (maxSqDist > sqTolerance) {
        // save the point importance in squared pixels as a z coordinate
        points[index].z = maxSqDist;
        if (index - first > 1)
            simplify(points, first, index, sqTolerance);
        if (last - index > 1)
            simplify(points, index, last, sqTolerance);
    }
}

inline void simplify(std::vector<vt_point>& points, double tolerance) {
    const size_t len = points.size();

    // always retain the endpoints (1 is the max value)
    points[0].z = 1.0;
    points[len - 1].z = 1.0;

    simplify(points, 0, len - 1, tolerance * tolerance);
}

} // namespace detail
} // namespace geojsonvt
} // namespace mapbox
