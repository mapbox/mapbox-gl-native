#pragma once

#include <assert.h>
#include <cmath>
#include <deque>
#include <list>
#include <map>
#include <mapbox/geometry/box.hpp>
#include <mapbox/geometry/wagyu/point.hpp>
#include <set>
#include <sstream>
#include <vector>

#ifdef DEBUG
#include <execinfo.h>
#include <iostream>
#include <sstream>
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

namespace mapbox {
namespace geometry {
namespace wagyu {

template <typename T>
double area_from_point(point_ptr<T> op, std::size_t& size, mapbox::geometry::box<T>& bbox) {
    point_ptr<T> startOp = op;
    size = 0;
    double a = 0.0;
    T min_x = op->x;
    T max_x = op->x;
    T min_y = op->y;
    T max_y = op->y;
    do {
        ++size;
        if (op->x > max_x) {
            max_x = op->x;
        } else if (op->x < min_x) {
            min_x = op->x;
        }
        if (op->y > max_y) {
            max_y = op->y;
        } else if (op->y < min_y) {
            min_y = op->y;
        }
        a += static_cast<double>(op->prev->x + op->x) * static_cast<double>(op->prev->y - op->y);
        op = op->next;
    } while (op != startOp);
    bbox.min.x = min_x;
    bbox.max.x = max_x;
    bbox.min.y = min_y;
    bbox.max.y = max_y;
    return a * 0.5;
}

// NOTE: ring and ring_ptr are forward declared in wagyu/point.hpp

template <typename T>
using ring_vector = std::vector<ring_ptr<T>>;

template <typename T>
struct ring {
    std::size_t ring_index; // To support unset 0 is undefined and indexes offset by 1

    std::size_t size_;             // number of points in the ring
    double area_;                  // area of the ring
    mapbox::geometry::box<T> bbox; // bounding box of the ring

    ring_ptr<T> parent;
    ring_vector<T> children;

    point_ptr<T> points;
    point_ptr<T> bottom_point;
    bool is_hole_;
    bool corrected;

    ring(ring const&) = delete;
    ring& operator=(ring const&) = delete;

    ring()
        : ring_index(0),
          size_(0),
          area_(std::numeric_limits<double>::quiet_NaN()),
          bbox({ 0, 0 }, { 0, 0 }),
          parent(nullptr),
          children(),
          points(nullptr),
          bottom_point(nullptr),
          is_hole_(false),
          corrected(false) {
    }

    void reset_stats() {
        area_ = std::numeric_limits<double>::quiet_NaN();
        is_hole_ = false;
        bbox.min.x = 0;
        bbox.min.y = 0;
        bbox.max.x = 0;
        bbox.max.y = 0;
        size_ = 0;
    }

    void recalculate_stats() {
        if (points != nullptr) {
            area_ = area_from_point(points, size_, bbox);
            is_hole_ = !(area_ > 0.0);
        }
    }

    void set_stats(double a, std::size_t s, mapbox::geometry::box<T> const& b) {
        bbox = b;
        area_ = a;
        size_ = s;
        is_hole_ = !(area_ > 0.0);
    }

    double area() {
        if (std::isnan(area_)) {
            recalculate_stats();
        }
        return area_;
    }

    bool is_hole() {
        if (std::isnan(area_)) {
            recalculate_stats();
        }
        return is_hole_;
    }

    std::size_t size() {
        if (std::isnan(area_)) {
            recalculate_stats();
        }
        return size_;
    }
};

template <typename T>
using hot_pixel_vector = std::vector<mapbox::geometry::point<T>>;

template <typename T>
using hot_pixel_itr = typename hot_pixel_vector<T>::iterator;

template <typename T>
using hot_pixel_rev_itr = typename hot_pixel_vector<T>::reverse_iterator;

template <typename T>
struct ring_manager {

    ring_vector<T> children;
    point_vector<T> all_points;
    hot_pixel_vector<T> hot_pixels;
    hot_pixel_itr<T> current_hp_itr;
    std::deque<point<T>> points;
    std::deque<ring<T>> rings;
    std::vector<point<T>> storage;
    std::size_t index;

    ring_manager(ring_manager const&) = delete;
    ring_manager& operator=(ring_manager const&) = delete;

