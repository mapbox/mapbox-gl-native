#pragma once
#include <mbgl/util/geometry.hpp>

namespace mbgl {

bool pointWithinPolygon(const Point<double>& point, const Polygon<double>& polygon);

bool pointWithinPolygons(const Point<double>& point, const MultiPolygon<double>& polygons);

bool lineStringWithinPolygon(const LineString<double>& lineString, const Polygon<double>& polygon);

bool lineStringWithinPolygons(const LineString<double>& line, const MultiPolygon<double>& polygons);
} // namespace mbgl
