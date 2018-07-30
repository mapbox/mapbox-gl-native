#pragma once

#include <algorithm>
#include <cmath>
#include <mapbox/geojsonvt/types.hpp>

namespace mapbox {
namespace geojsonvt {

struct Tile {
    mapbox::geometry::feature_collection<int16_t> features;
    uint32_t num_points = 0;
    uint32_t num_simplified = 0;
};

namespace detail {

class InternalTile {
public:
    const uint16_t extent;
    const uint8_t z;
    const uint32_t x;
    const uint32_t y;

    const double z2;
    const double tolerance;
    const double sq_tolerance;

    vt_features source_features;
    mapbox::geometry::box<double> bbox = { { 2, 1 }, { -1, 0 } };

    Tile tile;

    InternalTile(const vt_features& source,
                 const uint8_t z_,
                 const uint32_t x_,
                 const uint32_t y_,
                 const uint16_t extent_,
                 const double tolerance_)
        : extent(extent_),
          z(z_),
          x(x_),
          y(y_),
          z2(std::pow(2, z)),
          tolerance(tolerance_),
          sq_tolerance(tolerance_ * tolerance_) {

        for (const auto& feature : source) {
            const auto& geom = feature.geometry;
            const auto& props = feature.properties;
            const auto& id = feature.id;

            tile.num_points += feature.num_points;

            vt_geometry::visit(geom, [&](const auto& g) {
                // `this->` is a workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=61636
                this->addFeature(g, props, id);
            });

            bbox.min.x = std::min(feature.bbox.min.x, bbox.min.x);
            bbox.min.y = std::min(feature.bbox.min.y, bbox.min.y);
            bbox.max.x = std::max(feature.bbox.max.x, bbox.max.x);
            bbox.max.y = std::max(feature.bbox.max.y, bbox.max.y);
        }
    }

private:
    void
    addFeature(const vt_point& point, const property_map& props, const optional<identifier>& id) {
        tile.features.push_back({ transform(point), props, id });
    }

    void addFeature(const vt_line_string& line,
                    const property_map& props,
                    const optional<identifier>& id) {
        const auto new_line = transform(line);
        if (!new_line.empty())
            tile.features.push_back({ std::move(new_line), props, id });
    }

    void addFeature(const vt_polygon& polygon,
                    const property_map& props,
                    const optional<identifier>& id) {
        const auto new_polygon = transform(polygon);
        if (!new_polygon.empty())
            tile.features.push_back({ std::move(new_polygon), props, id });
    }

    void addFeature(const vt_geometry_collection& collection,
                    const property_map& props,
                    const optional<identifier>& id) {
        for (const auto& geom : collection) {
            vt_geometry::visit(geom, [&](const auto& g) {
                // `this->` is a workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=61636
                this->addFeature(g, props, id);
            });
        }
    }

    template <class T>
    void addFeature(const T& multi, const property_map& props, const optional<identifier>& id) {
        const auto new_multi = transform(multi);

        switch (new_multi.size()) {
        case 0:
            break;
        case 1:
            tile.features.push_back({ std::move(new_multi[0]), props, id });
            break;
        default:
            tile.features.push_back({ std::move(new_multi), props, id });
            break;
        }
    }

    mapbox::geometry::point<int16_t> transform(const vt_point& p) {
        ++tile.num_simplified;
        return { static_cast<int16_t>(::round((p.x * z2 - x) * extent)),
                 static_cast<int16_t>(::round((p.y * z2 - y) * extent)) };
    }

    mapbox::geometry::multi_point<int16_t> transform(const vt_multi_point& points) {
        mapbox::geometry::multi_point<int16_t> result;
        result.reserve(points.size());
        for (const auto& p : points) {
            result.push_back(transform(p));
        }
        return result;
    }

    mapbox::geometry::line_string<int16_t> transform(const vt_line_string& line) {
        mapbox::geometry::line_string<int16_t> result;
        if (line.dist > tolerance) {
            for (const auto& p : line) {
                if (p.z > sq_tolerance)
                    result.push_back(transform(p));
            }
        }
        return result;
    }

    mapbox::geometry::linear_ring<int16_t> transform(const vt_linear_ring& ring) {
        mapbox::geometry::linear_ring<int16_t> result;
        if (ring.area > sq_tolerance) {
            for (const auto& p : ring) {
                if (p.z > sq_tolerance)
                    result.push_back(transform(p));
            }
        }
        return result;
    }

    mapbox::geometry::multi_line_string<int16_t> transform(const vt_multi_line_string& lines) {
        mapbox::geometry::multi_line_string<int16_t> result;
        for (const auto& line : lines) {
            if (line.dist > tolerance)
                result.push_back(transform(line));
        }
        return result;
    }

    mapbox::geometry::polygon<int16_t> transform(const vt_polygon& rings) {
        mapbox::geometry::polygon<int16_t> result;
        for (const auto& ring : rings) {
            if (ring.area > sq_tolerance)
                result.push_back(transform(ring));
        }
        return result;
    }

    mapbox::geometry::multi_polygon<int16_t> transform(const vt_multi_polygon& polygons) {
        mapbox::geometry::multi_polygon<int16_t> result;
        for (const auto& polygon : polygons) {
            const auto p = transform(polygon);
            if (!p.empty())
                result.push_back(std::move(p));
        }
        return result;
    }
};

} // namespace detail
} // namespace geojsonvt
} // namespace mapbox
