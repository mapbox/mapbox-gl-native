#pragma once

#include <cmath>
#include <limits>
#include <list>

#include <mapbox/geometry/point.hpp>
#include <mapbox/geometry/wagyu/config.hpp>
#include <mapbox/geometry/wagyu/util.hpp>

#ifdef DEBUG
#include <iostream>
#endif

namespace mapbox {
namespace geometry {
namespace wagyu {

template <typename T>
struct bound;

template <typename T>
using bound_ptr = bound<T>*;

template <typename T>
struct edge {
    mapbox::geometry::point<T> bot;
    mapbox::geometry::point<T> top;
    double dx;

    edge(edge<T>&& e) noexcept : bot(std::move(e.bot)), top(std::move(e.top)), dx(std::move(e.dx)) {
    }

    edge& operator=(edge<T>&& e) noexcept {
        bot = std::move(e.bot);
        top = std::move(e.top);
        dx = std::move(e.dx);
        return *this;
    }

    template <typename T2>
    edge(mapbox::geometry::point<T2> const& current,
         mapbox::geometry::point<T2> const& next_pt) noexcept
        : bot(static_cast<T>(current.x), static_cast<T>(current.y)),
          top(static_cast<T>(current.x), static_cast<T>(current.y)),
          dx(0.0) {
        if (current.y >= next_pt.y) {
            top = mapbox::geometry::point<T>(static_cast<T>(next_pt.x), static_cast<T>(next_pt.y));
        } else {
            bot = mapbox::geometry::point<T>(static_cast<T>(next_pt.x), static_cast<T>(next_pt.y));
        }
        double dy = static_cast<double>(top.y - bot.y);
        if (value_is_zero(dy)) {
            dx = std::numeric_limits<double>::infinity();
        } else {
            dx = static_cast<double>(top.x - bot.x) / dy;
        }
    }
};

template <typename T>
using edge_ptr = edge<T>*;

template <typename T>
using edge_list = std::vector<edge<T>>;

template <typename T>
using edge_list_itr = typename edge_list<T>::iterator;

template <typename T>
bool slopes_equal(edge<T> const& e1, edge<T> const& e2) {
    return (e1.top.y - e1.bot.y) * (e2.top.x - e2.bot.x) ==
           (e1.top.x - e1.bot.x) * (e2.top.y - e2.bot.y);
}

template <typename T>
inline bool is_horizontal(edge<T> const& e) {
    return std::isinf(e.dx);
}

template <typename T>
inline double get_current_x(edge<T> const& edge, const T current_y) {
    if (current_y == edge.top.y) {
        return static_cast<double>(edge.top.x);
    } else {
        return static_cast<double>(edge.bot.x) +
               edge.dx * static_cast<double>(current_y - edge.bot.y);
    }
}

#ifdef DEBUG

template <class charT, class traits, typename T>
inline std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& out,
                                                     const edge<T>& e) {
    out << "    Edge: " << std::endl;
    out << "        bot x: " << e.bot.x << " y: " << e.bot.y << std::endl;
    out << "        top x: " << e.top.x << " y: " << e.top.y << std::endl;
    return out;
}

template <class charT, class traits, typename T>
inline std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& out,
                                                     edge_list<T> const& edges) {
    out << "[";
    bool first = true;
    for (auto const& e : edges) {
        if (first) {
            first = false;
        } else {
            out << ",";
        }
        out << "[[" << e.bot.x << "," << e.bot.y << "],[";
        out << e.top.x << "," << e.top.y << "]]";
    }
    out << "]";
    return out;
}

#endif
}
}
}