    ring_manager()
        : children(),
          all_points(),
          hot_pixels(),
          current_hp_itr(hot_pixels.end()),
          points(),
          rings(),
          storage(),
          index(0) {
    }
};

template <typename T>
void preallocate_point_memory(ring_manager<T>& rings, std::size_t size) {
    rings.storage.reserve(size);
    rings.all_points.reserve(size);
}

template <typename T>
ring_ptr<T> create_new_ring(ring_manager<T>& manager) {
    manager.rings.emplace_back();
    ring_ptr<T> result = &manager.rings.back();
    result->ring_index = manager.index++;
    return result;
}

template <typename T>
point_ptr<T>
create_new_point(ring_ptr<T> r, mapbox::geometry::point<T> const& pt, ring_manager<T>& rings) {
    point_ptr<T> point;
    if (rings.storage.size() < rings.storage.capacity()) {
        rings.storage.emplace_back(r, pt);
        point = &rings.storage.back();
    } else {
        rings.points.emplace_back(r, pt);
        point = &rings.points.back();
    }
    rings.all_points.push_back(point);
    return point;
}

template <typename T>
point_ptr<T> create_new_point(ring_ptr<T> r,
                              mapbox::geometry::point<T> const& pt,
                              point_ptr<T> before_this_point,
                              ring_manager<T>& rings) {
    point_ptr<T> point;
    if (rings.storage.size() < rings.storage.capacity()) {
        rings.storage.emplace_back(r, pt, before_this_point);
        point = &rings.storage.back();
    } else {
        rings.points.emplace_back(r, pt, before_this_point);
        point = &rings.points.back();
    }
    rings.all_points.push_back(point);
    return point;
}

template <typename T>
void set_to_children(ring_ptr<T> r, ring_vector<T>& children) {
    for (auto& c : children) {
        if (c == nullptr) {
            c = r;
            return;
        }
    }
    children.push_back(r);
}

template <typename T>
void remove_from_children(ring_ptr<T> r, ring_vector<T>& children) {
    for (auto& c : children) {
        if (c == r) {
            c = nullptr;
            return;
        }
    }
}

template <typename T>
void assign_as_child(ring_ptr<T> new_ring, ring_ptr<T> parent, ring_manager<T>& manager) {
    // Assigning as a child assumes that this is
    // a brand new ring. Therefore it does
    // not have any existing relationships
    if ((parent == nullptr && new_ring->is_hole()) ||
        (parent != nullptr && new_ring->is_hole() == parent->is_hole())) {
        throw std::runtime_error(
            "Trying to assign a child that is the same orientation as the parent");
    }
    auto& children = parent == nullptr ? manager.children : parent->children;
    set_to_children(new_ring, children);
    new_ring->parent = parent;
}

template <typename T>
void reassign_as_child(ring_ptr<T> ring, ring_ptr<T> parent, ring_manager<T>& manager) {
    // Reassigning a ring assumes it already
    // has an existing parent
    if ((parent == nullptr && ring->is_hole()) ||
        (parent != nullptr && ring->is_hole() == parent->is_hole())) {
        throw std::runtime_error(
            "Trying to re-assign a child that is the same orientation as the parent");
    }

    // Remove the old child relationship
    auto& old_children = ring->parent == nullptr ? manager.children : ring->parent->children;
    remove_from_children(ring, old_children);

    // Add new child relationship
    auto& children = parent == nullptr ? manager.children : parent->children;
    set_to_children(ring, children);
    ring->parent = parent;
}

template <typename T>
void assign_as_sibling(ring_ptr<T> new_ring, ring_ptr<T> sibling, ring_manager<T>& manager) {
    // Assigning as a sibling assumes that this is
    // a brand new ring. Therefore it does
    // not have any existing relationships
    if (new_ring->is_hole() != sibling->is_hole()) {
        throw std::runtime_error(
            "Trying to assign to be a sibling that is not the same orientation as the sibling");
    }
    auto& children = sibling->parent == nullptr ? manager.children : sibling->parent->children;
    set_to_children(new_ring, children);
    new_ring->parent = sibling->parent;
}

template <typename T>
void reassign_as_sibling(ring_ptr<T> ring, ring_ptr<T> sibling, ring_manager<T>& manager) {
    if (ring->parent == sibling->parent) {
        return;
    }
    // Assigning as a sibling assumes that this is
    // a brand new ring. Therefore it does
    // not have any existing relationships
    if (ring->is_hole() != sibling->is_hole()) {
        throw std::runtime_error(
            "Trying to assign to be a sibling that is not the same orientation as the sibling");
    }
    // Remove the old child relationship
    auto& old_children = ring->parent == nullptr ? manager.children : ring->parent->children;
    remove_from_children(ring, old_children);
    // Add new relationship
    auto& children = sibling->parent == nullptr ? manager.children : sibling->parent->children;
    set_to_children(ring, children);
    ring->parent = sibling->parent;
}

template <typename T>
void ring1_replaces_ring2(ring_ptr<T> ring1, ring_ptr<T> ring2, ring_manager<T>& manager) {
    assert(ring1 != ring2);
    auto& ring1_children = ring1 == nullptr ? manager.children : ring1->children;
    for (auto& c : ring2->children) {
        if (c == nullptr) {
            continue;
        }
        c->parent = ring1;
        set_to_children(c, ring1_children);
        c = nullptr;
    }
    // Remove the old child relationship
    auto& old_children = ring2->parent == nullptr ? manager.children : ring2->parent->children;
    remove_from_children(ring2, old_children);
    ring2->points = nullptr;
    ring2->reset_stats();
}

template <typename T>
void remove_points(point_ptr<T> pt) {
    if (pt != nullptr) {
        pt->prev->next = nullptr;
        while (pt != nullptr) {
            point_ptr<T> tmp = pt;
            pt = pt->next;
            tmp->next = nullptr;
            tmp->prev = nullptr;
            tmp->ring = nullptr;
        }
    }
}

template <typename T>
void remove_ring_and_points(ring_ptr<T> r,
                            ring_manager<T>& manager,
                            bool remove_children = true,
                            bool remove_from_parent = true) {
    // Removes a ring and any children that might be
    // under that ring.
    for (auto& c : r->children) {
        if (c == nullptr) {
            continue;
        }
        if (remove_children) {
            remove_ring_and_points(c, manager, true, false);
        }
        c = nullptr;
    }
    if (remove_from_parent) {
        // Remove the old child relationship
        auto& old_children = r->parent == nullptr ? manager.children : r->parent->children;
        remove_from_children(r, old_children);
    }
    point_ptr<T> pt = r->points;
    if (pt != nullptr) {
        pt->prev->next = nullptr;
        while (pt != nullptr) {
            point_ptr<T> tmp = pt;
            pt = pt->next;
            tmp->next = nullptr;
            tmp->prev = nullptr;
            tmp->ring = nullptr;
        }
    }
    r->points = nullptr;
    r->reset_stats();
}

template <typename T>
void remove_ring(ring_ptr<T> r,
                 ring_manager<T>& manager,
                 bool remove_children = true,
                 bool remove_from_parent = true) {
    // Removes a ring and any children that might be
    // under that ring.
    for (auto& c : r->children) {
        if (c == nullptr) {
            continue;
        }
        if (remove_children) {
            remove_ring(c, manager, true, false);
        }
        c = nullptr;
    }
    if (remove_from_parent) {
        // Remove the old child relationship
        auto& old_children = r->parent == nullptr ? manager.children : r->parent->children;
        remove_from_children(r, old_children);
    }
    r->points = nullptr;
    r->reset_stats();
}

template <typename T>
inline std::size_t ring_depth(ring_ptr<T> r) {
    std::size_t depth = 0;
    if (!r) {
        return depth;
    }
    while (r->parent) {
        depth++;
        r = r->parent;
    }
    return depth;
}

template <typename T>
inline bool ring_is_hole(ring_ptr<T> r) {
    // This is different then the "normal" way of determing if
    // a ring is a hole or not because it uses the depth of the
    // the ring to determine if it is a hole or not. This is only done
    // intially when rings are output from Vatti.
    return ring_depth(r) & 1;
}

template <typename T>
void set_next(const_point_ptr<T>& node, const const_point_ptr<T>& next_node) {
    node->next = next_node;
}

template <typename T>
point_ptr<T> get_next(const_point_ptr<T>& node) {
    return node->next;
}

template <typename T>
point_ptr<T> get_prev(const_point_ptr<T>& node) {
    return node->prev;
}

template <typename T>
void set_prev(const_point_ptr<T>& node, const const_point_ptr<T>& prev_node) {
    node->prev = prev_node;
}

template <typename T>
void init(const_point_ptr<T>& node) {
    set_next(node, node);
    set_prev(node, node);
}

template <typename T>
void link_before(point_ptr<T>& node, point_ptr<T>& new_node) {
    point_ptr<T> prev_node = get_prev(node);
    set_prev(new_node, prev_node);
    set_next(new_node, node);
    set_prev(node, new_node);
    set_next(prev_node, new_node);
}

template <typename T>
void link_after(point_ptr<T>& node, point_ptr<T>& new_node) {
    point_ptr<T> next_node = get_next(node);
    set_prev(new_node, node);
    set_next(new_node, next_node);
    set_next(node, new_node);
    set_prev(next_node, new_node);
}

template <typename T>
void transfer_point(point_ptr<T>& p, point_ptr<T>& b, point_ptr<T>& e) {
    if (b != e) {
        point_ptr<T> prev_p = get_prev(p);
        point_ptr<T> prev_b = get_prev(b);
        point_ptr<T> prev_e = get_prev(e);
        set_next(prev_e, p);
        set_prev(p, prev_e);
        set_next(prev_b, e);
        set_prev(e, prev_b);
        set_next(prev_p, b);
        set_prev(b, prev_p);
    } else {
        link_before(p, b);
    }
}

template <typename T>
void reverse_ring(point_ptr<T> pp) {
    if (!pp) {
        return;
    }
    point_ptr<T> pp1;
    point_ptr<T> pp2;
    pp1 = pp;
    do {
        pp2 = pp1->next;
        pp1->next = pp1->prev;
        pp1->prev = pp2;
        pp1 = pp2;
    } while (pp1 != pp);
}

#ifdef DEBUG

template <class charT, class traits, typename T>
inline std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& out,
                                                     ring<T>& r) {
    out << "  ring_index: " << r.ring_index << std::endl;
    if (!r.parent) {
        // out << "  parent_ring ptr: nullptr" << std::endl;
        out << "  parent_index: -----" << std::endl;
    } else {
        // out << "  parent_ring ptr: " << r.parent << std::endl;
        out << "  parent_ring idx: " << r.parent->ring_index << std::endl;
    }
    ring_ptr<T> n = const_cast<ring_ptr<T>>(&r);
    if (ring_is_hole(n)) {
        out << "  is_hole: true " << std::endl;
    } else {
        out << "  is_hole: false " << std::endl;
    }
    auto pt_itr = r.points;
    if (pt_itr) {
        out << "  area: " << r.area() << std::endl;
        out << "  points:" << std::endl;
        out << "      [[[" << pt_itr->x << "," << pt_itr->y << "],";
        pt_itr = pt_itr->next;
        while (pt_itr != r.points) {
            out << "[" << pt_itr->x << "," << pt_itr->y << "],";
            pt_itr = pt_itr->next;
        }
        out << "[" << pt_itr->x << "," << pt_itr->y << "]]]" << std::endl;
    } else {
        out << "  area: NONE" << std::endl;
        out << "  points: NONE" << std::endl;
    }
    return out;
}

template <typename T>
std::string debug_ring_addresses(ring_ptr<T> r) {
    std::ostringstream out;
    out << "Ring: " << r->ring_index << std::endl;
    if (r->points == nullptr) {
        out << "   Ring has no points" << std::endl;
        return out.str();
    }
    auto pt_itr = r->points;
    do {
        out << "    [" << pt_itr->x << "," << pt_itr->y << "] - " << pt_itr << std::endl;
        pt_itr = pt_itr->next;
    } while (pt_itr != r->points);
    return out.str();
}

template <typename T>
std::string output_as_polygon(ring_ptr<T> r) {
    std::ostringstream out;

    auto pt_itr = r->points;
    if (pt_itr) {
        out << "[";
        out << "[[" << pt_itr->x << "," << pt_itr->y << "],";
        pt_itr = pt_itr->next;
        while (pt_itr != r->points) {
            out << "[" << pt_itr->x << "," << pt_itr->y << "],";
            pt_itr = pt_itr->next;
        }
        out << "[" << pt_itr->x << "," << pt_itr->y << "]]";
        for (auto const& c : r->children) {
            if (c == nullptr) {
                continue;
            }
            pt_itr = c->points;
            if (pt_itr) {
                out << ",[[" << pt_itr->x << "," << pt_itr->y << "],";
                pt_itr = pt_itr->next;
                while (pt_itr != c->points) {
                    out << "[" << pt_itr->x << "," << pt_itr->y << "],";
                    pt_itr = pt_itr->next;
                }
                out << "[" << pt_itr->x << "," << pt_itr->y << "]]";
            }
        }
        out << "]" << std::endl;
    } else {
        out << "[]" << std::endl;
    }

    return out.str();
}

template <class charT, class traits, typename T>
inline std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& out,
                                                     ring_vector<T>& rings) {
    out << "START RING VECTOR" << std::endl;
    for (auto& r : rings) {
        if (r == nullptr || !r->points) {
            continue;
        }
        out << " ring: " << r->ring_index << " - " << r << std::endl;
        out << *r;
    }
    out << "END RING VECTOR" << std::endl;
    return out;
}

template <class charT, class traits, typename T>
inline std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& out,
                                                     std::deque<ring<T>>& rings) {
    out << "START RING VECTOR" << std::endl;
    for (auto& r : rings) {
        if (!r.points) {
            continue;
        }
        out << " ring: " << r.ring_index << std::endl;
        out << r;
    }
    out << "END RING VECTOR" << std::endl;
    return out;
}

template <class charT, class traits, typename T>
inline std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& out,
                                                     hot_pixel_vector<T>& hp_vec) {
    out << "Hot Pixels: " << std::endl;
    for (auto& hp : hp_vec) {
        out << hp << std::endl;
    }
    return out;
}
#endif
}
}
}
