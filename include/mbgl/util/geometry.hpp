#pragma once

#include <mapbox/geometry/geometry.hpp>
#include <mapbox/geometry/point_arithmetic.hpp>
#include <mapbox/geometry/for_each_point.hpp>

namespace mbgl {

enum class FeatureType : uint8_t {
    Unknown = 0,
    Point = 1,
    LineString = 2,
    Polygon = 3
};

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

template <class T>
using LinearRing = mapbox::geometry::linear_ring<T>;

template <class T>
using Geometry = mapbox::geometry::geometry<T>;

template <class S, class T>
Point<S> convertPoint(const Point<T>& p) {
    return Point<S>(p.x, p.y);
}

struct ToFeatureType {
    template <class T>
    FeatureType operator()(const Point<T> &) const { return FeatureType::Point; }
    template <class T>
    FeatureType operator()(const MultiPoint<T> &) const { return FeatureType::Point; }
    template <class T>
    FeatureType operator()(const LineString<T> &) const { return FeatureType::LineString; }
    template <class T>
    FeatureType operator()(const MultiLineString<T> &) const { return FeatureType::LineString; }
    template <class T>
    FeatureType operator()(const Polygon<T> &) const { return FeatureType::Polygon; }
    template <class T>
    FeatureType operator()(const MultiPolygon<T> &) const { return FeatureType::Polygon; }
    template <class T>
    FeatureType operator()(const mapbox::geometry::geometry_collection<T> &) const { return FeatureType::Unknown; }
};

template <class T, typename F>
auto forEachPoint(const Geometry<T>& g, F f) {
    mapbox::geometry::for_each_point(g, f);
}

} // namespace mbgl
