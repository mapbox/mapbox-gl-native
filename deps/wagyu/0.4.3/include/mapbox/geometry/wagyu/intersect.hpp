#pragma once

#include <set>

#include <mapbox/geometry/point.hpp>

#include <mapbox/geometry/wagyu/active_bound_list.hpp>

#ifdef DEBUG
#include <iostream>
#endif

namespace mapbox {
namespace geometry {
namespace wagyu {

template <typename T>
struct intersect_node {

    bound_ptr<T> bound1;
    bound_ptr<T> bound2;
    mapbox::geometry::point<double> pt;

    intersect_node(intersect_node<T>&& n)
        : bound1(std::move(n.bound1)), bound2(std::move(n.bound2)), pt(std::move(n.pt)) {
    }

    intersect_node& operator=(intersect_node<T>&& n) {
        bound1 = std::move(n.bound1);
        bound2 = std::move(n.bound2);
        pt = std::move(n.pt);
        return *this;
    }

    intersect_node(bound_ptr<T> const& bound1_,
                   bound_ptr<T> const& bound2_,
                   mapbox::geometry::point<double> const& pt_)
        : bound1(bound1_), bound2(bound2_), pt(pt_) {
    }
};

template <typename T>
using intersect_list = std::vector<intersect_node<T>>;

#ifdef DEBUG

template <class charT, class traits, typename T>
inline std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& out,
                                                     const intersect_node<T>& e) {
    out << " point x: " << e.pt.x << " y: " << e.pt.y << std::endl;
    out << " bound 1: " << std::endl;
    out << *e.bound1 << std::endl;
    out << " bound 2: " << std::endl;
    out << *e.bound2 << std::endl;
    return out;
}

template <class charT, class traits, typename T>
inline std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& out,
                                                     const intersect_list<T>& ints) {
    std::size_t c = 0;
    for (auto const& i : ints) {
        out << "Intersection: " << c++ << std::endl;
        out << i;
    }
    return out;
}

#endif
}
}
}
