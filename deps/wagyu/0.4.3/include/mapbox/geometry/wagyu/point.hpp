#pragma once

#include <mapbox/geometry/point.hpp>

#ifdef DEBUG
#include <iostream>
#endif

namespace mapbox {
namespace geometry {
namespace wagyu {

template <typename T>
struct point;

template <typename T>
using point_ptr = point<T>*;

template <typename T>
using const_point_ptr = point<T>* const;

template <typename T>
struct ring;

template <typename T>
using ring_ptr = ring<T>*;

template <typename T>
using const_ring_ptr = ring<T>* const;

template <typename T>
struct point {
    using coordinate_type = T;
    ring_ptr<T> ring;
    T x;
    T y;
    point_ptr<T> next;
    point_ptr<T> prev;

    point() : ring(nullptr), x(0), y(0), prev(this), next(this) {
    }

    point(T x_, T y_) : ring(nullptr), x(x_), y(y_), next(this), prev(this) {
    }

    point(ring_ptr<T> ring_, mapbox::geometry::point<T> const& pt)
        : ring(ring_), x(pt.x), y(pt.y), next(this), prev(this) {
    }

    point(ring_ptr<T> ring_, mapbox::geometry::point<T> const& pt, point_ptr<T> before_this_point)
        : ring(ring_), x(pt.x), y(pt.y), next(before_this_point), prev(before_this_point->prev) {
        before_this_point->prev = this;
        prev->next = this;
    }
};

template <typename T>
using point_vector = std::vector<point_ptr<T>>;

template <typename T>
using point_vector_itr = typename point_vector<T>::iterator;

template <typename T>
bool operator==(point<T> const& lhs, point<T> const& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

template <typename T>
bool operator==(mapbox::geometry::point<T> const& lhs, point<T> const& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

template <typename T>
bool operator==(point<T> const& lhs, mapbox::geometry::point<T> const& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

template <typename T>
bool operator!=(point<T> const& lhs, point<T> const& rhs) {
    return lhs.x != rhs.x || lhs.y != rhs.y;
}

template <typename T>
bool operator!=(mapbox::geometry::point<T> const& lhs, point<T> const& rhs) {
    return lhs.x != rhs.x || lhs.y != rhs.y;
}

template <typename T>
bool operator!=(point<T> const& lhs, mapbox::geometry::point<T> const& rhs) {
    return lhs.x != rhs.x || lhs.y != rhs.y;
}

#ifdef DEBUG

template <class charT, class traits, typename T>
inline std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& out,
                                                     const point<T>& p) {
    out << "  point at: " << p.x << ", " << p.y;
    return out;
}

template <class charT, class traits, typename T>
inline std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& out,
                                                     const mapbox::geometry::point<T>& p) {
    out << "  point at: " << p.x << ", " << p.y;
    return out;
}
#endif
}
}
}
