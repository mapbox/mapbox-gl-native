#include <mbgl/util/intersection_tests.hpp>
#include <mbgl/util/math.hpp>

namespace mbgl {
namespace util {

bool polygonContainsPoint(const GeometryCoordinates& ring, const GeometryCoordinate& p) {
    bool c = false;
    for (auto i = ring.begin(), j = ring.end() - 1; i != ring.end(); j = i++) {
        auto& p1 = *i;
        auto& p2 = *j;
        if (((p1.y > p.y) != (p2.y > p.y)) && (p.x < float(p2.x - p1.x) * float(p.y - p1.y) / float(p2.y - p1.y) + p1.x)) {
            c = !c;
        }
    }
    return c;
}

// Code from http://stackoverflow.com/a/1501725/331379.
float distToSegmentSquared(const GeometryCoordinate& p, const GeometryCoordinate& v, const GeometryCoordinate& w) {
    if (v == w) return util::distSqr<float>(p, v);
    const auto l2 = util::distSqr<float>(v, w);
    const float t = float((p.x - v.x) * (w.x - v.x) + (p.y - v.y) * (w.y - v.y)) / l2;
    if (t < 0) return util::distSqr<float>(p, v);
    if (t > 1) return util::distSqr<float>(p, w);
    return util::distSqr<float>(p, convertPoint<float>(w - v) * t + convertPoint<float>(v));
}

bool pointIntersectsBufferedLine(const GeometryCoordinate& p, const GeometryCoordinates& line, const float radius) {
    const float radiusSquared = radius * radius;

    if (line.size() == 1) return util::distSqr<float>(p, line.at(0)) < radiusSquared;
    if (line.size() == 0) return false;

    for (auto i = line.begin() + 1; i != line.end(); i++) {
        // Find line segments that have a distance <= radius^2 to p
        // In that case, we treat the line as "containing point p".
        auto& v = *(i - 1);
        auto& w = *i;
        if (distToSegmentSquared(p, v, w) < radiusSquared) return true;
    }
    return false;
}

// http://bryceboe.com/2006/10/23/line-segment-intersection-algorithm/
bool isCounterClockwise(const GeometryCoordinate& a, const GeometryCoordinate& b, const GeometryCoordinate& c) {
    return (c.y - a.y) * (b.x - a.x) > (b.y - a.y) * (c.x - a.x);
}

bool lineSegmentIntersectsLineSegment(const GeometryCoordinate& a0, const GeometryCoordinate& a1, const GeometryCoordinate& b0, const GeometryCoordinate& b1) {
    return isCounterClockwise(a0, b0, b1) != isCounterClockwise(a1, b0, b1) &&
        isCounterClockwise(a0, a1, b0) != isCounterClockwise(a0, a1, b1);
}
bool lineIntersectsLine(const GeometryCoordinates& lineA, const GeometryCoordinates& lineB) {
    if (lineA.size() == 0 || lineB.size() == 0) return false;
    for (auto i = lineA.begin(); i != lineA.end() - 1; i++) {
        auto& a0 = *i;
        auto& a1 = *(i + 1);
        for (auto j = lineB.begin(); j != lineB.end() - 1; j++) {
            auto& b0 = *j;
            auto& b1 = *(j + 1);
            if (lineSegmentIntersectsLineSegment(a0, a1, b0, b1)) return true;
        }
    }
    return false;
}

bool lineIntersectsBufferedLine(const GeometryCoordinates& lineA, const GeometryCoordinates& lineB, float radius) {
    if (lineA.size() > 1) {
        if (lineIntersectsLine(lineA, lineB)) return true;

        // Check whether any point in either line is within radius of the other line
        for (auto& p : lineB) {
            if (pointIntersectsBufferedLine(p, lineA, radius)) return true;
        }
    }

    for (auto& p : lineA) {
        if (pointIntersectsBufferedLine(p, lineB, radius)) return true;
    }

    return false;
}

bool polygonIntersectsBufferedPoint(const GeometryCoordinates& polygon, const GeometryCoordinate& point, float radius) {
    if (polygonContainsPoint(polygon, point)) return true;
    if (pointIntersectsBufferedLine(point, polygon, radius)) return true;
    return false;
}

bool polygonIntersectsBufferedMultiPoint(const GeometryCoordinates& polygon, const GeometryCollection& rings, float radius) {
    for (auto& ring : rings) {
        for (auto& point : ring) {
            if (polygonIntersectsBufferedPoint(polygon, point, radius)) return true;
        }
    }
    return false;
}

bool polygonIntersectsBufferedMultiLine(const GeometryCoordinates& polygon, const GeometryCollection& multiLine, float radius) {
    for (auto& line : multiLine) {
        if (polygon.size() >= 3) {
            for (auto& p : line) {
                if (polygonContainsPoint(polygon, p)) return true;
            }
        }

        if (lineIntersectsBufferedLine(polygon, line, radius)) return true;
    }

    return false;
}

bool polygonIntersectsPolygon(const GeometryCoordinates& polygonA, const GeometryCoordinates& polygonB) {
    for (auto& p : polygonA) {
        if (polygonContainsPoint(polygonB, p)) return true;
    }

    for (auto& p : polygonB) {
        if (polygonContainsPoint(polygonA, p)) return true;
    }

    if (lineIntersectsLine(polygonA, polygonB)) return true;

    return false;
}

bool polygonIntersectsMultiPolygon(const GeometryCoordinates& polygon, const GeometryCollection& multiPolygon) {
    for (auto& ring : multiPolygon) {
        if (polygonIntersectsPolygon(polygon, ring)) return true;
    }

    return false;
}

} // namespace util
} // namespace mbgl
