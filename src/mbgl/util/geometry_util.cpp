#include <mbgl/util/geometry_util.hpp>

#include <algorithm>

namespace mbgl {

template <typename T>
bool GeometryUtil<T>::rayIntersect(const Point<T>& p, const Point<T>& p1, const Point<T>& p2) {
    return ((p1.y > p.y) != (p2.y > p.y)) && (p.x < (p2.x - p1.x) * (p.y - p1.y) / (p2.y - p1.y) + p1.x);
}

// check if point p is on line segment with end points p1 and p2
template <typename T>
bool GeometryUtil<T>::pointOnBoundary(const Point<T>& p, const Point<T>& p1, const Point<T>& p2) {
    // requirements of point p on line segment:
    // 1. colinear: cross product of vector p->p1(x1, y1) and vector p->p2(x2, y2) equals to 0
    // 2. p is between p1 and p2
    const auto x1 = p.x - p1.x;
    const auto y1 = p.y - p1.y;
    const auto x2 = p.x - p2.x;
    const auto y2 = p.y - p2.y;
    return (x1 * y2 - x2 * y1 == 0) && (x1 * x2 <= 0) && (y1 * y2 <= 0);
}

template <typename T>
bool GeometryUtil<T>::segmentIntersectSegment(const Point<T>& a,
                                              const Point<T>& b,
                                              const Point<T>& c,
                                              const Point<T>& d) {
    // a, b are end points for line segment1, c and d are end points for line segment2
    const auto perp = [](const Point<T>& v1, const Point<T>& v2) { return (v1.x * v2.y - v1.y * v2.x); };

    // check if two segments are parallel or not
    // precondition is end point a, b is inside polygon, if line a->b is
    // parallel to polygon edge c->d, then a->b won't intersect with c->d
    auto vectorP = Point<T>(b.x - a.x, b.y - a.y);
    auto vectorQ = Point<T>(d.x - c.x, d.y - c.y);
    if (perp(vectorQ, vectorP) == 0) return false;

    // check if p1 and p2 are in different sides of line segment q1->q2
    const auto twoSided = [](const Point<T>& p1, const Point<T>& p2, const Point<T>& q1, const Point<T>& q2) {
        // q1->p1 (x1, y1), q1->p2 (x2, y2), q1->q2 (x3, y3)
        T x1 = p1.x - q1.x;
        T y1 = p1.y - q1.y;
        T x2 = p2.x - q1.x;
        T y2 = p2.y - q1.y;
        T x3 = q2.x - q1.x;
        T y3 = q2.y - q1.y;
        auto ret1 = (x1 * y3 - x3 * y1);
        auto ret2 = (x2 * y3 - x3 * y2);
        return (ret1 > 0 && ret2 < 0) || (ret1 < 0 && ret2 > 0);
    };

    // If lines are intersecting with each other, the relative location should be:
    // a and b lie in different sides of segment c->d
    // c and d lie in different sides of segment a->b
    return twoSided(a, b, c, d) && twoSided(c, d, a, b);
}

template <typename T>
bool GeometryUtil<T>::lineIntersectPolygon(const Point<T>& p1, const Point<T>& p2, const Polygon<T>& polygon) {
    for (auto ring : polygon) {
        auto length = ring.size();
        // loop through every edge of the ring
        for (std::size_t i = 0; i < length - 1; ++i) {
            if (segmentIntersectSegment(p1, p2, ring[i], ring[i + 1])) {
                return true;
            }
        }
    }
    return false;
}

// ray casting algorithm for detecting if point is in polygon
template <typename T>
bool GeometryUtil<T>::pointWithinPolygon(const Point<T>& point, const Polygon<T>& polygon, bool trueOnBoundary) {
    bool within = false;
    for (const auto& ring : polygon) {
        const auto length = ring.size();
        // loop through every edge of the ring
        for (std::size_t i = 0; i < length - 1; ++i) {
            if (pointOnBoundary(point, ring[i], ring[i + 1])) return trueOnBoundary;
            if (rayIntersect(point, ring[i], ring[i + 1])) {
                within = !within;
            }
        }
    }
    return within;
}

template <typename T>
bool GeometryUtil<T>::pointWithinPolygons(const Point<T>& point, const MultiPolygon<T>& polygons, bool trueOnBoundary) {
    for (const auto& polygon : polygons) {
        if (pointWithinPolygon(point, polygon, trueOnBoundary)) return true;
    }
    return false;
}

template <typename T>
bool GeometryUtil<T>::lineStringWithinPolygon(const LineString<T>& line, const Polygon<T>& polygon) {
    const auto length = line.size();
    // First, check if geometry points of line segments are all inside polygon
    for (std::size_t i = 0; i < length; ++i) {
        if (!pointWithinPolygon(line[i], polygon)) {
            return false;
        }
    }

    // Second, check if there is line segment intersecting polygon edge
    for (std::size_t i = 0; i < length - 1; ++i) {
        if (lineIntersectPolygon(line[i], line[i + 1], polygon)) {
            return false;
        }
    }
    return true;
}

template <typename T>
bool GeometryUtil<T>::lineStringWithinPolygons(const LineString<T>& line, const MultiPolygon<T>& polygons) {
    for (const auto& polygon : polygons) {
        if (lineStringWithinPolygon(line, polygon)) return true;
    }
    return false;
}

template struct GeometryUtil<double>;
template struct GeometryUtil<int64_t>;

} // namespace mbgl
