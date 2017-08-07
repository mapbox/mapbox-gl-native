#pragma once

#include <mapbox/geometry/wagyu/active_bound_list.hpp>
#include <mapbox/geometry/wagyu/bound.hpp>
#include <mapbox/geometry/wagyu/bubble_sort.hpp>
#include <mapbox/geometry/wagyu/config.hpp>
#include <mapbox/geometry/wagyu/intersect.hpp>
#include <mapbox/geometry/wagyu/ring_util.hpp>
#include <mapbox/geometry/wagyu/util.hpp>

#include <algorithm>

namespace mapbox {
namespace geometry {
namespace wagyu {

template <typename T>
struct intersect_list_sorter {
    inline bool operator()(intersect_node<T> const& node1, intersect_node<T> const& node2) {
        if (!values_are_equal(node2.pt.y, node1.pt.y)) {
            return node2.pt.y < node1.pt.y;
        } else {
            return (node2.bound1->winding_count2 + node2.bound2->winding_count2) >
                   (node1.bound1->winding_count2 + node1.bound2->winding_count2);
        }
    }
};

template <typename T>
inline mapbox::geometry::point<T> round_point(mapbox::geometry::point<double> const& pt) {
    return mapbox::geometry::point<T>(round_towards_max<T>(pt.x), round_towards_max<T>(pt.y));
}

template <typename T>
inline void swap_rings(bound<T>& b1, bound<T>& b2) {
    ring_ptr<T> ring = b1.ring;
    b1.ring = b2.ring;
    b2.ring = ring;
}

template <typename T>
inline void swap_sides(bound<T>& b1, bound<T>& b2) {
    edge_side side = b1.side;
    b1.side = b2.side;
    b2.side = side;
}

template <typename T1, typename T2>
bool get_edge_intersection(edge<T1> const& e1,
                           edge<T1> const& e2,
                           mapbox::geometry::point<T2>& pt) {
    T2 p0_x = static_cast<T2>(e1.bot.x);
    T2 p0_y = static_cast<T2>(e1.bot.y);
    T2 p1_x = static_cast<T2>(e1.top.x);
    T2 p1_y = static_cast<T2>(e1.top.y);
    T2 p2_x = static_cast<T2>(e2.bot.x);
    T2 p2_y = static_cast<T2>(e2.bot.y);
    T2 p3_x = static_cast<T2>(e2.top.x);
    T2 p3_y = static_cast<T2>(e2.top.y);
    T2 s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;
    s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;
    s2_y = p3_y - p2_y;

    T2 s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    T2 t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0.0 && s <= 1.0 && t >= 0.0 && t <= 1.0) {
        pt.x = p0_x + (t * s1_x);
        pt.y = p0_y + (t * s1_y);
        return true;
    }
    // LCOV_EXCL_START
    return false;
    // LCOV_EXCL_END
}

template <typename T>
struct intersection_compare {
    bool operator()(bound_ptr<T> const& b1, bound_ptr<T> const& b2) {
        return !(b1->current_x > b2->current_x &&
                 !slopes_equal(*(b1->current_edge), *(b2->current_edge)));
    }
};

template <typename T>
struct on_intersection_swap {

    intersect_list<T>& intersects;

    on_intersection_swap(intersect_list<T>& i) : intersects(i) {
    }

