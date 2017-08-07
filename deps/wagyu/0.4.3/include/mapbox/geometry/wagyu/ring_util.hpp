#pragma once

#ifdef DEBUG
#include <iostream>
// Example debug print for backtrace - only works on IOS
#include <execinfo.h>
#include <stdio.h>
//
// void* callstack[128];
// int i, frames = backtrace(callstack, 128);
// char** strs = backtrace_symbols(callstack, frames);
// for (i = 0; i < frames; ++i) {
//     printf("%s\n", strs[i]);
// }
// free(strs);
#endif

#include <algorithm>

#include <mapbox/geometry/wagyu/active_bound_list.hpp>
#include <mapbox/geometry/wagyu/config.hpp>
#include <mapbox/geometry/wagyu/edge.hpp>
#include <mapbox/geometry/wagyu/ring.hpp>
#include <mapbox/geometry/wagyu/util.hpp>

namespace mapbox {
namespace geometry {
namespace wagyu {

template <typename T>
void set_hole_state(bound<T>& bnd,
                    active_bound_list<T> const& active_bounds,
                    ring_manager<T>& rings) {
    auto bnd_itr = std::find(active_bounds.rbegin(), active_bounds.rend(), &bnd);
    ++bnd_itr;
    bound_ptr<T> bndTmp = nullptr;
    // Find first non line ring to the left of current bound.
    while (bnd_itr != active_bounds.rend()) {
        if (*bnd_itr == nullptr) {
            ++bnd_itr;
            continue;
        }
        if ((*bnd_itr)->ring) {
            if (!bndTmp) {
                bndTmp = (*bnd_itr);
            } else if (bndTmp->ring == (*bnd_itr)->ring) {
                bndTmp = nullptr;
            }
        }
        ++bnd_itr;
    }
    if (!bndTmp) {
        bnd.ring->parent = nullptr;
        rings.children.push_back(bnd.ring);
    } else {
        bnd.ring->parent = bndTmp->ring;
        bndTmp->ring->children.push_back(bnd.ring);
    }
}

template <typename T>
void update_current_hp_itr(T scanline_y, ring_manager<T>& rings) {
    while (rings.current_hp_itr->y > scanline_y) {
        ++rings.current_hp_itr;
    }
}

template <typename T>
struct hot_pixel_sorter {
    inline bool operator()(mapbox::geometry::point<T> const& pt1,
                           mapbox::geometry::point<T> const& pt2) {
        if (pt1.y == pt2.y) {
            return pt1.x < pt2.x;
        } else {
            return pt1.y > pt2.y;
        }
    }
};

// Due to the nature of floating point calculations
// and the high likely hood of values around X.5, we
// need to fudge what is X.5 some for our rounding.
const double rounding_offset = 1e-12;
const double rounding_offset_y = 5e-13;

template <typename T>
T round_towards_min(double val) {
    // 0.5 rounds to 0
    // 0.0 rounds to 0
    // -0.5 rounds to -1
    return static_cast<T>(std::ceil(val - 0.5 + rounding_offset));
}

template <typename T>
T round_towards_max(double val) {
    // 0.5 rounds to 1
    // 0.0 rounds to 0
    // -0.5 rounds to 0
    return static_cast<T>(std::floor(val + 0.5 + rounding_offset));
}

template <typename T>
inline T get_edge_min_x(edge<T> const& edge, const T current_y) {
    if (is_horizontal(edge)) {
        if (edge.bot.x < edge.top.x) {
            return edge.bot.x;
        } else {
            return edge.top.x;
        }
    } else if (edge.dx > 0.0) {
        if (current_y == edge.top.y) {
            return edge.top.x;
        } else {
            double lower_range_y = static_cast<double>(current_y - edge.bot.y) - 0.5;
            double return_val = static_cast<double>(edge.bot.x) + edge.dx * lower_range_y;
            T value = round_towards_min<T>(return_val);
            return value;
        }
    } else {
        if (current_y == edge.bot.y) {
            return edge.bot.x;
        } else {
            double return_val =
                static_cast<double>(edge.bot.x) +
                edge.dx * (static_cast<double>(current_y - edge.bot.y) + 0.5 - rounding_offset_y);
            T value = round_towards_min<T>(return_val);
            return value;
        }
    }
}

template <typename T>
inline T get_edge_max_x(edge<T> const& edge, const T current_y) {
    if (is_horizontal(edge)) {
        if (edge.bot.x > edge.top.x) {
            return edge.bot.x;
        } else {
            return edge.top.x;
        }
    } else if (edge.dx < 0.0) {
        if (current_y == edge.top.y) {
            return edge.top.x;
        } else {
            double lower_range_y = static_cast<double>(current_y - edge.bot.y) - 0.5;
            double return_val = static_cast<double>(edge.bot.x) + edge.dx * lower_range_y;
            T value = round_towards_max<T>(return_val);
            return value;
        }
    } else {
        if (current_y == edge.bot.y) {
            return edge.bot.x;
        } else {
            double return_val =
                static_cast<double>(edge.bot.x) +
                edge.dx * (static_cast<double>(current_y - edge.bot.y) + 0.5 - rounding_offset_y);
            T value = round_towards_max<T>(return_val);
            return value;
        }
    }
}

template <typename T>
void hot_pixel_set_left_to_right(T y,
                                 T start_x,
                                 T end_x,
                                 bound<T>& bnd,
                                 ring_manager<T>& rings,
                                 hot_pixel_itr<T>& itr,
                                 hot_pixel_itr<T>& end,
                                 bool add_end_point) {
    T x_min = get_edge_min_x(*(bnd.current_edge), y);
    x_min = std::max(x_min, start_x);
    T x_max = get_edge_max_x(*(bnd.current_edge), y);
    x_max = std::min(x_max, end_x);
    for (; itr != end; ++itr) {
        if (itr->x < x_min) {
            continue;
        }
        if (itr->x > x_max) {
            break;
        }
        if (!add_end_point && itr->x == end_x) {
            continue;
        }
        point_ptr<T> op = bnd.ring->points;
        bool to_front = (bnd.side == edge_left);
        if (to_front && (*itr == *op)) {
            continue;
        } else if (!to_front && (*itr == *op->prev)) {
            continue;
        }
        point_ptr<T> new_point = create_new_point(bnd.ring, *itr, op, rings);
        if (to_front) {
            bnd.ring->points = new_point;
        }
    }
}

template <typename T>
void hot_pixel_set_right_to_left(T y,
                                 T start_x,
                                 T end_x,
                                 bound<T>& bnd,
                                 ring_manager<T>& rings,
                                 hot_pixel_rev_itr<T>& itr,
                                 hot_pixel_rev_itr<T>& end,
                                 bool add_end_point) {
    T x_min = get_edge_min_x(*(bnd.current_edge), y);
    x_min = std::max(x_min, end_x);
    T x_max = get_edge_max_x(*(bnd.current_edge), y);
    x_max = std::min(x_max, start_x);
    for (; itr != end; ++itr) {
        if (itr->x > x_max) {
            continue;
        }
        if (itr->x < x_min) {
            break;
        }
        if (!add_end_point && itr->x == end_x) {
            continue;
        }
        point_ptr<T> op = bnd.ring->points;
        bool to_front = (bnd.side == edge_left);
        if (to_front && (*itr == *op)) {
            continue;
        } else if (!to_front && (*itr == *op->prev)) {
            continue;
        }
        point_ptr<T> new_point = create_new_point(bnd.ring, *itr, op, rings);
        if (to_front) {
            bnd.ring->points = new_point;
        }
    }
}

template <typename T>
void sort_hot_pixels(ring_manager<T>& rings) {
    std::sort(rings.hot_pixels.begin(), rings.hot_pixels.end(), hot_pixel_sorter<T>());
    auto last = std::unique(rings.hot_pixels.begin(), rings.hot_pixels.end());
    rings.hot_pixels.erase(last, rings.hot_pixels.end());
}

template <typename T>
void insert_hot_pixels_in_path(bound<T>& bnd,
                               mapbox::geometry::point<T> const& end_pt,
                               ring_manager<T>& rings,
                               bool add_end_point) {
    if (end_pt == bnd.last_point) {
        return;
    }

    T start_y = bnd.last_point.y;
    T start_x = bnd.last_point.x;
    T end_y = end_pt.y;
    T end_x = end_pt.x;

    auto itr = rings.current_hp_itr;
    while (itr->y <= start_y && itr != rings.hot_pixels.begin()) {
        --itr;
    }
    if (start_x > end_x) {
        for (; itr != rings.hot_pixels.end();) {
            if (itr->y > start_y) {
                ++itr;
                continue;
            }
            if (itr->y < end_y) {
                break;
            }
            T y = itr->y;
            auto last_itr = hot_pixel_rev_itr<T>(itr);
            while (itr != rings.hot_pixels.end() && itr->y == y) {
                ++itr;
            }
            auto first_itr = hot_pixel_rev_itr<T>(itr);
            bool add_end_point_itr = (y != end_pt.y || add_end_point);
            hot_pixel_set_right_to_left(y, start_x, end_x, bnd, rings, first_itr, last_itr,
                                        add_end_point_itr);
        }
    } else {
        for (; itr != rings.hot_pixels.end();) {
            if (itr->y > start_y) {
                ++itr;
                continue;
            }
            if (itr->y < end_y) {
                break;
            }
            T y = itr->y;
            auto first_itr = itr;
            while (itr != rings.hot_pixels.end() && itr->y == y) {
                ++itr;
            }
            auto last_itr = itr;
            bool add_end_point_itr = (y != end_pt.y || add_end_point);
            hot_pixel_set_left_to_right(y, start_x, end_x, bnd, rings, first_itr, last_itr,
                                        add_end_point_itr);
        }
    }
    bnd.last_point = end_pt;
}

template <typename T>
void add_to_hot_pixels(mapbox::geometry::point<T> const& pt, ring_manager<T>& rings) {
    rings.hot_pixels.push_back(pt);
}

template <typename T>
void add_first_point(bound<T>& bnd,
                     active_bound_list<T>& active_bounds,
                     mapbox::geometry::point<T> const& pt,
                     ring_manager<T>& rings) {

    ring_ptr<T> r = create_new_ring(rings);
    bnd.ring = r;
    r->points = create_new_point(r, pt, rings);
    set_hole_state(bnd, active_bounds, rings);
    bnd.last_point = pt;
}

template <typename T>
void add_point_to_ring(bound<T>& bnd,
                       mapbox::geometry::point<T> const& pt,
                       ring_manager<T>& rings) {
    assert(bnd.ring);
    // Handle hot pixels
    insert_hot_pixels_in_path(bnd, pt, rings, false);

    // bnd.ring->points is the 'Left-most' point & bnd.ring->points->prev is the
    // 'Right-most'
    point_ptr<T> op = bnd.ring->points;
    bool to_front = (bnd.side == edge_left);
    if (to_front && (pt == *op)) {
        return;
    } else if (!to_front && (pt == *op->prev)) {
        return;
    }
    point_ptr<T> new_point = create_new_point(bnd.ring, pt, bnd.ring->points, rings);
    if (to_front) {
        bnd.ring->points = new_point;
    }
}

template <typename T>
void add_point(bound<T>& bnd,
               active_bound_list<T>& active_bounds,
               mapbox::geometry::point<T> const& pt,
               ring_manager<T>& rings) {
    if (bnd.ring == nullptr) {
        add_first_point(bnd, active_bounds, pt, rings);
    } else {
        add_point_to_ring(bnd, pt, rings);
    }
}

template <typename T>
void add_local_minimum_point(bound<T>& b1,
                             bound<T>& b2,
                             active_bound_list<T>& active_bounds,
                             mapbox::geometry::point<T> const& pt,
                             ring_manager<T>& rings) {
    if (is_horizontal(*b2.current_edge) || (b1.current_edge->dx > b2.current_edge->dx)) {
        add_point(b1, active_bounds, pt, rings);
        b2.last_point = pt;
        b2.ring = b1.ring;
        b1.side = edge_left;
        b2.side = edge_right;
    } else {
        add_point(b2, active_bounds, pt, rings);
        b1.last_point = pt;
        b1.ring = b2.ring;
        b1.side = edge_right;
        b2.side = edge_left;
    }
}

template <typename T>
inline double get_dx(point<T> const& pt1, point<T> const& pt2) {
    if (pt1.y == pt2.y) {
        return std::numeric_limits<double>::infinity();
    } else {
        return static_cast<double>(pt2.x - pt1.x) / static_cast<double>(pt2.y - pt1.y);
    }
}

template <typename T>
bool first_is_bottom_point(const_point_ptr<T> btmPt1, const_point_ptr<T> btmPt2) {
    point_ptr<T> p = btmPt1->prev;
    while ((*p == *btmPt1) && (p != btmPt1)) {
        p = p->prev;
    }
    double dx1p = std::fabs(get_dx(*btmPt1, *p));

    p = btmPt1->next;
    while ((*p == *btmPt1) && (p != btmPt1)) {
        p = p->next;
    }
    double dx1n = std::fabs(get_dx(*btmPt1, *p));

    p = btmPt2->prev;
    while ((*p == *btmPt2) && (p != btmPt2)) {
        p = p->prev;
    }
    double dx2p = std::fabs(get_dx(*btmPt2, *p));

    p = btmPt2->next;
    while ((*p == *btmPt2) && (p != btmPt2)) {
        p = p->next;
    }
    double dx2n = std::fabs(get_dx(*btmPt2, *p));

    if (values_are_equal(std::max(dx1p, dx1n), std::max(dx2p, dx2n)) &&
        values_are_equal(std::min(dx1p, dx1n), std::min(dx2p, dx2n))) {
        std::size_t s = 0;
        mapbox::geometry::box<T> bbox({ 0, 0 }, { 0, 0 });
        return area_from_point(btmPt1, s, bbox) > 0.0; // if otherwise identical use orientation
    } else {
        return (greater_than_or_equal(dx1p, dx2p) && greater_than_or_equal(dx1p, dx2n)) ||
               (greater_than_or_equal(dx1n, dx2p) && greater_than_or_equal(dx1n, dx2n));
    }
}

template <typename T>
point_ptr<T> get_bottom_point(point_ptr<T> pp) {
    point_ptr<T> dups = nullptr;
    point_ptr<T> p = pp->next;
    while (p != pp) {
        if (p->y > pp->y) {
            pp = p;
            dups = nullptr;
        } else if (p->y == pp->y && p->x <= pp->x) {
            if (p->x < pp->x) {
                dups = nullptr;
                pp = p;
            } else {
                if (p->next != pp && p->prev != pp) {
                    dups = p;
                }
            }
        }
        p = p->next;
    }
    if (dups) {
        // there appears to be at least 2 vertices at bottom_point so ...
        while (dups != p) {
            if (!first_is_bottom_point(p, dups)) {
                pp = dups;
            }
            dups = dups->next;
            while (*dups != *pp) {
                dups = dups->next;
            }
        }
    }
    return pp;
}

template <typename T>
ring_ptr<T> get_lower_most_ring(ring_ptr<T> outRec1, ring_ptr<T> outRec2) {
    // work out which polygon fragment has the correct hole state ...
    if (!outRec1->bottom_point) {
        outRec1->bottom_point = get_bottom_point(outRec1->points);
    }
    if (!outRec2->bottom_point) {
        outRec2->bottom_point = get_bottom_point(outRec2->points);
    }
    point_ptr<T> OutPt1 = outRec1->bottom_point;
    point_ptr<T> OutPt2 = outRec2->bottom_point;
    if (OutPt1->y > OutPt2->y) {
        return outRec1;
    } else if (OutPt1->y < OutPt2->y) {
        return outRec2;
    } else if (OutPt1->x < OutPt2->x) {
        return outRec1;
    } else if (OutPt1->x > OutPt2->x) {
        return outRec2;
    } else if (OutPt1->next == OutPt1) {
        return outRec2;
    } else if (OutPt2->next == OutPt2) {
        return outRec1;
    } else if (first_is_bottom_point(OutPt1, OutPt2)) {
        return outRec1;
    } else {
        return outRec2;
    }
}

template <typename T>
bool ring1_child_below_ring2(ring_ptr<T> ring1, ring_ptr<T> ring2) {
    do {
        ring1 = ring1->parent;
        if (ring1 == ring2) {
            return true;
        }
    } while (ring1);
    return false;
}

template <typename T>
void update_points_ring(ring_ptr<T> ring) {
    point_ptr<T> op = ring->points;
    do {
        op->ring = ring;
        op = op->prev;
    } while (op != ring->points);
}

template <typename T>
void append_ring(bound<T>& b1,
                 bound<T>& b2,
                 active_bound_list<T>& active_bounds,
                 ring_manager<T>& manager) {
    // get the start and ends of both output polygons ...
    ring_ptr<T> outRec1 = b1.ring;
    ring_ptr<T> outRec2 = b2.ring;

    ring_ptr<T> keep_ring;
    bound_ptr<T> keep_bound;
    ring_ptr<T> remove_ring;
    bound_ptr<T> remove_bound;
    if (ring1_child_below_ring2(outRec1, outRec2)) {
        keep_ring = outRec2;
        keep_bound = &b2;
        remove_ring = outRec1;
        remove_bound = &b1;
    } else if (ring1_child_below_ring2(outRec2, outRec1)) {
        keep_ring = outRec1;
        keep_bound = &b1;
        remove_ring = outRec2;
        remove_bound = &b2;
    } else if (outRec1 == get_lower_most_ring(outRec1, outRec2)) {
        keep_ring = outRec1;
        keep_bound = &b1;
        remove_ring = outRec2;
        remove_bound = &b2;
    } else {
        keep_ring = outRec2;
        keep_bound = &b2;
        remove_ring = outRec1;
        remove_bound = &b1;
    }

    // get the start and ends of both output polygons and
    // join b2 poly onto b1 poly and delete pointers to b2 ...

    point_ptr<T> p1_lft = keep_ring->points;
    point_ptr<T> p1_rt = p1_lft->prev;
    point_ptr<T> p2_lft = remove_ring->points;
    point_ptr<T> p2_rt = p2_lft->prev;

    // join b2 poly onto b1 poly and delete pointers to b2 ...
    if (keep_bound->side == edge_left) {
        if (remove_bound->side == edge_left) {
            // z y x a b c
            reverse_ring(p2_lft);
            p2_lft->next = p1_lft;
            p1_lft->prev = p2_lft;
            p1_rt->next = p2_rt;
            p2_rt->prev = p1_rt;
            keep_ring->points = p2_rt;
        } else {
            // x y z a b c
            p2_rt->next = p1_lft;
            p1_lft->prev = p2_rt;
            p2_lft->prev = p1_rt;
            p1_rt->next = p2_lft;
            keep_ring->points = p2_lft;
        }
    } else {
        if (remove_bound->side == edge_right) {
            // a b c z y x
            reverse_ring(p2_lft);
            p1_rt->next = p2_rt;
            p2_rt->prev = p1_rt;
            p2_lft->next = p1_lft;
            p1_lft->prev = p2_lft;
        } else {
            // a b c x y z
            p1_rt->next = p2_lft;
            p2_lft->prev = p1_rt;
            p1_lft->prev = p2_rt;
            p2_rt->next = p1_lft;
        }
    }

    keep_ring->bottom_point = nullptr;
    bool keep_is_hole = ring_is_hole(keep_ring);
    bool remove_is_hole = ring_is_hole(remove_ring);

    remove_ring->points = nullptr;
    remove_ring->bottom_point = nullptr;
    if (keep_is_hole != remove_is_hole) {
        ring1_replaces_ring2(keep_ring->parent, remove_ring, manager);
    } else {
        ring1_replaces_ring2(keep_ring, remove_ring, manager);
    }

    update_points_ring(keep_ring);

    // nb: safe because we only get here via AddLocalMaxPoly
    keep_bound->ring = nullptr;
    remove_bound->ring = nullptr;

    for (auto& b : active_bounds) {
        if (b == nullptr) {
            continue;
        }
        if (b->ring == remove_ring) {
            b->ring = keep_ring;
            b->side = keep_bound->side;
            break; // Not sure why there is a break here but was transfered logic from angus
        }
    }
}

template <typename T>
void add_local_maximum_point(bound<T>& b1,
                             bound<T>& b2,
                             mapbox::geometry::point<T> const& pt,
                             ring_manager<T>& rings,
                             active_bound_list<T>& active_bounds) {
    insert_hot_pixels_in_path(b2, pt, rings, false);
    add_point(b1, active_bounds, pt, rings);
    if (b1.ring == b2.ring) {
        b1.ring = nullptr;
        b2.ring = nullptr;
        // I am not certain that order is important here?
    } else if (b1.ring->ring_index < b2.ring->ring_index) {
        append_ring(b1, b2, active_bounds, rings);
    } else {
        append_ring(b2, b1, active_bounds, rings);
    }
}

enum point_in_polygon_result : std::int8_t {
    point_on_polygon = -1,
    point_inside_polygon = 0,
    point_outside_polygon = 1
};

template <typename T>
point_in_polygon_result point_in_polygon(point<T> const& pt, point_ptr<T> op) {
    // returns 0 if false, +1 if true, -1 if pt ON polygon boundary
    point_in_polygon_result result = point_outside_polygon;
    point_ptr<T> startOp = op;
    do {
        if (op->next->y == pt.y) {
            if ((op->next->x == pt.x) ||
                (op->y == pt.y && ((op->next->x > pt.x) == (op->x < pt.x)))) {
                return point_on_polygon;
            }
        }
        if ((op->y < pt.y) != (op->next->y < pt.y)) {
            if (op->x >= pt.x) {
                if (op->next->x > pt.x) {
                    // Switch between point outside polygon and point inside
                    // polygon
                    if (result == point_outside_polygon) {
                        result = point_inside_polygon;
                    } else {
                        result = point_outside_polygon;
                    }
                } else {
                    double d =
                        static_cast<double>(op->x - pt.x) *
                            static_cast<double>(op->next->y - pt.y) -
                        static_cast<double>(op->next->x - pt.x) * static_cast<double>(op->y - pt.y);
                    if (value_is_zero(d)) {
                        return point_on_polygon;
                    }
                    if ((d > 0) == (op->next->y > op->y)) {
                        // Switch between point outside polygon and point inside
                        // polygon
                        if (result == point_outside_polygon) {
                            result = point_inside_polygon;
                        } else {
                            result = point_outside_polygon;
                        }
                    }
                }
            } else {
                if (op->next->x > pt.x) {
                    double d =
                        static_cast<double>(op->x - pt.x) *
                            static_cast<double>(op->next->y - pt.y) -
                        static_cast<double>(op->next->x - pt.x) * static_cast<double>(op->y - pt.y);
                    if (value_is_zero(d)) {
                        return point_on_polygon;
                    }
                    if ((d > 0) == (op->next->y > op->y)) {
                        // Switch between point outside polygon and point inside
                        // polygon
                        if (result == point_outside_polygon) {
                            result = point_inside_polygon;
                        } else {
                            result = point_outside_polygon;
                        }
                    }
                }
            }
        }
        op = op->next;
    } while (startOp != op);
    return result;
}

template <typename T>
point_in_polygon_result point_in_polygon(mapbox::geometry::point<double> const& pt,
                                         point_ptr<T> op) {
    // returns 0 if false, +1 if true, -1 if pt ON polygon boundary
    point_in_polygon_result result = point_outside_polygon;
    point_ptr<T> startOp = op;
    do {
        double op_x = static_cast<double>(op->x);
        double op_y = static_cast<double>(op->y);
        double op_next_x = static_cast<double>(op->next->x);
        double op_next_y = static_cast<double>(op->next->y);
        if (values_are_equal(op_next_y, pt.y)) {
            if (values_are_equal(op_next_x, pt.x) ||
                (values_are_equal(op_y, pt.y) && ((op_next_x > pt.x) == (op_x < pt.x)))) {
                return point_on_polygon;
            }
        }
        if ((op_y < pt.y) != (op_next_y < pt.y)) {
            if (greater_than_or_equal(op_x, pt.x)) {
                if (op_next_x > pt.x) {
                    // Switch between point outside polygon and point inside
                    // polygon
                    if (result == point_outside_polygon) {
                        result = point_inside_polygon;
                    } else {
                        result = point_outside_polygon;
                    }
                } else {
                    double d =
                        (op_x - pt.x) * (op_next_y - pt.y) - (op_next_x - pt.x) * (op_y - pt.y);
                    if (value_is_zero(d)) {
                        return point_on_polygon;
                    }
                    if ((d > 0.0) == (op_next_y > op_y)) {
                        // Switch between point outside polygon and point inside
                        // polygon
                        if (result == point_outside_polygon) {
                            result = point_inside_polygon;
                        } else {
                            result = point_outside_polygon;
                        }
                    }
                }
            } else {
                if (op_next_x > pt.x) {
                    double d =
                        (op_x - pt.x) * (op_next_y - pt.y) - (op_next_x - pt.x) * (op_y - pt.y);
                    if (value_is_zero(d)) {
                        return point_on_polygon;
                    }
                    if ((d > 0.0) == (op_next_y > op_y)) {
                        // Switch between point outside polygon and point inside
                        // polygon
                        if (result == point_outside_polygon) {
                            result = point_inside_polygon;
                        } else {
                            result = point_outside_polygon;
                        }
                    }
                }
            }
        }
        op = op->next;
    } while (startOp != op);
    return result;
}

template <typename T>
bool is_convex(point_ptr<T> edge) {
    point_ptr<T> prev = edge->prev;
    point_ptr<T> next = edge->next;
    T v1x = edge->x - prev->x;
    T v1y = edge->y - prev->y;
    T v2x = next->x - edge->x;
    T v2y = next->y - edge->y;
    T cross = v1x * v2y - v2x * v1y;
    if (cross < 0 && edge->ring->area() > 0) {
        return true;
    } else if (cross > 0 && edge->ring->area() < 0) {
        return true;
    } else {
        return false;
    }
}

template <typename T>
mapbox::geometry::point<double> centroid_of_points(point_ptr<T> edge) {
    point_ptr<T> prev = edge->prev;
    point_ptr<T> next = edge->next;
    return { static_cast<double>(prev->x + edge->x + next->x) / 3.0, static_cast<double>(prev->y + edge->y + next->y) / 3.0 };
}

template <typename T>
point_in_polygon_result inside_or_outside_special(point_ptr<T> first_pt, point_ptr<T> other_poly) {

    // We are going to loop through all the points
    // of the original triangle. The goal is to find a convex edge
    // that with its next and previous forms a triangle with its centroid
    // that is within the first ring. Then we will check the other polygon
    // to see if it is within this polygon.
    point_ptr<T> itr = first_pt;
    do {
        if (is_convex(itr)) {
            auto pt = centroid_of_points(itr);
            if (point_inside_polygon == point_in_polygon(pt, first_pt)) {
                return point_in_polygon(pt, other_poly);
            }
        }
        itr = itr->next;
    } while (itr != first_pt);

    throw std::runtime_error("Could not find a point within the polygon to test");
}

template <typename T>
bool box2_contains_box1(mapbox::geometry::box<T> const& box1,
                        mapbox::geometry::box<T> const& box2) {
    return (box2.max.x >= box1.max.x && box2.max.y >= box1.max.y && box2.min.x <= box1.min.x &&
            box2.min.y <= box1.min.y);
}

template <typename T>
bool poly2_contains_poly1(ring_ptr<T> ring1, ring_ptr<T> ring2) {
    if (!box2_contains_box1(ring1->bbox, ring2->bbox)) {
        return false;
    }
    if (std::fabs(ring2->area()) < std::fabs(ring1->area())) {
        return false;
    }
    point_ptr<T> outpt1 = ring1->points->next;
    point_ptr<T> outpt2 = ring2->points->next;
    point_ptr<T> op = outpt1;
    do {
        // nb: PointInPolygon returns 0 if false, +1 if true, -1 if pt on polygon
        point_in_polygon_result res = point_in_polygon(*op, outpt2);
        if (res != point_on_polygon) {
            return res == point_inside_polygon;
        }
        op = op->next;
    } while (op != outpt1);
    point_in_polygon_result res = inside_or_outside_special(outpt1, outpt2);
    return res == point_inside_polygon;
}
}
}
}
