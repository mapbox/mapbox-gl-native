#pragma once

#include <mapbox/geometry/geometry.hpp>
#include <mapbox/geometry/point_arithmetic.hpp>

namespace mbgl {

template <class T>
using Point = mapbox::geometry::point<T>;

template <class T>
using LineString = mapbox::geometry::line_string<T>;

template <class T>
using Polygon = mapbox::geometry::polygon<T>;

template <class T>
using MultiPoint = mapbox::geometry::multi_point<T>;

template <class T>
using MultiLineString = mapbox::geometry::multi_line_string<T>;

template <class T>
using MultiPolygon = mapbox::geometry::multi_polygon<T>;

} // namespace mbgl