    void operator()(bound_ptr<T> const& b1, bound_ptr<T> const& b2) {
        mapbox::geometry::point<double> pt;
        if (!get_edge_intersection<T, double>(*(b1->current_edge), *(b2->current_edge), pt)) {
            // LCOV_EXCL_START
            throw std::runtime_error("Trying to find intersection of lines that do not intersect");
            // LCOV_EXCL_END
        }
        intersects.emplace_back(b1, b2, pt);
    }
};

template <typename T>
void build_intersect_list(active_bound_list<T>& active_bounds, intersect_list<T>& intersects) {
    bubble_sort(active_bounds.begin(), active_bounds.end(), intersection_compare<T>(),
                on_intersection_swap<T>(intersects));
}

template <typename T>
void intersect_bounds(bound<T>& b1,
                      bound<T>& b2,
                      mapbox::geometry::point<T> const& pt,
                      clip_type cliptype,
                      fill_type subject_fill_type,
                      fill_type clip_fill_type,
                      ring_manager<T>& rings,
                      active_bound_list<T>& active_bounds) {
    bool b1Contributing = (b1.ring != nullptr);
    bool b2Contributing = (b2.ring != nullptr);

    // update winding counts...
    // assumes that b1 will be to the Right of b2 ABOVE the intersection
    if (b1.poly_type == b2.poly_type) {
        if (is_even_odd_fill_type(b1, subject_fill_type, clip_fill_type)) {
            std::swap(b1.winding_count, b2.winding_count);
        } else {
            if (b1.winding_count + b2.winding_delta == 0) {
                b1.winding_count = -b1.winding_count;
            } else {
                b1.winding_count += b2.winding_delta;
            }
            if (b2.winding_count - b1.winding_delta == 0) {
                b2.winding_count = -b2.winding_count;
            } else {
                b2.winding_count -= b1.winding_delta;
            }
        }
    } else {
        if (!is_even_odd_fill_type(b2, subject_fill_type, clip_fill_type)) {
            b1.winding_count2 += b2.winding_delta;
        } else {
            b1.winding_count2 = (b1.winding_count2 == 0) ? 1 : 0;
        }
        if (!is_even_odd_fill_type(b1, subject_fill_type, clip_fill_type)) {
            b2.winding_count2 -= b1.winding_delta;
        } else {
            b2.winding_count2 = (b2.winding_count2 == 0) ? 1 : 0;
        }
    }

    fill_type b1FillType, b2FillType, b1FillType2, b2FillType2;
    if (b1.poly_type == polygon_type_subject) {
        b1FillType = subject_fill_type;
        b1FillType2 = clip_fill_type;
    } else {
        b1FillType = clip_fill_type;
        b1FillType2 = subject_fill_type;
    }
    if (b2.poly_type == polygon_type_subject) {
        b2FillType = subject_fill_type;
        b2FillType2 = clip_fill_type;
    } else {
        b2FillType = clip_fill_type;
        b2FillType2 = subject_fill_type;
    }

    std::int32_t b1Wc, b2Wc;
    switch (b1FillType) {
    case fill_type_positive:
        b1Wc = b1.winding_count;
        break;
    case fill_type_negative:
        b1Wc = -b1.winding_count;
        break;
    case fill_type_even_odd:
    case fill_type_non_zero:
    default:
        b1Wc = std::abs(static_cast<int>(b1.winding_count));
    }
    switch (b2FillType) {
    case fill_type_positive:
        b2Wc = b2.winding_count;
        break;
    case fill_type_negative:
        b2Wc = -b2.winding_count;
        break;
    case fill_type_even_odd:
    case fill_type_non_zero:
    default:
        b2Wc = std::abs(static_cast<int>(b2.winding_count));
    }
    if (b1Contributing && b2Contributing) {
        if ((b1Wc != 0 && b1Wc != 1) || (b2Wc != 0 && b2Wc != 1) ||
            (b1.poly_type != b2.poly_type && cliptype != clip_type_x_or)) {
            add_local_maximum_point(b1, b2, pt, rings, active_bounds);
        } else {
            add_point(b1, active_bounds, pt, rings);
            add_point(b2, active_bounds, pt, rings);
            swap_sides(b1, b2);
            swap_rings(b1, b2);
        }
    } else if (b1Contributing) {
        if (b2Wc == 0 || b2Wc == 1) {
            add_point(b1, active_bounds, pt, rings);
            b2.last_point = pt;
            swap_sides(b1, b2);
            swap_rings(b1, b2);
        }
    } else if (b2Contributing) {
        if (b1Wc == 0 || b1Wc == 1) {
            b1.last_point = pt;
            add_point(b2, active_bounds, pt, rings);
            swap_sides(b1, b2);
            swap_rings(b1, b2);
        }
    } else if ((b1Wc == 0 || b1Wc == 1) && (b2Wc == 0 || b2Wc == 1)) {
        // neither bound is currently contributing ...

        std::int32_t b1Wc2, b2Wc2;
        switch (b1FillType2) {
        case fill_type_positive:
            b1Wc2 = b1.winding_count2;
            break;
        case fill_type_negative:
            b1Wc2 = -b1.winding_count2;
            break;
        case fill_type_even_odd:
        case fill_type_non_zero:
        default:
            b1Wc2 = std::abs(static_cast<int>(b1.winding_count2));
        }
        switch (b2FillType2) {
        case fill_type_positive:
            b2Wc2 = b2.winding_count2;
            break;
        case fill_type_negative:
            b2Wc2 = -b2.winding_count2;
            break;
        case fill_type_even_odd:
        case fill_type_non_zero:
        default:
            b2Wc2 = std::abs(static_cast<int>(b2.winding_count2));
        }

        if (b1.poly_type != b2.poly_type) {
            add_local_minimum_point(b1, b2, active_bounds, pt, rings);
        } else if (b1Wc == 1 && b2Wc == 1) {
            switch (cliptype) {
            case clip_type_intersection:
                if (b1Wc2 > 0 && b2Wc2 > 0) {
                    add_local_minimum_point(b1, b2, active_bounds, pt, rings);
                }
                break;
            default:
            case clip_type_union:
                if (b1Wc2 <= 0 && b2Wc2 <= 0) {
                    add_local_minimum_point(b1, b2, active_bounds, pt, rings);
                }
                break;
            case clip_type_difference:
                if (((b1.poly_type == polygon_type_clip) && (b1Wc2 > 0) && (b2Wc2 > 0)) ||
                    ((b1.poly_type == polygon_type_subject) && (b1Wc2 <= 0) && (b2Wc2 <= 0))) {
                    add_local_minimum_point(b1, b2, active_bounds, pt, rings);
                }
                break;
            case clip_type_x_or:
                add_local_minimum_point(b1, b2, active_bounds, pt, rings);
            }
        } else {
            swap_sides(b1, b2);
        }
    }
}

template <typename T>
bool bounds_adjacent(intersect_node<T> const& inode, bound_ptr<T> next) {
    return (next == inode.bound2) || (next == inode.bound1);
}

template <typename T>
struct find_first_bound {
    bound_ptr<T> b1;
    bound_ptr<T> b2;

