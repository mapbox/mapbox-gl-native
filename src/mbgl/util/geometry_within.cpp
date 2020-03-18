#include <mbgl/util/geometry_within.hpp>

#include <algorithm>

namespace mbgl {

namespace {
bool rayIntersect(const Point<int64_t>& p, const Point<int64_t>& p1, const Point<int64_t>& p2) {
    return ((p1.y > p.y) != (p2.y > p.y)) && (p.x < (p2.x - p1.x) * (p.y - p1.y) / (p2.y - p1.y) + p1.x);
}

// check if point p in on line segment with end points p1 and p2
bool onBoundary(const Point<int64_t>& p, const Point<int64_t>& p1, const Point<int64_t>& p2) {
    // requirements of point p on line segment:
    // 1. colinear: cross product of vector p->p1(x1, y1) and vector p->p2(x2, y2) equals to 0
    // 2. p is between p1 and p2
    const auto x1 = p.x - p1.x;
    const auto y1 = p.y - p1.y;
    const auto x2 = p.x - p2.x;
    const auto y2 = p.y - p2.y;
    return (x1 * y2 - x2 * y1 == 0) && (x1 * x2 <= 0) && (y1 * y2 <= 0);
}

// a, b are end points for line segment1, c and d are end points for line segment2
bool lineIntersectLine(const Point<int64_t>& a,
                       const Point<int64_t>& b,
                       const Point<int64_t>& c,
                       const Point<int64_t>& d) {
    const auto perp = [](const Point<int64_t>& v1, const Point<int64_t>& v2) { return (v1.x * v2.y - v1.y * v2.x); };

    // check if two segments are parallel or not
    // precondition is end point a, b is inside polygon, if line a->b is
    // parallel to polygon edge c->d, then a->b won't intersect with c->d
    auto vectorP = Point<int64_t>(b.x - a.x, b.y - a.y);
    auto vectorQ = Point<int64_t>(d.x - c.x, d.y - c.y);
    if (perp(vectorQ, vectorP) == 0) return false;

    // check if p1 and p2 are in different sides of line segment q1->q2
    const auto twoSided =
        [](const Point<int64_t>& p1, const Point<int64_t>& p2, const Point<int64_t>& q1, const Point<int64_t>& q2) {
            int64_t x1, y1, x2, y2, x3, y3;

            // q1->p1 (x1, y1), q1->p2 (x2, y2), q1->q2 (x3, y3)
            x1 = p1.x - q1.x;
            y1 = p1.y - q1.y;
            x2 = p2.x - q1.x;
            y2 = p2.y - q1.y;
            x3 = q2.x - q1.x;
            y3 = q2.y - q1.y;
            if ((x1 * y3 - x3 * y1) * (x2 * y3 - x3 * y2) < 0) return true;
            return false;
        };

    // If lines are intersecting with each other, the relative location should be:
    // a and b lie in different sides of segment c->d
    // c and d lie in different sides of segment a->b
    if (twoSided(a, b, c, d) && twoSided(c, d, a, b)) return true;
    return false;
}

bool lineIntersectPolygon(const Point<int64_t>& p1, const Point<int64_t>& p2, const Polygon<int64_t>& polygon) {
    for (auto ring : polygon) {
        auto length = ring.size();
        // loop through every edge of the ring
        for (std::size_t i = 0; i < length - 1; ++i) {
            if (lineIntersectLine(p1, p2, ring[i], ring[i + 1])) {
                return true;
            }
        }
    }
    return false;
}

} // namespace

void updateBBox(WithinBBox& bbox, const Point<int64_t>& p) {
    bbox[0] = std::min(p.x, bbox[0]);
    bbox[1] = std::min(p.y, bbox[1]);
    bbox[2] = std::max(p.x, bbox[2]);
    bbox[3] = std::max(p.y, bbox[3]);
}

bool boxWithinBox(const WithinBBox& bbox1, const WithinBBox& bbox2) {
    if (bbox1[0] <= bbox2[0]) return false;
    if (bbox1[2] >= bbox2[2]) return false;
    if (bbox1[1] <= bbox2[1]) return false;
    if (bbox1[3] >= bbox2[3]) return false;
    return true;
}

// ray casting algorithm for detecting if point is in polygon
bool pointWithinPolygon(const Point<int64_t>& point, const Polygon<int64_t>& polygon) {
    bool within = false;
    for (const auto& ring : polygon) {
        const auto length = ring.size();
        // loop through every edge of the ring
        for (std::size_t i = 0; i < length - 1; ++i) {
            if (onBoundary(point, ring[i], ring[i + 1])) return false;
            if (rayIntersect(point, ring[i], ring[i + 1])) {
                within = !within;
            }
        }
    }
    return within;
}

bool pointWithinPolygons(const Point<int64_t>& point, const MultiPolygon<int64_t>& polygons) {
    for (const auto& polygon : polygons) {
        if (pointWithinPolygon(point, polygon)) return true;
    }
    return false;
}

bool lineStringWithinPolygon(const LineString<int64_t>& line, const Polygon<int64_t>& polygon) {
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

bool lineStringWithinPolygons(const LineString<int64_t>& line, const MultiPolygon<int64_t>& polygons) {
    for (const auto& polygon : polygons) {
        if (lineStringWithinPolygon(line, polygon)) return true;
    }
    return false;
}
} // namespace mbgl
