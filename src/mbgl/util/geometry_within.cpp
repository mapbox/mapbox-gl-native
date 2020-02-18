#include <mbgl/util/geometry_within.hpp>
#include <mbgl/util/logging.hpp>

namespace mbgl {

namespace {
bool rayIntersect(const Point<double>& p, const Point<double>& p1, const Point<double>& p2) {
    return ((p1.y > p.y) != (p2.y > p.y)) && (p.x < (p2.x - p1.x) * (p.y - p1.y) / (p2.y - p1.y) + p1.x);
}

// a, b are end points for line segment1, c and d are end points for line segment2
bool lineIntersectLine(const Point<double>& a, const Point<double>& b, const Point<double>& c, const Point<double>& d) {
    const auto perp = [](const Point<double>& v1, const Point<double>& v2) { return (v1.x * v2.y - v1.y * v2.x); };

    // check if two segments are parallel or not
    // precondition is end point a, b is inside polygon, if line a->b is
    // parallel to polygon edge c->d, then a->b won't intersect with c->d
    auto vectorP = Point<double>(b.x - a.x, b.y - a.y);
    auto vectorQ = Point<double>(d.x - c.x, d.y - c.y);
    if (perp(vectorQ, vectorP) == 0) return false;

    // check if p1 and p2 are in different sides of line segment q1->q2
    const auto twoSided =
        [](const Point<double>& p1, const Point<double>& p2, const Point<double>& q1, const Point<double>& q2) {
            double x1, y1, x2, y2, x3, y3;

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

bool lineIntersectPolygon(const Point<double>& p1, const Point<double>& p2, const Polygon<double>& polygon) {
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

// ray casting algorithm for detecting if point is in polygon
bool pointWithinPolygon(const Point<double>& point, const Polygon<double>& polygon) {
    bool within = false;
    for (const auto& ring : polygon) {
        const auto length = ring.size();
        // loop through every edge of the ring
        for (std::size_t i = 0; i < length - 1; ++i) {
            if (rayIntersect(point, ring[i], ring[i + 1])) {
                within = !within;
            }
        }
    }
    return within;
}

bool pointWithinPolygons(const Point<double>& point, const MultiPolygon<double>& polygons) {
    for (const auto& polygon : polygons) {
        if (pointWithinPolygon(point, polygon)) return true;
    }
    return false;
}

bool lineStringWithinPolygon(const LineString<double>& line, const Polygon<double>& polygon) {
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

bool lineStringWithinPolygons(const LineString<double>& line, const MultiPolygon<double>& polygons) {
    for (const auto& polygon : polygons) {
        if (lineStringWithinPolygon(line, polygon)) return true;
    }
    return false;
}
} // namespace mbgl
