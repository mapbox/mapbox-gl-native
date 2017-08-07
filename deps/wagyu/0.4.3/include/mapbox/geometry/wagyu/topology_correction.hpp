#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include <algorithm>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <utility>

#include <mapbox/geometry/wagyu/config.hpp>
#include <mapbox/geometry/wagyu/ring.hpp>
#include <mapbox/geometry/wagyu/ring_util.hpp>

#ifdef DEBUG
#include <iostream>
#include <sstream>
#endif

namespace mapbox {
namespace geometry {
namespace wagyu {

template <typename T>
struct point_ptr_pair {
    point_ptr<T> op1;
    point_ptr<T> op2;

    constexpr point_ptr_pair(point_ptr<T> o1, point_ptr<T> o2) : op1(o1), op2(o2) {
    }

    point_ptr_pair(point_ptr_pair<T> const& p) = default;

    point_ptr_pair(point_ptr_pair<T>&& p) : op1(std::move(p.op1)), op2(std::move(p.op2)) {
    }

    point_ptr_pair& operator=(point_ptr_pair<T>&& p) {
        op1 = std::move(p.op1);
        op2 = std::move(p.op2);
        return *this;
    }
};

#ifdef DEBUG

template <class charT, class traits, typename T>
inline std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& out,
           const std::unordered_multimap<ring_ptr<T>, point_ptr_pair<T>>& dupe_ring) {

    out << " BEGIN CONNECTIONS: " << std::endl;
    for (auto& r : dupe_ring) {
        out << "  Ring: ";
        if (r.second.op1->ring) {
            out << r.second.op1->ring->ring_index;
        } else {
            out << "---";
        }
        out << " to ";
        if (r.second.op2->ring) {
            out << r.second.op2->ring->ring_index;
        } else {
            out << "---";
        }
        out << "  ( at " << r.second.op1->x << ", " << r.second.op1->y << " )";
        out << "  Ring1 ( ";
        if (r.second.op1->ring) {
            out << "area: " << r.second.op1->ring->area << " parent: ";
            if (r.second.op1->ring->parent) {
                out << r.second.op1->ring->parent->ring_index;
            } else {
                out << "---";
            }
        } else {
            out << "---";
        }
        out << " )";
        out << "  Ring2 ( ";
        if (r.second.op2->ring) {
            out << "area: " << r.second.op2->ring->area << " parent: ";
            if (r.second.op2->ring->parent) {
                out << r.second.op2->ring->parent->ring_index;
            } else {
                out << "---";
            }
        } else {
            out << "---";
        }
        out << " )";
        out << std::endl;
    }
    out << " END CONNECTIONS: " << std::endl;
    return out;
}

#endif

template <typename T>
bool find_intersect_loop(std::unordered_multimap<ring_ptr<T>, point_ptr_pair<T>>& dupe_ring,
                         std::list<std::pair<ring_ptr<T>, point_ptr_pair<T>>>& iList,
                         ring_ptr<T> ring_parent,
                         ring_ptr<T> ring_origin,
                         ring_ptr<T> ring_search,
                         std::set<ring_ptr<T>>& visited,
                         point_ptr<T> orig_pt,
                         point_ptr<T> prev_pt,
                         ring_manager<T>& rings) {
    {
        auto range = dupe_ring.equal_range(ring_search);
        // Check for direct connection
        for (auto& it = range.first; it != range.second;) {
            ring_ptr<T> it_ring1 = it->second.op1->ring;
            ring_ptr<T> it_ring2 = it->second.op2->ring;
            if (!it_ring1 || !it_ring2 || it_ring1 != ring_search ||
                (!it_ring1->is_hole() && !it_ring2->is_hole())) {
                it = dupe_ring.erase(it);
                continue;
            }
            if (it_ring2 == ring_origin &&
                (ring_parent == it_ring2 || ring_parent == it_ring2->parent) &&
                *prev_pt != *it->second.op2 && *orig_pt != *it->second.op2) {
                iList.emplace_front(ring_search, it->second);
                return true;
            }
            ++it;
        }
    }
    auto range = dupe_ring.equal_range(ring_search);
    visited.insert(ring_search);
    // Check for connection through chain of other intersections
    for (auto& it = range.first;
         it != range.second && it != dupe_ring.end() && it->first == ring_search; ++it) {
        ring_ptr<T> it_ring = it->second.op2->ring;
        if (visited.count(it_ring) > 0 || it_ring == nullptr ||
            (ring_parent != it_ring && ring_parent != it_ring->parent) ||
            value_is_zero(it_ring->area()) || *prev_pt == *it->second.op2) {
            continue;
        }
        if (find_intersect_loop(dupe_ring, iList, ring_parent, ring_origin, it_ring, visited,
                                orig_pt, it->second.op2, rings)) {
            iList.emplace_front(ring_search, it->second);
            return true;
        }
    }
    return false;
}

template <typename T>
struct point_ptr_cmp {
    inline bool operator()(point_ptr<T> op1, point_ptr<T> op2) {
        if (op1->y != op2->y) {
            return (op1->y > op2->y);
        } else if (op1->x != op2->x) {
            return (op1->x < op2->x);
        } else {
            std::size_t depth_1 = ring_depth(op1->ring);
            std::size_t depth_2 = ring_depth(op2->ring);
            return depth_1 > depth_2;
        }
    }
};

template <typename T>
void correct_orientations(ring_manager<T>& manager) {
    for (auto& r : manager.rings) {
        if (!r.points) {
            continue;
        }
        r.recalculate_stats();
        if (r.size() < 3) {
            remove_ring_and_points(&r, manager, false);
            continue;
        }
        if (ring_is_hole(&r) != r.is_hole()) {
            reverse_ring(r.points);
            r.recalculate_stats();
        }
    }
}

template <typename T>
point_vector<T> sort_ring_points(ring_ptr<T> r) {
    point_vector<T> sorted_points;
    point_ptr<T> point_itr = r->points;
    point_ptr<T> last_point = point_itr->prev;
    while (point_itr != last_point) {
        sorted_points.push_back(point_itr);
        point_itr = point_itr->next;
    }
    sorted_points.push_back(last_point);
    std::stable_sort(sorted_points.begin(), sorted_points.end(),
                     [](point_ptr<T> const& pt1, point_ptr<T> const& pt2) {
                         if (pt1->y != pt2->y) {
                             return (pt1->y > pt2->y);
                         }
                         return (pt1->x < pt2->x);
                     });
    return sorted_points;
}

template <typename T>
ring_ptr<T> correct_self_intersection(point_ptr<T> pt1,
                                      point_ptr<T> pt2,
                                      ring_manager<T>& manager) {
    if (pt1->ring != pt2->ring) {
        return static_cast<ring_ptr<T>>(nullptr);
    }

    ring_ptr<T> ring = pt1->ring;

    // split the polygon into two ...
    point_ptr<T> pt3 = pt1->prev;
    point_ptr<T> pt4 = pt2->prev;
    pt1->prev = pt4;
    pt4->next = pt1;
    pt2->prev = pt3;
    pt3->next = pt2;

    ring_ptr<T> new_ring = create_new_ring(manager);
    std::size_t size_1 = 0;
    std::size_t size_2 = 0;
    mapbox::geometry::box<T> box1({ 0, 0 }, { 0, 0 });
    mapbox::geometry::box<T> box2({ 0, 0 }, { 0, 0 });
    double area_1 = area_from_point(pt1, size_1, box1);
    double area_2 = area_from_point(pt2, size_2, box2);

    if (std::fabs(area_1) > std::fabs(area_2)) {
        ring->points = pt1;
        ring->set_stats(area_1, size_1, box1);
        new_ring->points = pt2;
        new_ring->set_stats(area_2, size_2, box2);
    } else {
        ring->points = pt2;
        ring->set_stats(area_2, size_2, box2);
        new_ring->points = pt1;
        new_ring->set_stats(area_1, size_1, box1);
    }
    update_points_ring(new_ring);
    return new_ring;
}

template <typename T>
void correct_repeated_points(ring_manager<T>& manager,
                             ring_vector<T>& new_rings,
                             point_vector_itr<T> const& begin,
                             point_vector_itr<T> const& end) {
    for (auto itr1 = begin; itr1 != end; ++itr1) {
        if ((*itr1)->ring == nullptr) {
            continue;
        }
        for (auto itr2 = std::next(itr1); itr2 != end; ++itr2) {
            if ((*itr2)->ring == nullptr) {
                continue;
            }
            ring_ptr<T> new_ring = correct_self_intersection(*itr1, *itr2, manager);
            if (new_ring != nullptr) {
                new_rings.push_back(new_ring);
            }
        }
    }
}

template <typename T>
void find_and_correct_repeated_points(ring_ptr<T> r,
                                      ring_manager<T>& manager,
                                      ring_vector<T>& new_rings) {
    auto sorted_points = sort_ring_points(r);
    // Find sets of repeated points
    std::size_t count = 0;
    auto prev_itr = sorted_points.begin();
    auto itr = std::next(prev_itr);
    while (itr != sorted_points.end()) {
        if (*(*prev_itr) == *(*(itr))) {
            ++count;
            ++prev_itr;
            ++itr;
            if (itr != sorted_points.end()) {
                continue;
            } else {
                ++prev_itr;
            }
        } else {
            ++prev_itr;
            ++itr;
        }
        if (count == 0) {
            continue;
        }
        auto first = prev_itr;
        std::advance(first, -(static_cast<int>(count) + 1));
        correct_repeated_points(manager, new_rings, first, prev_itr);
        count = 0;
    }
}

template <typename T>
void reassign_children_if_necessary(ring_ptr<T> new_ring,
                                    ring_ptr<T> sibling_ring,
                                    ring_manager<T>& manager,
                                    ring_vector<T>& new_rings) {
    auto& children = sibling_ring == nullptr ? manager.children : sibling_ring->children;
    for (auto c : children) {
        if (c == nullptr) {
            continue;
        }
        if (std::find(new_rings.begin(), new_rings.end(), c) != new_rings.end()) {
            continue;
        }
        if (poly2_contains_poly1(c, new_ring)) {
            reassign_as_child(c, new_ring, manager);
        }
    }
}

template <typename T>
bool find_parent_in_tree(ring_ptr<T> r, ring_ptr<T> possible_parent, ring_manager<T>& manager) {
    // Before starting this we are assuming that possible_parent
    // and r have opposite signs of their areas

    // First we must search all grandchildren
    for (auto c : possible_parent->children) {
        if (c == nullptr) {
            continue;
        }
        for (auto gc : c->children) {
            if (gc == nullptr) {
                continue;
            }
            if (find_parent_in_tree(r, gc, manager)) {
                return true;
            }
        }
    }

    if (poly2_contains_poly1(r, possible_parent)) {
        reassign_as_child(r, possible_parent, manager);
        return true;
    }
    return false;
}

template <typename T>
void assign_new_ring_parents(ring_manager<T>& manager,
                             ring_ptr<T> original_ring,
                             ring_vector<T>& new_rings) {

    // First lets remove any rings that have zero area
    // or have no points
    new_rings.erase(std::remove_if(new_rings.begin(), new_rings.end(),
                                   [](ring_ptr<T> const& r) {
                                       if (r->points == nullptr) {
                                           return true;
                                       }
                                       return value_is_zero(r->area());
                                   }),
                    new_rings.end());

    if (new_rings.empty()) {
        // No new rings created simply return;
        return;
    }

    // We should not have to re-assign the parent of the original ring
    // because we always maintained the largest ring during splitting
    // on repeated points.

    double original_ring_area = original_ring->area();
    bool original_positive = original_ring_area > 0.0;

    // If there is only one new ring the logic is very simple and we
    // do not have to check which ring contains, we only need to compare
    // the areas of the original ring and that of the new ring.
    if (new_rings.size() == 1) {
        double new_ring_area = new_rings.front()->area();
        bool new_positive = new_ring_area > 0.0;
        if (original_positive == new_positive) {
            // The rings should be siblings
            assign_as_child(new_rings.front(), original_ring->parent, manager);
            reassign_children_if_necessary(new_rings.front(), original_ring, manager, new_rings);
        } else {
            // The new ring is a child of original ring
            // Check the
            assign_as_child(new_rings.front(), original_ring, manager);
            reassign_children_if_necessary(new_rings.front(), original_ring->parent, manager,
                                           new_rings);
        }
        return;
    }

    // Now we want to sort rings from the largest in absolute area to the smallest
    // as we will assign the rings with the largest areas first
    std::stable_sort(new_rings.begin(), new_rings.end(),
                     [](ring_ptr<T> const& r1, ring_ptr<T> const& r2) {
                         return std::fabs(r1->area()) > std::fabs(r2->area());
                     });

    for (auto r_itr = new_rings.begin(); r_itr != new_rings.end(); ++r_itr) {
        double new_ring_area = (*r_itr)->area();
        bool new_positive = new_ring_area > 0.0;
        bool same_orientation = new_positive == original_positive;
        bool found = false;
        // First lets check the trees of any new_rings that might have
        // been assigned as siblings to the original ring.
        for (auto s_itr = new_rings.begin(); s_itr != r_itr; ++s_itr) {
            if ((*s_itr)->parent != original_ring->parent) {
                continue;
            }
            if (same_orientation) {
                for (auto s_child : (*s_itr)->children) {
                    if (s_child == nullptr) {
                        continue;
                    }
                    if (find_parent_in_tree(*r_itr, s_child, manager)) {
                        reassign_children_if_necessary(*r_itr, original_ring, manager, new_rings);
                        found = true;
                        break;
                    }
                }
            } else {
                if (find_parent_in_tree(*r_itr, *s_itr, manager)) {
                    reassign_children_if_necessary(*r_itr, original_ring->parent, manager,
                                                   new_rings);
                    found = true;
                }
            }
            if (found) {
                break;
            }
        }

        if (found) {
            continue;
        }

        // Next lets check the tree of the original_ring
        if (same_orientation) {
            for (auto o_child : original_ring->children) {
                if (o_child == nullptr) {
                    continue;
                }
                if (find_parent_in_tree(*r_itr, o_child, manager)) {
                    reassign_children_if_necessary(*r_itr, original_ring, manager, new_rings);
                    found = true;
                    break;
                }
            }
            if (!found) {
                // If we didn't find any parent and the same orientation
                // then it must be a sibling of the original ring
                assign_as_child(*r_itr, original_ring->parent, manager);
                reassign_children_if_necessary(*r_itr, original_ring, manager, new_rings);
            }
        } else {
            if (find_parent_in_tree(*r_itr, original_ring, manager)) {
                reassign_children_if_necessary(*r_itr, original_ring->parent, manager, new_rings);
            } else {
                throw std::runtime_error("Unable to find a proper parent ring");
            }
        }
    }
}

template <typename T>
bool correct_ring_self_intersections(ring_manager<T>& manager, ring_ptr<T> r, bool correct_tree) {

    if (r->corrected || !r->points) {
        return false;
    }

    ring_vector<T> new_rings;

    find_and_correct_repeated_points(r, manager, new_rings);

    if (correct_tree) {
        assign_new_ring_parents(manager, r, new_rings);
    }

    r->corrected = true;
    return true;
}

template <typename T>
void process_single_intersection(
    std::unordered_multimap<ring_ptr<T>, point_ptr_pair<T>>& connection_map,
    point_ptr<T> op_j,
    point_ptr<T> op_k,
    ring_manager<T>& manager) {
    ring_ptr<T> ring_j = op_j->ring;
    ring_ptr<T> ring_k = op_k->ring;
    if (ring_j == ring_k) {
        return;
    }

    if (!ring_j->is_hole() && !ring_k->is_hole()) {
        // Both are not holes, return nothing to do.
        return;
    }

    ring_ptr<T> ring_origin;
    ring_ptr<T> ring_search;
    ring_ptr<T> ring_parent;
    point_ptr<T> op_origin_1;
    point_ptr<T> op_origin_2;
    if (!ring_j->is_hole()) {
        ring_origin = ring_j;
        ring_parent = ring_origin;
        ring_search = ring_k;
        op_origin_1 = op_j;
        op_origin_2 = op_k;
    } else if (!ring_k->is_hole()) {
        ring_origin = ring_k;
        ring_parent = ring_origin;
        ring_search = ring_j;
        op_origin_1 = op_k;
        op_origin_2 = op_j;

    } else {
        // both are holes
        // Order doesn't matter
        ring_origin = ring_j;
        ring_parent = ring_origin->parent;
        ring_search = ring_k;
        op_origin_1 = op_j;
        op_origin_2 = op_k;
    }
    if (ring_parent != ring_search->parent) {
        // The two holes do not have the same parent, do not add them
        // simply return!
        return;
    }
    bool found = false;
    std::list<std::pair<ring_ptr<T>, point_ptr_pair<T>>> iList;
    {
        auto range = connection_map.equal_range(ring_search);
        // Check for direct connection
        for (auto& it = range.first; it != range.second;) {
            if (!it->second.op1->ring) {
                it = connection_map.erase(it);
                continue;
            }
            if (!it->second.op2->ring) {
                it = connection_map.erase(it);
                continue;
            }
            ring_ptr<T> it_ring2 = it->second.op2->ring;
            if (it_ring2 == ring_origin) {
                found = true;
                if (*op_origin_1 != *(it->second.op2)) {
                    iList.emplace_back(ring_search, it->second);
                    break;
                }
            }
            ++it;
        }
    }
    if (iList.empty()) {
        auto range = connection_map.equal_range(ring_search);
        std::set<ring_ptr<T>> visited;
        visited.insert(ring_search);
        // Check for connection through chain of other intersections
        for (auto& it = range.first;
             it != range.second && it != connection_map.end() && it->first == ring_search; ++it) {
            ring_ptr<T> it_ring = it->second.op2->ring;
            if (it_ring != ring_search && *op_origin_2 != *it->second.op2 && it_ring != nullptr &&
                (ring_parent == it_ring || ring_parent == it_ring->parent) &&
                !value_is_zero(it_ring->area()) &&
                find_intersect_loop(connection_map, iList, ring_parent, ring_origin, it_ring,
                                    visited, op_origin_2, it->second.op2, manager)) {
                found = true;
                iList.emplace_front(ring_search, it->second);
                break;
            }
        }
    }
    if (!found) {
        point_ptr_pair<T> intPt_origin = { op_origin_1, op_origin_2 };
        point_ptr_pair<T> intPt_search = { op_origin_2, op_origin_1 };
        connection_map.emplace(ring_origin, std::move(intPt_origin));
        connection_map.emplace(ring_search, std::move(intPt_search));
        return;
    }

    if (iList.empty()) {
        // The situation where both origin and search are holes might have a missing
        // search condition, we must check if a new pair must be added.
        bool missing = true;
        auto rng = connection_map.equal_range(ring_origin);
        // Check for direct connection
        for (auto& it = rng.first; it != rng.second; ++it) {
            ring_ptr<T> it_ring2 = it->second.op2->ring;
            if (it_ring2 == ring_search) {
                missing = false;
            }
        }
        if (missing) {
            point_ptr_pair<T> intPt_origin = { op_origin_1, op_origin_2 };
            connection_map.emplace(ring_origin, std::move(intPt_origin));
        }
        return;
    }
    if (ring_origin->is_hole()) {
        for (auto& iRing : iList) {
            ring_ptr<T> ring_itr = iRing.first;
            if (!ring_itr->is_hole()) {
                // Make the hole the origin!
                point_ptr<T> op1 = op_origin_1;
                op_origin_1 = iRing.second.op1;
                iRing.second.op1 = op1;
                point_ptr<T> op2 = op_origin_2;
                op_origin_2 = iRing.second.op2;
                iRing.second.op2 = op2;
                iRing.first = ring_origin;
                ring_origin = ring_itr;
                ring_parent = ring_origin;
                break;
            }
        }
    }
    bool origin_is_hole = ring_origin->is_hole();

    // Switch
    point_ptr<T> op_origin_1_next = op_origin_1->next;
    point_ptr<T> op_origin_2_next = op_origin_2->next;
    op_origin_1->next = op_origin_2_next;
    op_origin_2->next = op_origin_1_next;
    op_origin_1_next->prev = op_origin_2;
    op_origin_2_next->prev = op_origin_1;

    for (auto& iRing : iList) {
        point_ptr<T> op_search_1 = iRing.second.op1;
        point_ptr<T> op_search_2 = iRing.second.op2;
        point_ptr<T> op_search_1_next = op_search_1->next;
        point_ptr<T> op_search_2_next = op_search_2->next;
        op_search_1->next = op_search_2_next;
        op_search_2->next = op_search_1_next;
        op_search_1_next->prev = op_search_2;
        op_search_2_next->prev = op_search_1;
    }

    ring_ptr<T> ring_new = create_new_ring(manager);
    ring_origin->corrected = false;
    std::size_t size_1 = 0;
    std::size_t size_2 = 0;
    mapbox::geometry::box<T> box1({ 0, 0 }, { 0, 0 });
    mapbox::geometry::box<T> box2({ 0, 0 }, { 0, 0 });
    double area_1 = area_from_point(op_origin_1, size_1, box1);
    double area_2 = area_from_point(op_origin_2, size_2, box2);
    if (origin_is_hole && ((area_1 < 0.0))) {
        ring_origin->points = op_origin_1;
        ring_origin->set_stats(area_1, size_1, box1);
        ring_new->points = op_origin_2;
        ring_new->set_stats(area_2, size_2, box2);
    } else {
        ring_origin->points = op_origin_2;
        ring_origin->set_stats(area_2, size_2, box2);
        ring_new->points = op_origin_1;
        ring_new->set_stats(area_1, size_1, box1);
    }

    update_points_ring(ring_origin);
    update_points_ring(ring_new);

    ring_origin->bottom_point = nullptr;

    for (auto& iRing : iList) {
        ring_ptr<T> ring_itr = iRing.first;
        ring_itr->bottom_point = nullptr;
        if (origin_is_hole) {
            ring1_replaces_ring2(ring_origin, ring_itr, manager);
        } else {
            ring1_replaces_ring2(ring_origin->parent, ring_itr, manager);
        }
    }
    if (origin_is_hole) {
        assign_as_child(ring_new, ring_origin, manager);
        // The parent ring in this situation might need to give up children
        // to the new ring.
        for (auto c : ring_parent->children) {
            if (c == nullptr) {
                continue;
            }
            if (poly2_contains_poly1(c, ring_new)) {
                reassign_as_child(c, ring_new, manager);
            }
        }
    } else {
        // The new ring and the origin ring need to be siblings
        // however some children ring from the ring origin might
        // need to be re-assigned to the new ring
        assign_as_sibling(ring_new, ring_origin, manager);
        for (auto c : ring_origin->children) {
            if (c == nullptr) {
                continue;
            }
            if (poly2_contains_poly1(c, ring_new)) {
                reassign_as_child(c, ring_new, manager);
            }
        }
    }

    std::list<std::pair<ring_ptr<T>, point_ptr_pair<T>>> move_list;

    for (auto& iRing : iList) {
        auto range_itr = connection_map.equal_range(iRing.first);
        if (range_itr.first != range_itr.second) {
            for (auto& it = range_itr.first; it != range_itr.second; ++it) {
                ring_ptr<T> it_ring = it->second.op1->ring;
                ring_ptr<T> it_ring2 = it->second.op2->ring;
                if (it_ring == nullptr || it_ring2 == nullptr || it_ring == it_ring2) {
                    continue;
                }
                if (it_ring->is_hole() || it_ring2->is_hole()) {
                    move_list.emplace_back(it_ring, it->second);
                }
            }
            connection_map.erase(iRing.first);
        }
    }

    auto range_itr = connection_map.equal_range(ring_origin);
    for (auto& it = range_itr.first; it != range_itr.second;) {
        ring_ptr<T> it_ring = it->second.op1->ring;
        ring_ptr<T> it_ring2 = it->second.op2->ring;
        if (it_ring == nullptr || it_ring2 == nullptr || it_ring == it_ring2) {
            it = connection_map.erase(it);
            continue;
        }
        if (it_ring != ring_origin) {
            if (it_ring->is_hole() || it_ring2->is_hole()) {
                move_list.emplace_back(it_ring, it->second);
            }
            it = connection_map.erase(it);
        } else {
            if (it_ring->is_hole() || it_ring2->is_hole()) {
                ++it;
            } else {
                it = connection_map.erase(it);
            }
        }
    }

    if (!move_list.empty()) {
        connection_map.insert(move_list.begin(), move_list.end());
    }

    return;
}

template <typename T>
void correct_chained_repeats(
    ring_manager<T>& manager,
    std::unordered_multimap<ring_ptr<T>, point_ptr_pair<T>>& connection_map,
    point_vector_itr<T> const& begin,
    point_vector_itr<T> const& end) {
    for (auto itr1 = begin; itr1 != end; ++itr1) {
        if ((*itr1)->ring == nullptr) {
            continue;
        }
        for (auto itr2 = std::next(itr1); itr2 != end; ++itr2) {
            if ((*itr2)->ring == nullptr) {
                continue;
            }
            process_single_intersection(connection_map, *itr1, *itr2, manager);
        }
    }
}

template <typename T>
void correct_chained_rings(ring_manager<T>& manager) {

    if (manager.all_points.size() < 2) {
        return;
    }
    // Setup connection map which is a map of rings and their
    // connection point pairs with other rings.
    std::unordered_multimap<ring_ptr<T>, point_ptr_pair<T>> connection_map;
    connection_map.reserve(manager.rings.size());

    // Now lets find and process any points
    // that overlap -- we should have solved
    // all situations where these points
    // would be self intersections of a ring with
    // earlier processing so this should just be
    // points where different rings are touching.
    std::size_t count = 0;
    auto prev_itr = manager.all_points.begin();
    auto itr = std::next(prev_itr);
    while (itr != manager.all_points.end()) {
        if (*(*prev_itr) == *(*(itr))) {
            ++count;
            ++prev_itr;
            ++itr;
            if (itr != manager.all_points.end()) {
                continue;
            } else {
                ++prev_itr;
            }
        } else {
            ++prev_itr;
            ++itr;
        }
        if (count == 0) {
            continue;
        }
        auto first = prev_itr;
        std::advance(first, -(static_cast<int>(count) + 1));
        correct_chained_repeats(manager, connection_map, first, prev_itr);
        count = 0;
    }
}

template <typename T>
ring_vector<T> sort_rings_largest_to_smallest(ring_manager<T>& manager) {
    ring_vector<T> sorted_rings;
    sorted_rings.reserve(manager.rings.size());
    for (auto& r : manager.rings) {
        sorted_rings.push_back(&r);
    }
    std::stable_sort(sorted_rings.begin(), sorted_rings.end(),
                     [](ring_ptr<T> const& r1, ring_ptr<T> const& r2) {
                         if (!r1->points || !r2->points) {
                             return r1->points != nullptr;
                         }
                         return std::fabs(r1->area()) > std::fabs(r2->area());
                     });
    return sorted_rings;
}

template <typename T>
ring_vector<T> sort_rings_smallest_to_largest(ring_manager<T>& manager) {
    ring_vector<T> sorted_rings;
    sorted_rings.reserve(manager.rings.size());
    for (auto& r : manager.rings) {
        sorted_rings.push_back(&r);
    }
    std::stable_sort(sorted_rings.begin(), sorted_rings.end(),
                     [](ring_ptr<T> const& r1, ring_ptr<T> const& r2) {
                         if (!r1->points || !r2->points) {
                             return r1->points != nullptr;
                         }
                         return std::fabs(r1->area()) < std::fabs(r2->area());
                     });
    return sorted_rings;
}

template <typename T>
struct collinear_path {
    // Collinear edges are in opposite directions
    // such that start_1 is at the same position
    // of end_2 and vise versa. Start to end is
    // always forward (next) on path.
    point_ptr<T> start_1;
    point_ptr<T> end_1;
    point_ptr<T> start_2;
    point_ptr<T> end_2;
};

template <typename T>
struct collinear_result {
    point_ptr<T> pt1;
    point_ptr<T> pt2;
};

template <typename T>
collinear_result<T> fix_collinear_path(collinear_path<T>& path) {
    // Left and right are just the opposite ends of the
    // collinear path, they may not be actually left
    // and right of each other.
    // The left end is start_1 and end_2
    // The right end is start_2 and end_1

    // NOTE spike detection is checking that the
    // pointers are the same values, not position!
    // additionally duplicate points we will treat
    // if they are a spike left.
    bool spike_left = (path.start_1 == path.end_2);
    bool spike_right = (path.start_2 == path.end_1);

    if (spike_left && spike_right) {
        // If both ends are spikes we should simply
        // delete all the points. (they should be in a loop)
        point_ptr<T> itr = path.start_1;
        while (itr != nullptr) {
            itr->prev->next = nullptr;
            itr->prev = nullptr;
            itr->ring = nullptr;
            itr = itr->next;
        }
        return { nullptr, nullptr };
    } else if (spike_left) {
        point_ptr<T> prev = path.start_2->prev;
        point_ptr<T> itr = path.start_2;
        while (itr != path.end_1) {
            itr->prev->next = nullptr;
            itr->prev = nullptr;
            itr->ring = nullptr;
            itr = itr->next;
        }
        prev->next = path.end_1;
        path.end_1->prev = prev;
        return { path.end_1, nullptr };
    } else if (spike_right) {
        point_ptr<T> prev = path.start_1->prev;
        point_ptr<T> itr = path.start_1;
        while (itr != path.end_2) {
            itr->prev->next = nullptr;
            itr->prev = nullptr;
            itr->ring = nullptr;
            itr = itr->next;
        }
        prev->next = path.end_2;
        path.end_2->prev = prev;
        return { path.end_2, nullptr };
    } else {
        point_ptr<T> prev_1 = path.start_1->prev;
        point_ptr<T> prev_2 = path.start_2->prev;
        point_ptr<T> itr = path.start_1;
        do {
            itr->prev->next = nullptr;
            itr->prev = nullptr;
            itr->ring = nullptr;
            itr = itr->next;
        } while (itr != path.end_1 && itr != nullptr);
        itr = path.start_2;
        do {
            itr->prev->next = nullptr;
            itr->prev = nullptr;
            itr->ring = nullptr;
            itr = itr->next;
        } while (itr != path.end_2 && itr != nullptr);
        if (path.start_1 == path.end_1 && path.start_2 == path.end_2) {
            return { nullptr, nullptr };
        } else if (path.start_1 == path.end_1) {
            prev_2->next = path.end_2;
            path.end_2->prev = prev_2;
            return { path.end_2, nullptr };
        } else if (path.start_2 == path.end_2) {
            prev_1->next = path.end_1;
            path.end_1->prev = prev_1;
            return { path.end_1, nullptr };
        } else {
            prev_1->next = path.end_2;
            path.end_2->prev = prev_1;
            prev_2->next = path.end_1;
            path.end_1->prev = prev_2;
            return { path.end_1, path.end_2 };
        }
    }
}

template <typename T>
collinear_path<T> find_start_and_end_of_collinear_edges(point_ptr<T> pt_a, point_ptr<T> pt_b) {
    // Search backward on A, forwards on B first
    bool same_ring = pt_a->ring == pt_b->ring;
    point_ptr<T> back = pt_a;
    point_ptr<T> forward = pt_b;
    bool first = true;
    do {
        while (*(back->prev) == *back && back != forward) {
            back = back->prev;
            if (back == pt_a) {
                break;
            }
        }
        if (back == forward) {
            back = back->prev;
            forward = forward->next;
            break;
        }
        while (*(forward->next) == *forward && back != forward) {
            forward = forward->next;
            if (forward == pt_b) {
                break;
            }
        }
        if (!first && (back == pt_a || forward == pt_b)) {
            break;
        }
        if (back == forward) {
            back = back->prev;
            forward = forward->next;
            break;
        }
        back = back->prev;
        forward = forward->next;
        first = false;
    } while (*back == *forward);
    point_ptr<T> start_a = back->next;
    // If there are repeated points at the diverge we want to select
    // only the first of those repeated points.
    while (!same_ring && *start_a == *start_a->next && start_a != pt_a) {
        start_a = start_a->next;
    }
    point_ptr<T> end_b = forward->prev;
    while (!same_ring && *end_b == *end_b->prev && end_b != pt_b) {
        end_b = end_b->prev;
    }
    // Search backward on B, forwards on A next
    back = pt_b;
    forward = pt_a;
    first = true;
    do {
        while (*(back->prev) == *back && back != forward) {
            back = back->prev;
            if (back == pt_b) {
                break;
            }
        }
        if (back == forward) {
            back = back->prev;
            forward = forward->next;
            break;
        }
        while (*(forward->next) == *forward && back != forward) {
            forward = forward->next;
            if (forward == pt_a) {
                break;
            }
        }
        if (!first && (back == pt_b || forward == pt_a)) {
            break;
        }
        if (back == forward || (!first && (back == end_b || forward == start_a))) {
            back = back->prev;
            forward = forward->next;
            break;
        }
        back = back->prev;
        forward = forward->next;
        first = false;
    } while (*back == *forward);
    point_ptr<T> start_b = back->next;
    while (!same_ring && *start_b == *start_b->next && start_b != pt_b) {
        start_b = start_b->next;
    }
    point_ptr<T> end_a = forward->prev;
    while (!same_ring && *end_a == *end_a->prev && end_a != pt_a) {
        end_a = end_a->prev;
    }
    return { start_a, end_a, start_b, end_b };
}

template <typename T>
bool has_collinear_edge(point_ptr<T> pt_a, point_ptr<T> pt_b) {
    // It is assumed pt_a and pt_b are at the same location.
    return (*pt_a->next == *pt_b->prev || *pt_b->next == *pt_a->prev);
}

template <typename T>
void process_collinear_edges_same_ring(point_ptr<T> pt_a,
                                       point_ptr<T> pt_b,
                                       ring_manager<T>& manager) {
    ring_ptr<T> original_ring = pt_a->ring;
    // As they are the same ring that are forming a collinear edge
    // we should expect the creation of two different rings.
    auto path = find_start_and_end_of_collinear_edges(pt_a, pt_b);
    auto results = fix_collinear_path(path);
    if (results.pt1 == nullptr) {
        // If pt1 is a nullptr, both values
        // are nullptrs. This mean the ring was
        // removed during this processing.
        remove_ring(original_ring, manager, false);
    } else if (results.pt2 == nullptr) {
        // If a single point is only returned, we simply removed a spike.
        // In this case, we don't need to worry about parent or children
        // and we simply need to set the points and clear the area
        original_ring->points = results.pt1;
        original_ring->recalculate_stats();
    } else {
        // If we have two seperate points, the ring has split into
        // two different rings.
        ring_ptr<T> ring_new = create_new_ring(manager);
        ring_new->points = results.pt2;
        ring_new->recalculate_stats();
        update_points_ring(ring_new);
        original_ring->points = results.pt1;
        original_ring->recalculate_stats();
    }
}

template <typename T>
void process_collinear_edges_different_rings(point_ptr<T> pt_a,
                                             point_ptr<T> pt_b,
                                             ring_manager<T>& manager) {
    ring_ptr<T> ring_a = pt_a->ring;
    ring_ptr<T> ring_b = pt_b->ring;
    bool ring_a_larger = std::fabs(ring_a->area()) > std::fabs(ring_b->area());
    auto path = find_start_and_end_of_collinear_edges(pt_a, pt_b);
    // This should result in two rings becoming one.
    auto results = fix_collinear_path(path);
    if (results.pt1 == nullptr) {
        remove_ring(ring_a, manager, false);
        remove_ring(ring_b, manager, false);
        return;
    }
    // Rings should merge into a single ring of the same orientation.
    // Therefore, we we will need to replace one ring with the other
    ring_ptr<T> merged_ring = ring_a_larger ? ring_a : ring_b;
    ring_ptr<T> deleted_ring = ring_a_larger ? ring_b : ring_a;

    merged_ring->points = results.pt1;
    update_points_ring(merged_ring);
    merged_ring->recalculate_stats();
    if (merged_ring->size() < 3) {
        remove_ring_and_points(merged_ring, manager, false);
    }
    remove_ring(deleted_ring, manager, false);
}

template <typename T>
bool remove_duplicate_points(point_ptr<T> pt_a, point_ptr<T> pt_b, ring_manager<T>& manager) {
    if (pt_a->ring == pt_b->ring) {
        if (pt_a->next == pt_b) {
            pt_a->next = pt_b->next;
            pt_a->next->prev = pt_a;
            pt_b->next = nullptr;
            pt_b->prev = nullptr;
            pt_b->ring = nullptr;
            if (pt_a->ring->points == pt_b) {
                pt_a->ring->points = pt_a;
            }
            return true;
        } else if (pt_b->next == pt_a) {
            pt_a->prev = pt_b->prev;
            pt_a->prev->next = pt_a;
            pt_b->next = nullptr;
            pt_b->prev = nullptr;
            pt_b->ring = nullptr;
            if (pt_a->ring->points == pt_b) {
                pt_a->ring->points = pt_a;
            }
            return true;
        }
    }
    while (*pt_a->next == *pt_a && pt_a->next != pt_a) {
        point_ptr<T> remove = pt_a->next;
        pt_a->next = remove->next;
        pt_a->next->prev = pt_a;
        remove->next = nullptr;
        remove->prev = nullptr;
        remove->ring = nullptr;
        if (pt_a->ring->points == remove) {
            pt_a->ring->points = pt_a;
        }
    }
    while (*pt_a->prev == *pt_a && pt_a->prev != pt_a) {
        point_ptr<T> remove = pt_a->prev;
        pt_a->prev = remove->prev;
        pt_a->prev->next = pt_a;
        remove->next = nullptr;
        remove->prev = nullptr;
        remove->ring = nullptr;
        if (pt_a->ring->points == remove) {
            pt_a->ring->points = pt_a;
        }
    }
    if (pt_a->next == pt_a) {
        remove_ring_and_points(pt_a->ring, manager, false);
        return true;
    }
    if (pt_b->ring == nullptr) {
        return true;
    }
    while (*pt_b->next == *pt_b && pt_b->next != pt_b) {
        point_ptr<T> remove = pt_b->next;
        pt_b->next = remove->next;
        pt_b->next->prev = pt_b;
        remove->next = nullptr;
        remove->prev = nullptr;
        remove->ring = nullptr;
        if (pt_b->ring->points == remove) {
            pt_b->ring->points = pt_b;
        }
    }
    while (*pt_b->prev == *pt_b && pt_b->prev != pt_b) {
        point_ptr<T> remove = pt_b->prev;
        pt_b->prev = remove->prev;
        pt_b->prev->next = pt_b;
        remove->next = nullptr;
        remove->prev = nullptr;
        remove->ring = nullptr;
        if (pt_b->ring->points == remove) {
            pt_b->ring->points = pt_b;
        }
    }
    if (pt_b->next == pt_b) {
        remove_ring_and_points(pt_b->ring, manager, false);
        return true;
    }
    if (pt_a->ring == nullptr) {
        return true;
    }
    return false;
}

template <typename T>
bool process_collinear_edges(point_ptr<T> pt_a, point_ptr<T> pt_b, ring_manager<T>& manager) {
    // Neither point assigned to a ring (deleted points)
    if (!pt_a->ring || !pt_b->ring) {
        return false;
    }

    if (remove_duplicate_points(pt_a, pt_b, manager)) {
        return true;
    }

    if (!has_collinear_edge(pt_a, pt_b)) {
        if (pt_a->ring == pt_b->ring) {
            correct_self_intersection(pt_a, pt_b, manager);
            return true;
        }
        return false;
    }

    if (pt_a->ring == pt_b->ring) {
        process_collinear_edges_same_ring(pt_a, pt_b, manager);
    } else {
        process_collinear_edges_different_rings(pt_a, pt_b, manager);
    }
    return true;
}

template <typename T>
void correct_collinear_repeats(ring_manager<T>& manager,
                               point_vector_itr<T> const& begin,
                               point_vector_itr<T> const& end) {
    for (auto itr1 = begin; itr1 != end; ++itr1) {
        if ((*itr1)->ring == nullptr) {
            continue;
        }
        for (auto itr2 = begin; itr2 != end;) {
            if ((*itr1)->ring == nullptr) {
                break;
            }
            if ((*itr2)->ring == nullptr || *itr2 == *itr1) {
                ++itr2;
                continue;
            }
            if (process_collinear_edges(*itr1, *itr2, manager)) {
                itr2 = begin;
            } else {
                ++itr2;
            }
        }
    }
}

template <typename T>
void correct_collinear_edges(ring_manager<T>& manager) {

    if (manager.all_points.size() < 2) {
        return;
    }
    std::size_t count = 0;
    auto prev_itr = manager.all_points.begin();
    auto itr = std::next(prev_itr);
    while (itr != manager.all_points.end()) {
        if (*(*prev_itr) == *(*(itr))) {
            ++count;
            ++prev_itr;
            ++itr;
            if (itr != manager.all_points.end()) {
                continue;
            } else {
                ++prev_itr;
            }
        } else {
            ++prev_itr;
            ++itr;
        }
        if (count == 0) {
            continue;
        }
        auto first = prev_itr;
        std::advance(first, -(static_cast<int>(count) + 1));
        correct_collinear_repeats(manager, first, prev_itr);
        count = 0;
    }
}

template <typename T>
void correct_tree(ring_manager<T>& manager) {

    // It is possible that vatti resulted in some parent child
    // relationships that are not quite right, therefore, we
    // need to just rebuild the entire tree of rings

    // First lets process the rings in order of size from largest
    // area to smallest, we know right away that no smaller ring could ever
    // contain a larger ring so we can use this to our advantage
    // as we iterate over the rings.
    using rev_itr = typename ring_vector<T>::reverse_iterator;
    ring_vector<T> sorted_rings = sort_rings_largest_to_smallest(manager);
    for (auto itr = sorted_rings.begin(); itr != sorted_rings.end(); ++itr) {
        if ((*itr)->points == nullptr) {
            continue;
        }
        if ((*itr)->size() < 3 || value_is_zero((*itr)->area())) {
            remove_ring_and_points(*itr, manager, false);
            continue;
        }
        (*itr)->corrected = true;
        bool found = false;
        // Search in reverse from the current iterator back to the begining
        // to see if any of those rings might be its parent.
        for (auto r_itr = rev_itr(itr); r_itr != sorted_rings.rend(); ++r_itr) {
            // If orientations are not different, this can't be its parent.
            if ((*r_itr)->is_hole() == (*itr)->is_hole()) {
                continue;
            }
            if (poly2_contains_poly1(*itr, *r_itr)) {
                reassign_as_child(*itr, *r_itr, manager);
                found = true;
                break;
            }
        }
        if (!found) {
            if ((*itr)->is_hole()) {
                throw std::runtime_error("Could not properly place hole to a parent.");
            } else {
                // Assign to base of tree by passing nullptr
                reassign_as_child(*itr, static_cast<ring_ptr<T>>(nullptr), manager);
            }
        }
    }
}

template <typename T>
bool correct_self_intersections(ring_manager<T>& manager, bool correct_tree) {
    bool fixed_intersections = false;
    auto sorted_rings = sort_rings_smallest_to_largest(manager);
    for (auto const& r : sorted_rings) {
        if (correct_ring_self_intersections(manager, r, correct_tree)) {
            fixed_intersections = true;
        }
    }
    return fixed_intersections;
}

template <typename T>
void correct_topology(ring_manager<T>& manager) {

    // Sort all the points, this will be used for the locating of chained rings
    // and the collinear edges and only needs to be done once.
    std::stable_sort(manager.all_points.begin(), manager.all_points.end(), point_ptr_cmp<T>());

    // Initially the orientations of the rings
    // could be incorrect, we need to adjust them
    correct_orientations(manager);

    // We should only have to fix collinear edges once.
    // During this we also correct self intersections
    correct_collinear_edges(manager);

    correct_self_intersections(manager, false);

    correct_tree(manager);

    bool fixed_intersections = true;
    while (fixed_intersections) {
        correct_chained_rings(manager);
        fixed_intersections = correct_self_intersections(manager, true);
    }
}
}
}
}
