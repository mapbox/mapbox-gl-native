#pragma once

#ifdef DEBUG
#include <iostream>
#include <sstream>
#endif

#include <mapbox/geometry/wagyu/bound.hpp>
#include <mapbox/geometry/wagyu/config.hpp>
#include <mapbox/geometry/wagyu/edge.hpp>
#include <mapbox/geometry/wagyu/local_minimum.hpp>
#include <mapbox/geometry/wagyu/local_minimum_util.hpp>
#include <mapbox/geometry/wagyu/ring.hpp>
#include <mapbox/geometry/wagyu/scanbeam.hpp>
#include <mapbox/geometry/wagyu/util.hpp>

namespace mapbox {
namespace geometry {
namespace wagyu {

template <typename T>
using active_bound_list = std::vector<bound_ptr<T>>;

template <typename T>
using active_bound_list_itr = typename active_bound_list<T>::iterator;

template <typename T>
using active_bound_list_rev_itr = typename active_bound_list<T>::reverse_iterator;

#ifdef DEBUG

template <class charT, class traits, typename T>
inline std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& out,
                                                     const active_bound_list<T>& bnds) {
    std::size_t c = 0;
    for (auto const& bnd : bnds) {
        out << "Index: " << c++ << std::endl;
        out << *bnd;
    }
    return out;
}

template <typename T>
std::string output_edges(active_bound_list<T> const& bnds) {
    std::ostringstream out;
    out << "[";
    bool first = true;
    for (auto const& bnd : bnds) {
        if (first) {
            first = false;
        } else {
            out << ",";
        }
        out << "[[" << bnd->current_edge->bot.x << "," << bnd->current_edge->bot.y << "],[";
        out << bnd->current_edge->top.x << "," << bnd->current_edge->top.y << "]]";
    }
    out << "]";
    return out.str();
}

#endif

template <typename T>
bool is_even_odd_fill_type(bound<T> const& bound,
                           fill_type subject_fill_type,
                           fill_type clip_fill_type) {
    if (bound.poly_type == polygon_type_subject) {
        return subject_fill_type == fill_type_even_odd;
    } else {
        return clip_fill_type == fill_type_even_odd;
    }
}

template <typename T>
bool is_even_odd_alt_fill_type(bound<T> const& bound,
                               fill_type subject_fill_type,
                               fill_type clip_fill_type) {
    if (bound.poly_type == polygon_type_subject) {
        return clip_fill_type == fill_type_even_odd;
    } else {
        return subject_fill_type == fill_type_even_odd;
    }
}

template <typename T>
struct bound_insert_location {

    bound<T> const& bound2;

    bound_insert_location(bound<T> const& b) : bound2(b) {
    }

