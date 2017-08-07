#pragma once

#include <mapbox/geojsonvt/simplify.hpp>
#include <mapbox/geojsonvt/types.hpp>
#include <mapbox/geometry.hpp>

#include <algorithm>
#include <cmath>

namespace mapbox {
namespace geojsonvt {
namespace detail {

struct project {
    const double tolerance;
    using result_type = vt_geometry;

    vt_point operator()(const geometry::point<double>& p) {
        const double sine = std::sin(p.y * M_PI / 180);
        const double x = p.x / 360 + 0.5;
        const double y =
            std::max(std::min(0.5 - 0.25 * std::log((1 + sine) / (1 - sine)) / M_PI, 1.0), 0.0);
        return { x, y, 0.0 };
    }

    vt_line_string operator()(const geometry::line_string<double>& points) {
        vt_line_string result;
        const size_t len = points.size();

        if (len == 0)
            return result;

        result.reserve(len);

        for (const auto& p : points) {
            result.push_back(operator()(p));
        }

        for (size_t i = 0; i < len - 1; ++i) {
            const auto& a = result[i];
            const auto& b = result[i + 1];
            // use Manhattan distance instead of Euclidian to avoid expensive square root
            // computation
            result.dist += std::abs(b.x - a.x) + std::abs(b.y - a.y);
        }

        simplify(result, tolerance);

        return result;
    }

    vt_linear_ring operator()(const geometry::linear_ring<double>& ring) {
        vt_linear_ring result;
        const size_t len = ring.size();

        if (len == 0)
            return result;

        result.reserve(len);

        for (const auto& p : ring) {
            result.push_back(operator()(p));
        }

        double area = 0.0;

        for (size_t i = 0; i < len - 1; ++i) {
            const auto& a = result[i];
            const auto& b = result[i + 1];
            area += a.x * b.y - b.x * a.y;
        }
        result.area = std::abs(area / 2);

        simplify(result, tolerance);

        return result;
    }

    vt_geometry operator()(const geometry::geometry<double>& geometry) {
        return geometry::geometry<double>::visit(geometry, project{ tolerance });
    }

    // Handles polygon, multi_*, geometry_collection.
    template <class T>
    auto operator()(const T& vector) {
        typename vt_geometry_type<T>::type result;
        result.reserve(vector.size());
        for (const auto& e : vector) {
            result.push_back(operator()(e));
        }
        return result;
    }
};

inline vt_features convert(const geometry::feature_collection<double>& features,
                           const double tolerance) {
    vt_features projected;
    projected.reserve(features.size());
    for (const auto& feature : features) {
        projected.emplace_back(
            geometry::geometry<double>::visit(feature.geometry, project{ tolerance }),
            feature.properties,
            feature.id);
    }
    return projected;
}

} // namespace detail
} // namespace geojsonvt
} // namespace mapbox
