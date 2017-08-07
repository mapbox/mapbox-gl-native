#pragma once

#include <mapbox/geometry/wagyu/active_bound_list.hpp>
#include <mapbox/geometry/wagyu/bound.hpp>
#include <mapbox/geometry/wagyu/bubble_sort.hpp>
#include <mapbox/geometry/wagyu/config.hpp>
#include <mapbox/geometry/wagyu/edge.hpp>
#include <mapbox/geometry/wagyu/intersect.hpp>
#include <mapbox/geometry/wagyu/intersect_util.hpp>
#include <mapbox/geometry/wagyu/ring.hpp>
#include <mapbox/geometry/wagyu/ring_util.hpp>
#include <mapbox/geometry/wagyu/util.hpp>

namespace mapbox {
namespace geometry {
namespace wagyu {

template <typename T>
struct hp_intersection_swap {

    ring_manager<T>& manager;

    hp_intersection_swap(ring_manager<T>& m) : manager(m) {
    }

    void operator()(bound_ptr<T> const& b1, bound_ptr<T> const& b2) {
        mapbox::geometry::point<double> pt;
        if (!get_edge_intersection<T, double>(*(b1->current_edge), *(b2->current_edge), pt)) {
            // LCOV_EXCL_START
            throw std::runtime_error("Trying to find intersection of lines that do not intersect");
            // LCOV_EXCL_END
        }
        add_to_hot_pixels(round_point<T>(pt), manager);
    }
};

template <typename T>
void process_hot_pixel_intersections(T top_y,
                                     active_bound_list<T>& active_bounds,
                                     ring_manager<T>& manager) {
    if (active_bounds.empty()) {
        return;
    }
    update_current_x(active_bounds, top_y);
    bubble_sort(active_bounds.begin(), active_bounds.end(), intersection_compare<T>(),
                hp_intersection_swap<T>(manager));
}

template <typename T>
bool horizontals_at_top_scanbeam(T top_y,
                                 active_bound_list_itr<T>& bnd_curr,
                                 active_bound_list<T>& active_bounds,
                                 ring_manager<T>& manager) {
    bool shifted = false;
    auto& current_edge = (*bnd_curr)->current_edge;
    (*bnd_curr)->current_x = static_cast<double>(current_edge->top.x);
    if (current_edge->bot.x < current_edge->top.x) {
        // left to right
        auto bnd_next = std::next(bnd_curr);
        while (bnd_next != active_bounds.end() &&
               (*bnd_next == nullptr || (*bnd_next)->current_x < (*bnd_curr)->current_x)) {
            if (*bnd_next != nullptr && (*bnd_next)->current_edge->top.y != top_y &&
                (*bnd_next)->current_edge->bot.y != top_y) {
                mapbox::geometry::point<T> pt(wround<T>((*bnd_next)->current_x), top_y);
                add_to_hot_pixels(pt, manager);
            }
            std::iter_swap(bnd_curr, bnd_next);
            ++bnd_curr;
            ++bnd_next;
            shifted = true;
        }
    } else {
        // right to left
        if (bnd_curr != active_bounds.begin()) {
            auto bnd_prev = std::prev(bnd_curr);
            while (bnd_curr != active_bounds.begin() &&
                   (*bnd_prev == nullptr || (*bnd_prev)->current_x > (*bnd_curr)->current_x)) {
                if (*bnd_prev != nullptr && (*bnd_prev)->current_edge->top.y != top_y &&
                    (*bnd_prev)->current_edge->bot.y != top_y) {
                    mapbox::geometry::point<T> pt(wround<T>((*bnd_prev)->current_x), top_y);
                    add_to_hot_pixels(pt, manager);
                }
                std::iter_swap(bnd_curr, bnd_prev);
                --bnd_curr;
                if (bnd_curr != active_bounds.begin()) {
                    --bnd_prev;
                }
            }
        }
    }
    return shifted;
}

template <typename T>
void process_hot_pixel_edges_at_top_of_scanbeam(T top_y,
                                                scanbeam_list<T>& scanbeam,
                                                active_bound_list<T>& active_bounds,
                                                ring_manager<T>& manager) {
    for (auto bnd = active_bounds.begin(); bnd != active_bounds.end();) {
        if (*bnd == nullptr) {
            ++bnd;
            continue;
        }
        bound<T>& current_bound = *(*bnd);
        auto bnd_curr = bnd;
        bool shifted = false;
        auto& current_edge = current_bound.current_edge;
        while (current_edge != current_bound.edges.end() && current_edge->top.y == top_y) {
            add_to_hot_pixels(current_edge->top, manager);
            if (is_horizontal(*current_edge)) {
                if (horizontals_at_top_scanbeam(top_y, bnd_curr, active_bounds, manager)) {
                    shifted = true;
                }
            }
            next_edge_in_bound(current_bound, scanbeam);
        }
        if (current_edge == current_bound.edges.end()) {
            *bnd_curr = nullptr;
        }
        if (!shifted) {
            ++bnd;
        }
    }
    active_bounds.erase(std::remove(active_bounds.begin(), active_bounds.end(), nullptr),
                        active_bounds.end());
}

template <typename T>
void insert_local_minima_into_ABL_hot_pixel(T top_y,
                                            local_minimum_ptr_list<T>& minima_sorted,
                                            local_minimum_ptr_list_itr<T>& lm,
                                            active_bound_list<T>& active_bounds,
                                            ring_manager<T>& manager,
                                            scanbeam_list<T>& scanbeam) {
    while (lm != minima_sorted.end() && (*lm)->y == top_y) {
        add_to_hot_pixels((*lm)->left_bound.edges.front().bot, manager);
        auto& left_bound = (*lm)->left_bound;
        auto& right_bound = (*lm)->right_bound;
        left_bound.current_edge = left_bound.edges.begin();
        left_bound.next_edge = std::next(left_bound.current_edge);
        left_bound.current_x = static_cast<double>(left_bound.current_edge->bot.x);
        right_bound.current_edge = right_bound.edges.begin();
        right_bound.next_edge = std::next(right_bound.current_edge);
        right_bound.current_x = static_cast<double>(right_bound.current_edge->bot.x);
        auto lb_abl_itr = insert_bound_into_ABL(left_bound, right_bound, active_bounds);
        if (!current_edge_is_horizontal<T>(lb_abl_itr)) {
            scanbeam.push_back((*lb_abl_itr)->current_edge->top.y);
        }
        auto rb_abl_itr = std::next(lb_abl_itr);
        if (!current_edge_is_horizontal<T>(rb_abl_itr)) {
            scanbeam.push_back((*rb_abl_itr)->current_edge->top.y);
        }
        ++lm;
    }
}

template <typename T>
void build_hot_pixels(local_minimum_list<T>& minima_list, ring_manager<T>& manager) {
    active_bound_list<T> active_bounds;
    scanbeam_list<T> scanbeam;
    T scanline_y = std::numeric_limits<T>::max();

    local_minimum_ptr_list<T> minima_sorted;
    minima_sorted.reserve(minima_list.size());
    for (auto& lm : minima_list) {
        minima_sorted.push_back(&lm);
    }
    std::stable_sort(minima_sorted.begin(), minima_sorted.end(), local_minimum_sorter<T>());
    local_minimum_ptr_list_itr<T> current_lm = minima_sorted.begin();

    setup_scanbeam(minima_list, scanbeam);

    // Estimate size for reserving hot pixels
    std::size_t reserve = 0;
    for (auto& lm : minima_list) {
        reserve += lm.left_bound.edges.size() + 2;
        reserve += lm.right_bound.edges.size() + 2;
    }
    manager.hot_pixels.reserve(reserve);

    while (pop_from_scanbeam(scanline_y, scanbeam) || current_lm != minima_sorted.end()) {

        process_hot_pixel_intersections(scanline_y, active_bounds, manager);

        insert_local_minima_into_ABL_hot_pixel(scanline_y, minima_sorted, current_lm, active_bounds,
                                               manager, scanbeam);

        process_hot_pixel_edges_at_top_of_scanbeam(scanline_y, scanbeam, active_bounds, manager);
    }
    preallocate_point_memory(manager, manager.hot_pixels.size());
    sort_hot_pixels(manager);
}
}
}
}
