#pragma once

#include <mapbox/geometry/wagyu/ring.hpp>
#include <mapbox/geometry/wagyu/ring_util.hpp>

#include <mapbox/geometry/multi_polygon.hpp>

namespace mapbox {
namespace geometry {
namespace wagyu {

template <typename T1, typename T2>
void push_ring_to_polygon(mapbox::geometry::polygon<T2>& poly,
                          ring_ptr<T1> r,
                          bool reverse_output) {
    mapbox::geometry::linear_ring<T2> lr;
    lr.reserve(r->size() + 1);
    auto firstPt = r->points;
    auto ptIt = r->points;
    if (reverse_output) {
        do {
            lr.emplace_back(static_cast<T2>(ptIt->x), static_cast<T2>(ptIt->y));
            ptIt = ptIt->next;
        } while (ptIt != firstPt);
    } else {
        do {
            lr.emplace_back(static_cast<T2>(ptIt->x), static_cast<T2>(ptIt->y));
            ptIt = ptIt->prev;
        } while (ptIt != firstPt);
    }
    lr.emplace_back(firstPt->x, firstPt->y); // close the ring
    poly.push_back(lr);
}

template <typename T1, typename T2>
void build_result_polygons(mapbox::geometry::multi_polygon<T2>& solution,
                           ring_vector<T1>const& rings,
                           bool reverse_output) {
    for (auto r : rings) {
        if (r == nullptr) {
            continue;
        }
        assert(r->points);
        solution.emplace_back();
        push_ring_to_polygon(solution.back(), r, reverse_output);
        for (auto c : r->children) {
            if (c == nullptr) {
                continue;
            }
            assert(c->points);
            push_ring_to_polygon(solution.back(), c, reverse_output);
        }
        for (auto c : r->children) {
            if (c == nullptr) {
                continue;
            }
            if (!c->children.empty()) {
                build_result_polygons(solution, c->children, reverse_output);
            }
        }
    }
}

template <typename T1, typename T2>
void build_result(mapbox::geometry::multi_polygon<T2>& solution,
                  ring_manager<T1>const& rings,
                  bool reverse_output) {
    build_result_polygons(solution, rings.children, reverse_output);
}
}
}
}
