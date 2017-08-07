#pragma once

#include <mapbox/geometry/wagyu/edge.hpp>
#include <mapbox/geometry/wagyu/local_minimum.hpp>

#include <algorithm>

#ifdef DEBUG
#include <stdexcept>
#endif

namespace mapbox {
namespace geometry {
namespace wagyu {

template <typename T>
inline void reverse_horizontal(edge<T>& e) {
    // swap horizontal edges' top and bottom x's so they follow the natural
    // progression of the bounds - ie so their xbots will align with the
    // adjoining lower edge. [Helpful in the process_horizontal() method.]
    std::swap(e.top.x, e.bot.x);
}

// Make a list start on a local maximum by
// shifting all the points not on a local maximum to the
template <typename T>
void start_list_on_local_maximum(edge_list<T>& edges) {
    if (edges.size() <= 2) {
        return;
    }
    // Find the first local maximum going forward in the list
    auto prev_edge = edges.end();
    --prev_edge;
    bool prev_edge_is_horizontal = is_horizontal(*prev_edge);
    auto edge = edges.begin();
    bool edge_is_horizontal;
    bool y_decreasing_before_last_horizontal = false; // assume false at start

    while (edge != edges.end()) {
        edge_is_horizontal = is_horizontal(*edge);
        if ((!prev_edge_is_horizontal && !edge_is_horizontal && edge->top == prev_edge->top)) {
            break;
        }
        if (!edge_is_horizontal && prev_edge_is_horizontal) {
            if (y_decreasing_before_last_horizontal &&
                (edge->top == prev_edge->bot || edge->top == prev_edge->top)) {
                break;
            }
        } else if (!y_decreasing_before_last_horizontal && !prev_edge_is_horizontal &&
                   edge_is_horizontal &&
                   (prev_edge->top == edge->top || prev_edge->top == edge->bot)) {
            y_decreasing_before_last_horizontal = true;
        }
        prev_edge_is_horizontal = edge_is_horizontal;
        prev_edge = edge;
        ++edge;
    }
    std::rotate(edges.begin(), edge, edges.end());
}

template <typename T>
bound<T> create_bound_towards_minimum(edge_list<T>& edges) {
    if (edges.size() == 1) {
        if (is_horizontal(edges.front())) {
            reverse_horizontal(edges.front());
        }
        bound<T> bnd;
        std::swap(bnd.edges, edges);
        return bnd;
    }
    auto next_edge = edges.begin();
    auto edge = next_edge;
    ++next_edge;
    bool edge_is_horizontal = is_horizontal(*edge);
    if (edge_is_horizontal) {
        reverse_horizontal(*edge);
    }
    bool next_edge_is_horizontal;
    bool y_increasing_before_last_horizontal = false; // assume false at start

    while (next_edge != edges.end()) {
        next_edge_is_horizontal = is_horizontal(*next_edge);
        if ((!next_edge_is_horizontal && !edge_is_horizontal && edge->bot == next_edge->bot)) {
            break;
        }
        if (!next_edge_is_horizontal && edge_is_horizontal) {
            if (y_increasing_before_last_horizontal &&
                (next_edge->bot == edge->bot || next_edge->bot == edge->top)) {
                break;
            }
        } else if (!y_increasing_before_last_horizontal && !edge_is_horizontal &&
                   next_edge_is_horizontal &&
                   (edge->bot == next_edge->top || edge->bot == next_edge->bot)) {
            y_increasing_before_last_horizontal = true;
        }
        edge_is_horizontal = next_edge_is_horizontal;
        edge = next_edge;
        if (edge_is_horizontal) {
            reverse_horizontal(*edge);
        }
        ++next_edge;
    }
    bound<T> bnd;
    if (next_edge == edges.end()) {
        std::swap(edges, bnd.edges);
    } else {
        bnd.edges.reserve(static_cast<std::size_t>(std::distance(edges.begin(), next_edge)));
        std::move(edges.begin(), next_edge, std::back_inserter(bnd.edges));
        edges.erase(edges.begin(), next_edge);
    }
    std::reverse(bnd.edges.begin(), bnd.edges.end());
    return bnd;
}

template <typename T>
bound<T> create_bound_towards_maximum(edge_list<T>& edges) {
    if (edges.size() == 1) {
        bound<T> bnd;
        std::swap(bnd.edges, edges);
        return bnd;
    }
    auto next_edge = edges.begin();
    auto edge = next_edge;
    ++next_edge;
    bool edge_is_horizontal = is_horizontal(*edge);
    bool next_edge_is_horizontal;
    bool y_decreasing_before_last_horizontal = false; // assume false at start

    while (next_edge != edges.end()) {
        next_edge_is_horizontal = is_horizontal(*next_edge);
        if ((!next_edge_is_horizontal && !edge_is_horizontal && edge->top == next_edge->top)) {
            break;
        }
        if (!next_edge_is_horizontal && edge_is_horizontal) {
            if (y_decreasing_before_last_horizontal &&
                (next_edge->top == edge->bot || next_edge->top == edge->top)) {
                break;
            }
        } else if (!y_decreasing_before_last_horizontal && !edge_is_horizontal &&
                   next_edge_is_horizontal &&
                   (edge->top == next_edge->top || edge->top == next_edge->bot)) {
            y_decreasing_before_last_horizontal = true;
        }
        edge_is_horizontal = next_edge_is_horizontal;
        edge = next_edge;
        ++next_edge;
    }
    bound<T> bnd;
    if (next_edge == edges.end()) {
        std::swap(bnd.edges, edges);
    } else {
        bnd.edges.reserve(static_cast<std::size_t>(std::distance(edges.begin(), next_edge)));
        std::move(edges.begin(), next_edge, std::back_inserter(bnd.edges));
        edges.erase(edges.begin(), next_edge);
    }
    return bnd;
}

template <typename T>
void fix_horizontals(bound<T>& bnd) {

    auto edge_itr = bnd.edges.begin();
    auto next_itr = std::next(edge_itr);
    if (next_itr == bnd.edges.end()) {
        return;
    }
    if (is_horizontal(*edge_itr) && next_itr->bot != edge_itr->top) {
        reverse_horizontal(*edge_itr);
    }
    auto prev_itr = edge_itr++;
    while (edge_itr != bnd.edges.end()) {
        if (is_horizontal(*edge_itr) && prev_itr->top != edge_itr->bot) {
            reverse_horizontal(*edge_itr);
        }
        prev_itr = edge_itr;
        ++edge_itr;
    }
}

template <typename T>
void move_horizontals_on_left_to_right(bound<T>& left_bound, bound<T>& right_bound) {
    // We want all the horizontal segments that are at the same Y as the minimum to be on the right
    // bound
    auto edge_itr = left_bound.edges.begin();
    while (edge_itr != left_bound.edges.end()) {
        if (!is_horizontal(*edge_itr)) {
            break;
        }
        reverse_horizontal(*edge_itr);
        ++edge_itr;
    }
    if (edge_itr == left_bound.edges.begin()) {
        return;
    }
    std::reverse(left_bound.edges.begin(), edge_itr);
    auto dist = std::distance(left_bound.edges.begin(), edge_itr);
    std::move(left_bound.edges.begin(), edge_itr, std::back_inserter(right_bound.edges));
    left_bound.edges.erase(left_bound.edges.begin(), edge_itr);
    std::rotate(right_bound.edges.begin(), std::prev(right_bound.edges.end(), dist),
                right_bound.edges.end());
}

template <typename T>
void add_ring_to_local_minima_list(edge_list<T>& edges,
                                   local_minimum_list<T>& minima_list,
                                   polygon_type poly_type) {

    if (edges.empty()) {
        return;
    }
    // Adjust the order of the ring so we start on a local maximum
    // therefore we start right away on a bound.
    start_list_on_local_maximum(edges);

    bound_ptr<T> first_minimum = nullptr;
    bound_ptr<T> last_maximum = nullptr;
    while (!edges.empty()) {
        bool lm_minimum_has_horizontal = false;
        auto to_minimum = create_bound_towards_minimum(edges);
        if (edges.empty()) {
            throw std::runtime_error("Edges is empty after only creating a single bound.");
        }
        auto to_maximum = create_bound_towards_maximum(edges);
        fix_horizontals(to_minimum);
        fix_horizontals(to_maximum);
        auto to_max_first_non_horizontal = to_maximum.edges.begin();
        auto to_min_first_non_horizontal = to_minimum.edges.begin();
        bool minimum_is_left = true;
        while (to_max_first_non_horizontal != to_maximum.edges.end() &&
               is_horizontal(*to_max_first_non_horizontal)) {
            lm_minimum_has_horizontal = true;
            ++to_max_first_non_horizontal;
        }
        while (to_min_first_non_horizontal != to_minimum.edges.end() &&
               is_horizontal(*to_min_first_non_horizontal)) {
            lm_minimum_has_horizontal = true;
            ++to_min_first_non_horizontal;
        }

        if (to_max_first_non_horizontal == to_maximum.edges.end() ||
            to_min_first_non_horizontal == to_minimum.edges.end()) {
            throw std::runtime_error("should not have a horizontal only bound for a ring");
        }

        if (lm_minimum_has_horizontal) {
            if (to_max_first_non_horizontal->bot.x > to_min_first_non_horizontal->bot.x) {
                minimum_is_left = true;
                move_horizontals_on_left_to_right(to_minimum, to_maximum);
            } else {
                minimum_is_left = false;
                move_horizontals_on_left_to_right(to_maximum, to_minimum);
            }
        } else {
            if (to_max_first_non_horizontal->dx > to_min_first_non_horizontal->dx) {
                minimum_is_left = false;
            } else {
                minimum_is_left = true;
            }
        }
        assert(!to_minimum.edges.empty());
        assert(!to_maximum.edges.empty());
        auto const& min_front = to_minimum.edges.front();
        if (last_maximum) {
            to_minimum.maximum_bound = last_maximum;
        }
        to_minimum.poly_type = poly_type;
        to_maximum.poly_type = poly_type;
        if (!minimum_is_left) {
            to_minimum.side = edge_right;
            to_maximum.side = edge_left;
            to_minimum.winding_delta = -1;
            to_maximum.winding_delta = 1;
            minima_list.emplace_back(std::move(to_maximum), std::move(to_minimum), min_front.bot.y,
                                     lm_minimum_has_horizontal);
            if (!last_maximum) {
                first_minimum = &(minima_list.back().right_bound);
            } else {
                last_maximum->maximum_bound = &(minima_list.back().right_bound);
            }
            last_maximum = &(minima_list.back().left_bound);
        } else {
            to_minimum.side = edge_left;
            to_maximum.side = edge_right;
            to_minimum.winding_delta = -1;
            to_maximum.winding_delta = 1;
            minima_list.emplace_back(std::move(to_minimum), std::move(to_maximum), min_front.bot.y,
                                     lm_minimum_has_horizontal);
            if (!last_maximum) {
                first_minimum = &(minima_list.back().left_bound);
            } else {
                last_maximum->maximum_bound = &(minima_list.back().left_bound);
            }
            last_maximum = &(minima_list.back().right_bound);
        }
    }
    last_maximum->maximum_bound = first_minimum;
    first_minimum->maximum_bound = last_maximum;
}

template <typename T>
void initialize_lm(local_minimum_ptr_list_itr<T>& lm) {
    if (!(*lm)->left_bound.edges.empty()) {
        (*lm)->left_bound.current_edge = (*lm)->left_bound.edges.begin();
        (*lm)->left_bound.next_edge = std::next((*lm)->left_bound.current_edge);
        (*lm)->left_bound.current_x = static_cast<double>((*lm)->left_bound.current_edge->bot.x);
        (*lm)->left_bound.winding_count = 0;
        (*lm)->left_bound.winding_count2 = 0;
        (*lm)->left_bound.side = edge_left;
        (*lm)->left_bound.ring = nullptr;
    }
    if (!(*lm)->right_bound.edges.empty()) {
        (*lm)->right_bound.current_edge = (*lm)->right_bound.edges.begin();
        (*lm)->right_bound.next_edge = std::next((*lm)->right_bound.current_edge);
        (*lm)->right_bound.current_x = static_cast<double>((*lm)->right_bound.current_edge->bot.x);
        (*lm)->right_bound.winding_count = 0;
        (*lm)->right_bound.winding_count2 = 0;
        (*lm)->right_bound.side = edge_right;
        (*lm)->right_bound.ring = nullptr;
    }
}
}
}
}
