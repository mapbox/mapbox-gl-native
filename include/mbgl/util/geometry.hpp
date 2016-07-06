#pragma once

#include <mapbox/geometry/geometry.hpp>
#include <mapbox/geometry/point_arithmetic.hpp>

#include <mbgl/util/geo.hpp>

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

template <class T>
LatLngBounds boundsForGeometry(const Point<T>& p) {
    return LatLngBounds::singleton(LatLng(p.y, p.x));
}

template <class T>
LatLngBounds boundsForGeometry(const LineString<T>& l) {
    LatLngBounds bounds = LatLngBounds::empty();
    for (auto &point: l) {
        bounds.extend(LatLng(point.y, point.x));
    }
    return bounds;
}

template <class T>
LatLngBounds boundsForGeometry(const Polygon<T>& p) {
    LatLngBounds bounds = LatLngBounds::empty();
    if (!p.empty()) {
        for (auto &point: p.front()) {
            bounds.extend(LatLng(point.y, point.x));
        }
    }
    return bounds;
}

template <class T>
LatLngBounds boundsForGeometry(const MultiPoint<T>& m) {
    LatLngBounds bounds = LatLngBounds::empty();
    for (auto &point: m) {
        bounds.extend(LatLng(point.y, point.x));
    }
    return bounds;
}

template <class T>
LatLngBounds boundsForGeometry(const MultiLineString<T>& m) {
    LatLngBounds bounds = LatLngBounds::empty();
    for (auto &line: m) {
        for (auto &point: line) {
            bounds.extend(LatLng(point.y, point.x));
        }
    }
    return bounds;
}

template <class T>
LatLngBounds boundsForGeometry(const MultiPolygon<T>& m) {
    LatLngBounds bounds = LatLngBounds::empty();
    for (auto &geometry: m) {
        if (!geometry.empty()) {
            for (auto &point: geometry.front()) {
                bounds.extend(LatLng(point.y, point.x));
            }
        }
    }
    return bounds;
}

} // namespace mbgl