    find_first_bound(intersect_node<T> const& inode) : b1(inode.bound1), b2(inode.bound2) {
    }

    bool operator()(bound_ptr<T> const& b) {
        return b == b1 || b == b2;
    }
};

template <typename T>
void process_intersect_list(intersect_list<T>& intersects,
                            clip_type cliptype,
                            fill_type subject_fill_type,
                            fill_type clip_fill_type,
                            ring_manager<T>& rings,
                            active_bound_list<T>& active_bounds) {
    for (auto node_itr = intersects.begin(); node_itr != intersects.end(); ++node_itr) {
        auto b1 = std::find_if(active_bounds.begin(), active_bounds.end(),
                               find_first_bound<T>(*node_itr));
        auto b2 = std::next(b1);
        if (!bounds_adjacent(*node_itr, *b2)) {
            auto next_itr = std::next(node_itr);
            while (next_itr != intersects.end()) {
                auto n1 = std::find_if(active_bounds.begin(), active_bounds.end(),
                                       find_first_bound<T>(*next_itr));
                auto n2 = std::next(n1);
                if (bounds_adjacent(*next_itr, *n2)) {
                    b1 = n1;
                    b2 = n2;
                    break;
                }
                ++next_itr;
            }
            if (next_itr == intersects.end()) {
                throw std::runtime_error("Could not properly correct intersection order.");
            }
            std::iter_swap(node_itr, next_itr);
        }
        mapbox::geometry::point<T> pt = round_point<T>(node_itr->pt);
        intersect_bounds(*(node_itr->bound1), *(node_itr->bound2), pt, cliptype, subject_fill_type,
                         clip_fill_type, rings, active_bounds);
        std::iter_swap(b1, b2);
    }
}

template <typename T>
void update_current_x(active_bound_list<T>& active_bounds, T top_y) {
    std::size_t pos = 0;
    for (auto& bnd : active_bounds) {
        bnd->pos = pos++;
        bnd->current_x = get_current_x(*bnd->current_edge, top_y);
    }
}

template <typename T>
void process_intersections(T top_y,
                           active_bound_list<T>& active_bounds,
                           clip_type cliptype,
                           fill_type subject_fill_type,
                           fill_type clip_fill_type,
                           ring_manager<T>& rings) {
    if (active_bounds.empty()) {
        return;
    }
    update_current_x(active_bounds, top_y);
    intersect_list<T> intersects;
    build_intersect_list(active_bounds, intersects);

    if (intersects.empty()) {
        return;
    }

    // Restore order of active bounds list
    std::stable_sort(
        active_bounds.begin(), active_bounds.end(),
        [](bound_ptr<T> const& b1, bound_ptr<T> const& b2) { return b1->pos < b2->pos; });

    // Sort the intersection list
    std::stable_sort(intersects.begin(), intersects.end(), intersect_list_sorter<T>());

    process_intersect_list(intersects, cliptype, subject_fill_type, clip_fill_type, rings,
                           active_bounds);
}
}
}
}
