#pragma once

#include <cmath>

#include <mapbox/geometry/point.hpp>
#include <mapbox/geometry/polygon.hpp>
#include <mapbox/geometry/wagyu/point.hpp>

namespace mapbox {
namespace geometry {
namespace wagyu {

template <typename T>
double area(mapbox::geometry::linear_ring<T> const& poly) {
    std::size_t size = poly.size();
    if (size < 3) {
        return 0.0;
    }

    double a = 0.0;
    auto itr = poly.begin();
    auto itr_prev = poly.end();
    --itr_prev;
    a += static_cast<double>(itr_prev->x + itr->x) * static_cast<double>(itr_prev->y - itr->y);
    ++itr;
    itr_prev = poly.begin();
    for (; itr != poly.end(); ++itr, ++itr_prev) {
        a += static_cast<double>(itr_prev->x + itr->x) * static_cast<double>(itr_prev->y - itr->y);
    }
    return -a * 0.5;
}

inline bool value_is_zero(double val) {
    return std::fabs(val) < (5.0 * std::numeric_limits<double>::epsilon());
}

inline bool values_are_equal(double x, double y) {
    return value_is_zero(x - y);
}

inline bool greater_than_or_equal(double x, double y) {
    return x > y || values_are_equal(x, y);
}

template <typename T>
bool slopes_equal(mapbox::geometry::point<T> const& pt1,
                  mapbox::geometry::point<T> const& pt2,
                  mapbox::geometry::point<T> const& pt3) {
    return (pt1.y - pt2.y) * (pt2.x - pt3.x) == (pt1.x - pt2.x) * (pt2.y - pt3.y);
}

template <typename T>
bool slopes_equal(mapbox::geometry::wagyu::point<T> const& pt1,
                  mapbox::geometry::wagyu::point<T> const& pt2,
                  mapbox::geometry::point<T> const& pt3) {
    return (pt1.y - pt2.y) * (pt2.x - pt3.x) == (pt1.x - pt2.x) * (pt2.y - pt3.y);
}

template <typename T>
bool slopes_equal(mapbox::geometry::wagyu::point<T> const& pt1,
                  mapbox::geometry::wagyu::point<T> const& pt2,
                  mapbox::geometry::wagyu::point<T> const& pt3) {
    return (pt1.y - pt2.y) * (pt2.x - pt3.x) == (pt1.x - pt2.x) * (pt2.y - pt3.y);
}

template <typename T>
bool slopes_equal(mapbox::geometry::point<T> const& pt1,
                  mapbox::geometry::point<T> const& pt2,
                  mapbox::geometry::point<T> const& pt3,
                  mapbox::geometry::point<T> const& pt4) {
    return (pt1.y - pt2.y) * (pt3.x - pt4.x) == (pt1.x - pt2.x) * (pt3.y - pt4.y);
}

template <typename T>
inline T wround(double value) {
    return static_cast<T>(::llround(value));
}

template <>
inline std::int64_t wround<std::int64_t>(double value) {
    return ::llround(value);
}
}
}
}
