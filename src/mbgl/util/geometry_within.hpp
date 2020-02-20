#pragma once

#include <array>
#include <limits>
#include <mbgl/util/geometry.hpp>

namespace mbgl {

// contains minX, minY, maxX, maxY
using WithinBBox = std::array<double, 4>;
const WithinBBox DefaultBBox = WithinBBox{std::numeric_limits<double>::infinity(),
                                          std::numeric_limits<double>::infinity(),
                                          -std::numeric_limits<double>::infinity(),
                                          -std::numeric_limits<double>::infinity()};

// check if bbox1 is within bbox2
bool boxWithinBox(const WithinBBox& bbox1, const WithinBBox& bbox2);

WithinBBox calculateBBox(const Geometry<double>& geometries);

bool pointWithinPolygon(const Point<double>& point, const Polygon<double>& polygon);

bool pointWithinPolygons(const Point<double>& point, const MultiPolygon<double>& polygons);

bool lineStringWithinPolygon(const LineString<double>& lineString, const Polygon<double>& polygon);

bool lineStringWithinPolygons(const LineString<double>& line, const MultiPolygon<double>& polygons);

} // namespace mbgl
