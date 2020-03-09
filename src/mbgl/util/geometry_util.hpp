#pragma once

#include <mbgl/util/geometry.hpp>

namespace mbgl {

template <typename T>
struct GeometryUtil {
    using type = T;
    static bool segmentIntersectSegment(const Point<type>& a,
                                        const Point<type>& b,
                                        const Point<type>& c,
                                        const Point<type>& d);
    static bool rayIntersect(const Point<type>& p, const Point<type>& p1, const Point<type>& p2);
    static bool pointOnBoundary(const Point<type>& p, const Point<type>& p1, const Point<type>& p2);
    static bool lineIntersectPolygon(const Point<type>& p1, const Point<type>& p2, const Polygon<type>& polygon);
    static bool pointWithinPolygon(const Point<type>& point, const Polygon<type>& polygon, bool trueOnBoundary = false);
    static bool pointWithinPolygons(const Point<type>& point,
                                    const MultiPolygon<type>& polygons,
                                    bool trueOnBoundary = false);
    static bool lineStringWithinPolygon(const LineString<type>& line, const Polygon<type>& polygon);
    static bool lineStringWithinPolygons(const LineString<type>& line, const MultiPolygon<type>& polygons);
};

} // namespace mbgl
