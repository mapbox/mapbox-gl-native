#pragma once

#include <mapbox/geometry/line_string.hpp>
#include <mapbox/geometry/point.hpp>
#include <mapbox/geometry/polygon.hpp>

#include <mapbox/geometry/wagyu/config.hpp>
#include <mapbox/geometry/wagyu/edge.hpp>
#include <mapbox/geometry/wagyu/util.hpp>

namespace mapbox {
namespace geometry {
namespace wagyu {

template <typename T>
bool point_2_is_between_point_1_and_point_3(mapbox::geometry::point<T> const& pt1,
                                            mapbox::geometry::point<T> const& pt2,
                                            mapbox::geometry::point<T> const& pt3) {
    if ((pt1 == pt3) || (pt1 == pt2) || (pt3 == pt2)) {
        return false;
    } else if (pt1.x != pt3.x) {
        return (pt2.x > pt1.x) == (pt2.x < pt3.x);
    } else {
        return (pt2.y > pt1.y) == (pt2.y < pt3.y);
    }
}

template <typename T1, typename T2>
bool build_edge_list(mapbox::geometry::linear_ring<T2> const& path_geometry, edge_list<T1>& edges) {

    if (path_geometry.size() < 3) {
        return false;
    }

    // As this is a loop, we need to first go backwards from end to try and find
    // the proper starting point for the iterators before the beginning

    auto itr_rev = path_geometry.rbegin();
    auto itr = path_geometry.begin();
    mapbox::geometry::point<T2> pt1 = *itr_rev;
    mapbox::geometry::point<T2> pt2 = *itr;

    // Find next non repeated point going backwards from
    // end for pt1
    while (pt1 == pt2) {
        ++itr_rev;
        if (itr_rev == path_geometry.rend()) {
            return false;
        }
        pt1 = *itr_rev;
    }
    ++itr;
    mapbox::geometry::point<T2> pt3 = *itr;
    auto itr_last = itr_rev.base();
    mapbox::geometry::point<T2> front_pt;
    mapbox::geometry::point<T2> back_pt;
    while (true) {
        if (pt3 == pt2) {
            // Duplicate point advance itr, but do not
            // advance other points
            if (itr == itr_last) {
                break;
            }
            ++itr;
            if (itr == itr_last) {
                if (edges.empty()) {
                    break;
                }
                pt3 = front_pt;
            } else {
                pt3 = *itr;
            }
            continue;
        }

        // Now check if slopes are equal between two segments - either
        // a spike or a collinear point - if so drop point number 2.
        if (slopes_equal(pt1, pt2, pt3)) {
            // We need to reconsider previously added points
            // because the point it was using was found to be collinear
            // or a spike
            pt2 = pt1;
            if (!edges.empty()) {
                edges.pop_back(); // remove previous edge (pt1)
            }
            if (!edges.empty()) {
                auto const& back_top = edges.back().top;
                if (static_cast<T1>(back_pt.x) == back_top.x &&
                    static_cast<T1>(back_pt.y) == back_top.y) {
                    auto const& back_bot = edges.back().bot;
                    pt1 = mapbox::geometry::point<T2>(static_cast<T2>(back_bot.x),
                                                      static_cast<T2>(back_bot.y));
                } else {
                    pt1 = mapbox::geometry::point<T2>(static_cast<T2>(back_top.x),
                                                      static_cast<T2>(back_top.y));
                }
                back_pt = pt1;
            } else {
                // If this occurs we must look to the back of the
                // ring for new points.
                while (*itr_rev == pt2) {
                    ++itr_rev;
                    if ((itr + 1) == itr_rev.base()) {
                        return false;
                    }
                }
                pt1 = *itr_rev;
                itr_last = itr_rev.base();
            }
            continue;
        }

        if (edges.empty()) {
            front_pt = pt2;
        }
        edges.emplace_back(pt2, pt3);
        back_pt = pt2;
        if (itr == itr_last) {
            break;
        }
        pt1 = pt2;
        pt2 = pt3;
        ++itr;
        if (itr == itr_last) {
            if (edges.empty()) {
                break;
            }
            pt3 = front_pt;
        } else {
            pt3 = *itr;
        }
    }

    bool modified = false;
    do {
        modified = false;
        if (edges.size() < 3) {
            return false;
        }
        auto& f = edges.front();
        auto& b = edges.back();
        if (slopes_equal(f, b)) {
            if (f.bot == b.top) {
                if (f.top == b.bot) {
                    edges.pop_back();
                    edges.erase(edges.begin());
                } else {
                    f.bot = b.bot;
                    edges.pop_back();
                }
                modified = true;
            } else if (f.top == b.bot) {
                f.top = b.top;
                edges.pop_back();
                modified = true;
            } else if (f.top == b.top && f.bot == b.bot) {
                edges.pop_back();
                edges.erase(edges.begin());
                modified = true;
            } else if (f.top == b.top) {
                if (point_2_is_between_point_1_and_point_3(f.top, f.bot, b.bot)) {
                    b.top = f.bot;
                    edges.erase(edges.begin());
                } else {
                    f.top = b.bot;
                    edges.pop_back();
                }
                modified = true;
            } else if (f.bot == b.bot) {
                if (point_2_is_between_point_1_and_point_3(f.bot, f.top, b.top)) {
                    b.bot = f.top;
                    edges.erase(edges.begin());
                } else {
                    f.bot = b.top;
                    edges.pop_back();
                }
                modified = true;
            }
        }
    } while (modified);

    return true;
}
}
}
}
