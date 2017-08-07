#pragma once

#include <mapbox/geojsonvt/types.hpp>

namespace mapbox {
namespace geojsonvt {
namespace detail {

template <uint8_t I>
class clipper {
public:
    const double k1;
    const double k2;

    vt_geometry operator()(const vt_point& point) const {
        return point;
    }

    vt_geometry operator()(const vt_multi_point& points) const {
        vt_multi_point part;
        for (const auto& p : points) {
            const double ak = get<I>(p);
            if (ak >= k1 && ak <= k2)
                part.push_back(p);
        }
        return part;
    }

    vt_geometry operator()(const vt_line_string& line) const {
        vt_multi_line_string parts;
        clipLine(line, parts);
        if (parts.size() == 1)
            return parts[0];
        else
            return parts;
    }

    vt_geometry operator()(const vt_multi_line_string& lines) const {
        vt_multi_line_string parts;
        for (const auto& line : lines) {
            clipLine(line, parts);
        }
        if (parts.size() == 1)
            return parts[0];
        else
            return parts;
    }

    vt_geometry operator()(const vt_polygon& polygon) const {
        vt_polygon result;
        for (const auto& ring : polygon) {
            const auto new_ring = clipRing(ring);
            if (!new_ring.empty())
                result.push_back(new_ring);
        }
        return result;
    }

    vt_geometry operator()(const vt_multi_polygon& polygons) const {
        vt_multi_polygon result;
        for (const auto& polygon : polygons) {
            vt_polygon p;
            for (const auto& ring : polygon) {
                const auto new_ring = clipRing(ring);
                if (!new_ring.empty())
                    p.push_back(new_ring);
            }
            if (!p.empty())
                result.push_back(p);
        }
        return result;
    }

    vt_geometry operator()(const vt_geometry_collection& geometries) const {
        vt_geometry_collection result;
        for (const auto& geometry : geometries) {
            vt_geometry::visit(geometry,
                               [&](const auto& g) { result.push_back(this->operator()(g)); });
        }
        return result;
    }

private:
    vt_line_string newSlice(vt_multi_line_string& parts, vt_line_string& slice, double dist) const {
        if (!slice.empty()) {
            slice.dist = dist;
            parts.push_back(std::move(slice));
        }
        return {};
    }

    void clipLine(const vt_line_string& line, vt_multi_line_string& slices) const {

        const double dist = line.dist;
        const size_t len = line.size();

        if (len < 2)
            return;

        vt_line_string slice;

        for (size_t i = 0; i < (len - 1); ++i) {
            const auto& a = line[i];
            const auto& b = line[i + 1];
            const double ak = get<I>(a);
            const double bk = get<I>(b);

            if (ak < k1) {
                if (bk > k2) { // ---|-----|-->
                    slice.push_back(intersect<I>(a, b, k1));
                    slice.push_back(intersect<I>(a, b, k2));
                    slice = newSlice(slices, slice, dist);

                } else if (bk >= k1) { // ---|-->  |
                    slice.push_back(intersect<I>(a, b, k1));
                    if (i == len - 2)
                        slice.push_back(b); // last point
                }
            } else if (ak > k2) {
                if (bk < k1) { // <--|-----|---
                    slice.push_back(intersect<I>(a, b, k2));
                    slice.push_back(intersect<I>(a, b, k1));
                    slice = newSlice(slices, slice, dist);

                } else if (bk <= k2) { // |  <--|---
                    slice.push_back(intersect<I>(a, b, k2));
                    if (i == len - 2)
                        slice.push_back(b); // last point
                }
            } else {
                slice.push_back(a);

                if (bk < k1) { // <--|---  |
                    slice.push_back(intersect<I>(a, b, k1));
                    slice = newSlice(slices, slice, dist);

                } else if (bk > k2) { // |  ---|-->
                    slice.push_back(intersect<I>(a, b, k2));
                    slice = newSlice(slices, slice, dist);

                } else if (i == len - 2) { // | --> |
                    slice.push_back(b);
                }
            }
        }

        // add the final slice
        newSlice(slices, slice, dist);
    }

    vt_linear_ring clipRing(const vt_linear_ring& ring) const {
        const size_t len = ring.size();

        vt_linear_ring slice;
        slice.area = ring.area;

        if (len < 2)
            return slice;

        for (size_t i = 0; i < (len - 1); ++i) {
            const auto& a = ring[i];
            const auto& b = ring[i + 1];
            const double ak = get<I>(a);
            const double bk = get<I>(b);

            if (ak < k1) {
                if (bk >= k1) {
                    slice.push_back(intersect<I>(a, b, k1)); // ---|-->  |
                    if (bk > k2)                             // ---|-----|-->
                        slice.push_back(intersect<I>(a, b, k2));
                    else if (i == len - 2)
                        slice.push_back(b); // last point
                }
            } else if (ak > k2) {
                if (bk <= k2) { // |  <--|---
                    slice.push_back(intersect<I>(a, b, k2));
                    if (bk < k1) // <--|-----|---
                        slice.push_back(intersect<I>(a, b, k1));
                    else if (i == len - 2)
                        slice.push_back(b); // last point
                }
            } else {
                slice.push_back(a);
                if (bk < k1) // <--|---  |
                    slice.push_back(intersect<I>(a, b, k1));
                else if (bk > k2) // |  ---|-->
                    slice.push_back(intersect<I>(a, b, k2));
                // | --> |
            }
        }

        // close the polygon if its endpoints are not the same after clipping
        if (!slice.empty()) {
            const auto& first = slice.front();
            const auto& last = slice.back();
            if (first != last) {
                slice.push_back(first);
            }
        }

        return slice;
    }
};

/* clip features between two axis-parallel lines:
 *     |        |
 *  ___|___     |     /
 * /   |   \____|____/
 *     |        |
 */

template <uint8_t I>
inline vt_features clip(const vt_features& features,
                        const double k1,
                        const double k2,
                        const double minAll,
                        const double maxAll) {

    if (minAll >= k1 && maxAll <= k2) // trivial accept
        return features;

    if (minAll > k2 || maxAll < k1) // trivial reject
        return {};

    vt_features clipped;

    for (const auto& feature : features) {
        const auto& geom = feature.geometry;
        const auto& props = feature.properties;
        const auto& id = feature.id;

        const double min = get<I>(feature.bbox.min);
        const double max = get<I>(feature.bbox.max);

        if (min >= k1 && max <= k2) { // trivial accept
            clipped.push_back(feature);

        } else if (min > k2 || max < k1) { // trivial reject
            continue;

        } else {
            clipped.emplace_back(vt_geometry::visit(geom, clipper<I>{ k1, k2 }), props, id);
        }
    }

    return clipped;
}

} // namespace detail
} // namespace geojsonvt
} // namespace mapbox
