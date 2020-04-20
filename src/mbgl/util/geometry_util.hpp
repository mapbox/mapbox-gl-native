#pragma once

#include <array>
#include <mbgl/util/geometry.hpp>

namespace mbgl {

// contains minX, minY, maxX, maxY
template <typename T>
using GeometryBBox = std::array<T, 4>;
const GeometryBBox<int64_t> DefaultWithinBBox = std::array<int64_t, 4>{std::numeric_limits<int64_t>::max(),
                                                                       std::numeric_limits<int64_t>::max(),
                                                                       std::numeric_limits<int64_t>::min(),
                                                                       std::numeric_limits<int64_t>::min()};

const GeometryBBox<double> DefaultDistanceBBox = std::array<double, 4>{std::numeric_limits<double>::infinity(),
                                                                       std::numeric_limits<double>::infinity(),
                                                                       -std::numeric_limits<double>::infinity(),
                                                                       -std::numeric_limits<double>::infinity()};

template <typename T>
void updateBBox(GeometryBBox<T>& bbox, const Point<T>& p);

// check if bbox1 is within bbox2
template <typename T>
bool boxWithinBox(const GeometryBBox<T>& bbox1, const GeometryBBox<T>& bbox2);

template <typename T>
bool segmentIntersectSegment(const Point<T>& a, const Point<T>& b, const Point<T>& c, const Point<T>& d);

template <typename T>
bool rayIntersect(const Point<T>& p, const Point<T>& p1, const Point<T>& p2);

template <typename T>
bool pointOnBoundary(const Point<T>& p, const Point<T>& p1, const Point<T>& p2);

template <typename T>
bool lineIntersectPolygon(const Point<T>& p1, const Point<T>& p2, const Polygon<T>& polygon);

template <typename T>
bool pointWithinPolygon(const Point<T>& point, const Polygon<T>& polygon, bool trueOnBoundary = false);

template <typename T>
bool pointWithinPolygons(const Point<T>& point, const MultiPolygon<T>& polygons, bool trueOnBoundary = false);

template <typename T>
bool lineStringWithinPolygon(const LineString<T>& line, const Polygon<T>& polygon);

template <typename T>
bool lineStringWithinPolygons(const LineString<T>& line, const MultiPolygon<T>& polygons);

} // namespace mbgl
