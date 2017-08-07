#pragma once

#ifdef DEBUG
#include <iostream>
#include <sstream>
#endif

#include <queue>

#include <mapbox/geometry/wagyu/bound.hpp>

namespace mapbox {
namespace geometry {
namespace wagyu {

template <typename T>
struct local_minimum {
    bound<T> left_bound;
    bound<T> right_bound;
    T y;
    bool minimum_has_horizontal;

    local_minimum(bound<T>&& left_bound_, bound<T>&& right_bound_, T y_, bool has_horz_)
        : left_bound(std::move(left_bound_)),
          right_bound(std::move(right_bound_)),
          y(y_),
          minimum_has_horizontal(has_horz_) {
    }
};

template <typename T>
using local_minimum_list = std::deque<local_minimum<T>>;

template <typename T>
using local_minimum_itr = typename local_minimum_list<T>::iterator;

template <typename T>
using local_minimum_ptr = local_minimum<T>*;

template <typename T>
using local_minimum_ptr_list = std::vector<local_minimum_ptr<T>>;

template <typename T>
using local_minimum_ptr_list_itr = typename local_minimum_ptr_list<T>::iterator;

template <typename T>
struct local_minimum_sorter {
    inline bool operator()(local_minimum_ptr<T> const& locMin1,
                           local_minimum_ptr<T> const& locMin2) {
        if (locMin2->y == locMin1->y) {
            return locMin2->minimum_has_horizontal != locMin1->minimum_has_horizontal &&
                   locMin1->minimum_has_horizontal;
        }
        return locMin2->y < locMin1->y;
    }
};

#ifdef DEBUG

template <class charT, class traits, typename T>
inline std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& out,
                                                     const local_minimum<T>& lm) {
    out << " Local Minimum:" << std::endl;
    out << "    y: " << lm.y << std::endl;
    if (lm.minimum_has_horizontal) {
        out << "    minimum_has_horizontal: true" << std::endl;
    } else {
        out << "    minimum_has_horizontal: false" << std::endl;
    }
    out << "   left_bound: " << std::endl;
    out << lm.left_bound << std::endl;
    out << "   right_bound: " << std::endl;
    out << lm.right_bound << std::endl;
    return out;
}

template <class charT, class traits, typename T>
inline std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& out,
                                                     const local_minimum_ptr_list<T>& lms) {
    for (auto const& lm : lms) {
        out << *lm;
    }
    return out;
}

template <typename T>
std::string output_all_edges(local_minimum_ptr_list<T> const& lms) {
    std::ostringstream out;
    out << "[";
    bool first = true;
    for (auto const& lm : lms) {
        for (auto const& e : lm->left_bound.edges) {
            if (first) {
                first = false;
            } else {
                out << ",";
            }
            out << "[[" << e.bot.x << "," << e.bot.y << "],[";
            out << e.top.x << "," << e.top.y << "]]";
        }
        for (auto const& e : lm->right_bound.edges) {
            if (first) {
                first = false;
            } else {
                out << ",";
            }
            out << "[[" << e.bot.x << "," << e.bot.y << "],[";
            out << e.top.x << "," << e.top.y << "]]";
        }
    }
    out << "]";
    return out.str();
}

#endif
}
}
}