    bool operator()(bound_ptr<T> const& b) {
        auto const& bound1 = *b;
        if (values_are_equal(bound2.current_x, bound1.current_x)) {
            if (bound2.current_edge->top.y > bound1.current_edge->top.y) {
                return static_cast<double>(bound2.current_edge->top.x) <
                       get_current_x(*(bound1.current_edge), bound2.current_edge->top.y);
            } else {
                return static_cast<double>(bound1.current_edge->top.x) >
                       get_current_x(*(bound2.current_edge), bound1.current_edge->top.y);
            }
        } else {
            return bound2.current_x < bound1.current_x;
        }
    }
};

template <typename T>
active_bound_list_itr<T>
insert_bound_into_ABL(bound<T>& left, bound<T>& right, active_bound_list<T>& active_bounds) {

    auto itr =
        std::find_if(active_bounds.begin(), active_bounds.end(), bound_insert_location<T>(left));
    return active_bounds.insert(itr, { &left, &right });
}

template <typename T>
inline bool is_maxima(bound<T>& bnd, T y) {
    return bnd.next_edge == bnd.edges.end() && bnd.current_edge->top.y == y;
}

template <typename T>
inline bool is_maxima(active_bound_list_itr<T>& bnd, T y) {
    return is_maxima(*(*bnd), y);
}

template <typename T>
inline bool is_intermediate(bound<T>& bnd, T y) {
    return bnd.next_edge != bnd.edges.end() && bnd.current_edge->top.y == y;
}

template <typename T>
inline bool is_intermediate(active_bound_list_itr<T>& bnd, T y) {
    return is_intermediate(*(*bnd), y);
}

template <typename T>
inline bool current_edge_is_horizontal(active_bound_list_itr<T>& bnd) {
    return is_horizontal(*((*bnd)->current_edge));
}

template <typename T>
inline bool next_edge_is_horizontal(active_bound_list_itr<T>& bnd) {
    return is_horizontal(*((*bnd)->next_edge));
}

template <typename T>
void next_edge_in_bound(bound<T>& bnd, scanbeam_list<T>& scanbeam) {
    auto& current_edge = bnd.current_edge;
    ++current_edge;
    if (current_edge != bnd.edges.end()) {
        ++(bnd.next_edge);
        bnd.current_x = static_cast<double>(current_edge->bot.x);
        if (!is_horizontal<T>(*current_edge)) {
            scanbeam.push_back(current_edge->top.y);
        }
    }
}

template <typename T>
active_bound_list_itr<T> get_maxima_pair(active_bound_list_itr<T> const& bnd,
                                         active_bound_list<T>& active_bounds) {
    bound_ptr<T> maximum = (*bnd)->maximum_bound;
    return std::find(active_bounds.begin(), active_bounds.end(), maximum);
}

template <typename T>
void set_winding_count(active_bound_list_itr<T>& bnd_itr,
                       active_bound_list<T>& active_bounds,
                       fill_type subject_fill_type,
                       fill_type clip_fill_type) {

    auto rev_bnd_itr = active_bound_list_rev_itr<T>(bnd_itr);
    if (rev_bnd_itr == active_bounds.rend()) {
        (*bnd_itr)->winding_count = (*bnd_itr)->winding_delta;
        (*bnd_itr)->winding_count2 = 0;
        return;
    }

    // find the edge of the same polytype that immediately preceeds 'edge' in
    // AEL
    while (rev_bnd_itr != active_bounds.rend() &&
           (*rev_bnd_itr)->poly_type != (*bnd_itr)->poly_type) {
        ++rev_bnd_itr;
    }
    if (rev_bnd_itr == active_bounds.rend()) {
        (*bnd_itr)->winding_count = (*bnd_itr)->winding_delta;
        (*bnd_itr)->winding_count2 = 0;
    } else if (is_even_odd_fill_type(*(*bnd_itr), subject_fill_type, clip_fill_type)) {
        // EvenOdd filling ...
        (*bnd_itr)->winding_count = (*bnd_itr)->winding_delta;
        (*bnd_itr)->winding_count2 = (*rev_bnd_itr)->winding_count2;
    } else {
        // nonZero, Positive or Negative filling ...
        if ((*rev_bnd_itr)->winding_count * (*rev_bnd_itr)->winding_delta < 0) {
            // prev edge is 'decreasing' WindCount (WC) toward zero
            // so we're outside the previous polygon ...
            if (std::abs(static_cast<int>((*rev_bnd_itr)->winding_count)) > 1) {
                // outside prev poly but still inside another.
                // when reversing direction of prev poly use the same WC
                if ((*rev_bnd_itr)->winding_delta * (*bnd_itr)->winding_delta < 0) {
                    (*bnd_itr)->winding_count = (*rev_bnd_itr)->winding_count;
                } else {
                    // otherwise continue to 'decrease' WC ...
                    (*bnd_itr)->winding_count =
                        (*rev_bnd_itr)->winding_count + (*bnd_itr)->winding_delta;
                }
            } else {
                // now outside all polys of same polytype so set own WC ...
                (*bnd_itr)->winding_count = (*bnd_itr)->winding_delta;
            }
        } else {
            // prev edge is 'increasing' WindCount (WC) away from zero
            // so we're inside the previous polygon ...
            if ((*rev_bnd_itr)->winding_delta * (*bnd_itr)->winding_delta < 0) {
                // if wind direction is reversing prev then use same WC
                (*bnd_itr)->winding_count = (*rev_bnd_itr)->winding_count;
            } else {
                // otherwise add to WC ...
                (*bnd_itr)->winding_count =
                    (*rev_bnd_itr)->winding_count + (*bnd_itr)->winding_delta;
            }
        }
        (*bnd_itr)->winding_count2 = (*rev_bnd_itr)->winding_count2;
    }

    // update winding_count2 ...
    auto bnd_itr_forward = rev_bnd_itr.base();
    if (is_even_odd_alt_fill_type(*(*bnd_itr), subject_fill_type, clip_fill_type)) {
        // EvenOdd filling ...
        while (bnd_itr_forward != bnd_itr) {
            (*bnd_itr)->winding_count2 = ((*bnd_itr)->winding_count2 == 0 ? 1 : 0);
            ++bnd_itr_forward;
        }
    } else {
        // nonZero, Positive or Negative filling ...
        while (bnd_itr_forward != bnd_itr) {
            (*bnd_itr)->winding_count2 += (*bnd_itr_forward)->winding_delta;
            ++bnd_itr_forward;
        }
    }
}

template <typename T>
bool is_contributing(bound<T> const& bnd,
                     clip_type cliptype,
                     fill_type subject_fill_type,
                     fill_type clip_fill_type) {
    fill_type pft = subject_fill_type;
    fill_type pft2 = clip_fill_type;
    if (bnd.poly_type != polygon_type_subject) {
        pft = clip_fill_type;
        pft2 = subject_fill_type;
    }

    switch (pft) {
    case fill_type_even_odd:
        break;
    case fill_type_non_zero:
        if (std::abs(static_cast<int>(bnd.winding_count)) != 1) {
            return false;
        }
        break;
    case fill_type_positive:
        if (bnd.winding_count != 1) {
            return false;
        }
        break;
    case fill_type_negative:
    default:
        if (bnd.winding_count != -1) {
            return false;
        }
    }

    switch (cliptype) {
    case clip_type_intersection:
        switch (pft2) {
        case fill_type_even_odd:
        case fill_type_non_zero:
            return (bnd.winding_count2 != 0);
        case fill_type_positive:
            return (bnd.winding_count2 > 0);
        case fill_type_negative:
        default:
            return (bnd.winding_count2 < 0);
        }
        break;
    case clip_type_union:
        switch (pft2) {
        case fill_type_even_odd:
        case fill_type_non_zero:
            return (bnd.winding_count2 == 0);
        case fill_type_positive:
            return (bnd.winding_count2 <= 0);
        case fill_type_negative:
        default:
            return (bnd.winding_count2 >= 0);
        }
        break;
    case clip_type_difference:
        if (bnd.poly_type == polygon_type_subject) {
            switch (pft2) {
            case fill_type_even_odd:
            case fill_type_non_zero:
                return (bnd.winding_count2 == 0);
            case fill_type_positive:
                return (bnd.winding_count2 <= 0);
            case fill_type_negative:
            default:
                return (bnd.winding_count2 >= 0);
            }
        } else {
            switch (pft2) {
            case fill_type_even_odd:
            case fill_type_non_zero:
                return (bnd.winding_count2 != 0);
            case fill_type_positive:
                return (bnd.winding_count2 > 0);
            case fill_type_negative:
            default:
                return (bnd.winding_count2 < 0);
            }
        }
        break;
    case clip_type_x_or:
        return true;
        break;
    default:
        return true;
    }
}

template <typename T>
void insert_lm_left_and_right_bound(bound<T>& left_bound,
                                    bound<T>& right_bound,
                                    active_bound_list<T>& active_bounds,
                                    ring_manager<T>& rings,
                                    scanbeam_list<T>& scanbeam,
                                    clip_type cliptype,
                                    fill_type subject_fill_type,
                                    fill_type clip_fill_type) {

    // Both left and right bound
    auto lb_abl_itr = insert_bound_into_ABL(left_bound, right_bound, active_bounds);
    auto rb_abl_itr = std::next(lb_abl_itr);
    set_winding_count(lb_abl_itr, active_bounds, subject_fill_type, clip_fill_type);
    (*rb_abl_itr)->winding_count = (*lb_abl_itr)->winding_count;
    (*rb_abl_itr)->winding_count2 = (*lb_abl_itr)->winding_count2;
    if (is_contributing(left_bound, cliptype, subject_fill_type, clip_fill_type)) {
        add_local_minimum_point(*(*lb_abl_itr), *(*rb_abl_itr), active_bounds,
                                (*lb_abl_itr)->current_edge->bot, rings);
    }

    // Add top of edges to scanbeam
    scanbeam.push_back((*lb_abl_itr)->current_edge->top.y);

    if (!current_edge_is_horizontal<T>(rb_abl_itr)) {
        scanbeam.push_back((*rb_abl_itr)->current_edge->top.y);
    }
}

template <typename T>
void insert_local_minima_into_ABL(T const bot_y,
                                  local_minimum_ptr_list<T> const& minima_sorted,
                                  local_minimum_ptr_list_itr<T>& current_lm,
                                  active_bound_list<T>& active_bounds,
                                  ring_manager<T>& rings,
                                  scanbeam_list<T>& scanbeam,
                                  clip_type cliptype,
                                  fill_type subject_fill_type,
                                  fill_type clip_fill_type) {
    while (current_lm != minima_sorted.end() && bot_y == (*current_lm)->y) {
        initialize_lm<T>(current_lm);
        auto& left_bound = (*current_lm)->left_bound;
        auto& right_bound = (*current_lm)->right_bound;
        insert_lm_left_and_right_bound(left_bound, right_bound, active_bounds, rings, scanbeam,
                                       cliptype, subject_fill_type, clip_fill_type);
        ++current_lm;
    }
}

template <typename T>
void insert_horizontal_local_minima_into_ABL(T const top_y,
                                             local_minimum_ptr_list<T> const& minima_sorted,
                                             local_minimum_ptr_list_itr<T>& current_lm,
                                             active_bound_list<T>& active_bounds,
                                             ring_manager<T>& rings,
                                             scanbeam_list<T>& scanbeam,
                                             clip_type cliptype,
                                             fill_type subject_fill_type,
                                             fill_type clip_fill_type) {
    while (current_lm != minima_sorted.end() && top_y == (*current_lm)->y &&
           (*current_lm)->minimum_has_horizontal) {
        initialize_lm<T>(current_lm);
        auto& left_bound = (*current_lm)->left_bound;
        auto& right_bound = (*current_lm)->right_bound;
        insert_lm_left_and_right_bound(left_bound, right_bound, active_bounds, rings, scanbeam,
                                       cliptype, subject_fill_type, clip_fill_type);
        ++current_lm;
    }
}
}
}
}
