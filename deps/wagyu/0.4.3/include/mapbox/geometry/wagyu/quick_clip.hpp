#pragma once

#include <mapbox/geometry/box.hpp>
#include <mapbox/geometry/multi_polygon.hpp>
#include <mapbox/geometry/polygon.hpp>
#include <mapbox/geometry/wagyu/wagyu.hpp>

namespace mapbox {
namespace geometry {
namespace wagyu {
namespace quick_clip {

template <typename T>
mapbox::geometry::point<T> intersect(mapbox::geometry::point<T> a,
                                     mapbox::geometry::point<T> b,
                                     size_t edge,
                                     mapbox::geometry::box<T> const& box) {
    switch (edge) {
    case 0:
        return mapbox::geometry::point<T>(
            mapbox::geometry::wagyu::wround<T>(static_cast<double>(a.x) + static_cast<double>(b.x - a.x) * static_cast<double>(box.min.y - a.y) / static_cast<double>(b.y - a.y)),
            box.min.y);

    case 1:
        return mapbox::geometry::point<T>(
            box.max.x,
            mapbox::geometry::wagyu::wround<T>(static_cast<double>(a.y) + static_cast<double>(b.y - a.y) * static_cast<double>(box.max.x - a.x) / static_cast<double>(b.x - a.x)));

    case 2:
        return mapbox::geometry::point<T>(
            mapbox::geometry::wagyu::wround<T>(static_cast<double>(a.x) + static_cast<double>(b.x - a.x) * static_cast<double>(box.max.y - a.y) / static_cast<double>(b.y - a.y)),
            box.max.y);

    default: // case 3
        return mapbox::geometry::point<T>(
            box.min.x,
            mapbox::geometry::wagyu::wround<T>(static_cast<double>(a.y) + static_cast<double>(b.y - a.y) * static_cast<double>(box.min.x - a.x) / static_cast<double>(b.x - a.x)));
    }
}

template <typename T>
bool inside(mapbox::geometry::point<T> p, size_t edge, mapbox::geometry::box<T> const& b) {
    switch (edge) {
    case 0:
        return p.y > b.min.y;

    case 1:
        return p.x < b.max.x;

    case 2:
        return p.y < b.max.y;

    default: // case 3
        return p.x > b.min.x;
    }
}

template <typename T>
mapbox::geometry::linear_ring<T> quick_lr_clip(mapbox::geometry::linear_ring<T> const& ring,
                                               mapbox::geometry::box<T> const& b) {
    mapbox::geometry::linear_ring<T> out = ring;

    for (size_t edge = 0; edge < 4; edge++) {
        if (out.size() > 0) {
            mapbox::geometry::linear_ring<T> in = out;
            mapbox::geometry::point<T> S = in[in.size() - 1];
            out.resize(0);

            for (size_t e = 0; e < in.size(); e++) {
                mapbox::geometry::point<T> E = in[e];

                if (inside(E, edge, b)) {
                    if (!inside(S, edge, b)) {
                        out.push_back(intersect(S, E, edge, b));
                    }
                    out.push_back(E);
                } else if (inside(S, edge, b)) {
                    out.push_back(intersect(S, E, edge, b));
                }

                S = E;
            }
        }
    }

    if (out.size() < 3) {
        out.clear();
        return out;
    }
    // Close the ring if the first/last point was outside
    if (out[0] != out[out.size() - 1]) {
        out.push_back(out[0]);
    }
    return out;
}
}

template <typename T>
mapbox::geometry::multi_polygon<T> clip(mapbox::geometry::polygon<T> const& poly,
                                        mapbox::geometry::box<T> const& b,
                                        fill_type subject_fill_type) {
    mapbox::geometry::multi_polygon<T> result;
    wagyu<T> clipper;
    for (auto const& lr : poly) {
        auto new_lr = quick_clip::quick_lr_clip(lr, b);
        if (!new_lr.empty()) {
            clipper.add_ring(new_lr, polygon_type_subject);
        }
    }
    clipper.execute(clip_type_union, result, subject_fill_type, fill_type_even_odd);
    return result;
}

template <typename T>
mapbox::geometry::multi_polygon<T> clip(mapbox::geometry::multi_polygon<T> const& mp,
                                        mapbox::geometry::box<T> const& b,
                                        fill_type subject_fill_type) {
    mapbox::geometry::multi_polygon<T> result;
    wagyu<T> clipper;
    for (auto const& poly : mp) {
        for (auto const& lr : poly) {
            auto new_lr = quick_clip::quick_lr_clip(lr, b);
            if (!new_lr.empty()) {
                clipper.add_ring(new_lr, polygon_type_subject);
            }
        }
    }
    clipper.execute(clip_type_union, result, subject_fill_type, fill_type_even_odd);
    return result;
}
}
}
}
