#pragma once

#include <kdbush.hpp>
#include <mapbox/geometry/feature.hpp>
#include <mapbox/geometry/point_arithmetic.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

#ifdef DEBUG_TIMER
#include <chrono>
#include <iostream>
#endif

namespace mapbox {
namespace supercluster {

using namespace mapbox::geometry;

struct Cluster {
    point<double> pos;
    std::uint32_t num_points;
    std::uint32_t id = 0;
    bool visited = false;

    Cluster(point<double> pos_,
            std::uint32_t num_points_,
            std::uint32_t id_ = 0,
            bool visited_ = false)
        : pos(std::move(pos_)),
          num_points(num_points_),
          id(id_),
          visited(visited_) {}
};

} // namespace supercluster
} // namespace mapbox

namespace kdbush {

using Cluster = mapbox::supercluster::Cluster;

template <>
struct nth<0, Cluster> {
    inline static double get(const Cluster &c) {
        return c.pos.x;
    };
};
template <>
struct nth<1, Cluster> {
    inline static double get(const Cluster &c) {
        return c.pos.y;
    };
};

} // namespace kdbush

namespace mapbox {
namespace supercluster {

#ifdef DEBUG_TIMER
class Timer {
public:
    std::chrono::high_resolution_clock::time_point started;
    Timer() {
        started = std::chrono::high_resolution_clock::now();
    }
    void operator()(std::string msg) {
        const auto now = std::chrono::high_resolution_clock::now();
        const auto ms = std::chrono::duration_cast<std::chrono::microseconds>(now - started);
        std::cerr << msg << ": " << double(ms.count()) / 1000 << "ms\n";
        started = now;
    }
};
#endif

struct Options {
    std::uint8_t minZoom = 0;   // min zoom to generate clusters on
    std::uint8_t maxZoom = 16;  // max zoom level to cluster the points on
    std::uint16_t radius = 40;  // cluster radius in pixels
    std::uint16_t extent = 512; // tile extent (radius is calculated relative to it)
};

class Supercluster {
    using GeoJSONPoint = point<double>;
    using GeoJSONFeatures = feature_collection<double>;

    using TilePoint = point<std::int16_t>;
    using TileFeature = feature<std::int16_t>;
    using TileFeatures = feature_collection<std::int16_t>;

public:
    const GeoJSONFeatures features;
    const Options options;

    Supercluster(const GeoJSONFeatures &features_, const Options options_ = Options())
        : features(features_), options(options_) {

#ifdef DEBUG_TIMER
        Timer timer;
#endif
        // convert and index initial points
        zooms.emplace(options.maxZoom + 1, features);
#ifdef DEBUG_TIMER
        timer(std::to_string(features.size()) + " initial points");
#endif
        for (int z = options.maxZoom; z >= options.minZoom; z--) {
            // cluster points from the previous zoom level
            const double r = options.radius / (options.extent * std::pow(2, z));
            zooms.emplace(z, Zoom(zooms[z + 1], r));
#ifdef DEBUG_TIMER
            timer(std::to_string(zooms[z].clusters.size()) + " clusters");
#endif
        }
    }

    TileFeatures getTile(std::uint8_t z, std::uint32_t x_, std::uint32_t y) {
        TileFeatures result;
        auto &zoom = zooms[limitZoom(z)];

        std::uint32_t z2 = std::pow(2, z);
        double const r = static_cast<double>(options.radius) / options.extent;
        std::int32_t x = static_cast<std::int32_t>(x_);

        auto visitor = [&, this](const auto &id) {
            auto const &c = zoom.clusters[id];

            TilePoint point(::round(this->options.extent * (c.pos.x * z2 - x)),
                            ::round(this->options.extent * (c.pos.y * z2 - y)));
            TileFeature feature{ point };

            if (c.num_points == 1) {
                feature.properties = this->features[c.id].properties;
            } else {
                feature.properties["cluster"] = true;
                feature.properties["point_count"] = static_cast<std::uint64_t>(c.num_points);
            }

            result.push_back(feature);
        };

        double const top = (y - r) / z2;
        double const bottom = (y + 1 + r) / z2;

        zoom.tree.range((x - r) / z2, top, (x + 1 + r) / z2, bottom, visitor);

        if (x_ == 0) {
            x = z2;
            zoom.tree.range(1 - r / z2, top, 1, bottom, visitor);
        }
        if (x_ == z2 - 1) {
            x = -1;
            zoom.tree.range(0, top, r / z2, bottom, visitor);
        }

        return result;
    }

private:
    struct Zoom {
        kdbush::KDBush<Cluster, std::uint32_t> tree;
        std::vector<Cluster> clusters;

        Zoom() = default;

        Zoom(const GeoJSONFeatures &features_) {
            // generate a cluster object for each point
            std::uint32_t i = 0;

            for (const auto &f : features_) {
                clusters.push_back({ project(f.geometry.get<GeoJSONPoint>()), 1, i++ });
            }

            tree.fill(clusters);
        }

        Zoom(Zoom &previous, double r) {
            for (auto &p : previous.clusters) {
                if (p.visited)
                    continue;
                p.visited = true;

                auto num_points = p.num_points;
                point<double> weight = p.pos * double(num_points);

                // find all nearby points
                previous.tree.within(p.pos.x, p.pos.y, r, [&](const auto &id) {
                    auto &b = previous.clusters[id];

                    // filter out neighbors that are already processed
                    if (b.visited)
                        return;
                    b.visited = true;

                    // accumulate coordinates for calculating weighted center
                    weight += b.pos * double(b.num_points);
                    num_points += b.num_points;
                });

                clusters.push_back({ weight / double(num_points), num_points, p.id });
            }

            tree.fill(clusters);
        }
    };

    std::unordered_map<std::uint8_t, Zoom> zooms;

    std::uint8_t limitZoom(std::uint8_t z) {
        if (z < options.minZoom)
            return options.minZoom;
        if (z > options.maxZoom + 1)
            return options.maxZoom + 1;
        return z;
    }

    static point<double> project(const GeoJSONPoint &p) {
        auto lngX = p.x / 360 + 0.5;
        const double sine = std::sin(p.y * M_PI / 180);
        const double y = 0.5 - 0.25 * std::log((1 + sine) / (1 - sine)) / M_PI;
        auto latY = std::min(std::max(y, 0.0), 1.0);
        return { lngX, latY };
    }
};

} // namespace supercluster
} // namespace mapbox
