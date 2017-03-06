#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/tile/tile_id.hpp>

#include <mapbox/geometry/wagyu/wagyu.hpp>

namespace mbgl {

static double signedArea(const GeometryCoordinates& ring) {
    double sum = 0;

    for (std::size_t i = 0, len = ring.size(), j = len - 1; i < len; j = i++) {
        const GeometryCoordinate& p1 = ring[i];
        const GeometryCoordinate& p2 = ring[j];
        sum += (p2.x - p1.x) * (p1.y + p2.y);
    }

    return sum;
}

static LinearRing<std::int32_t> toWagyuPath(const GeometryCoordinates& ring) {
    LinearRing<std::int32_t> result;
    result.reserve(ring.size());
    for (const auto& p : ring) {
        result.emplace_back(p.x, p.y);
    }
    return result;
}

static void pushWagyuRing(GeometryCollection & solution,
                          mapbox::geometry::wagyu::ring_ptr<std::int32_t> r) {
    GeometryCoordinates lr;
    lr.reserve(r->size() + 1);
    auto firstPt = r->points;
    auto ptIt = r->points;
    do {
        lr.emplace_back(ptIt->x, ptIt->y);
        ptIt = ptIt->prev;
    } while (ptIt != firstPt);
    lr.emplace_back(firstPt->x, firstPt->y); // close the ring
    solution.push_back(lr);
}

static void buildWagyuResults(GeometryCollection & solution,
                              mapbox::geometry::wagyu::ring_vector<std::int32_t>& rings) {
    for (auto r : rings) {
        if (r == nullptr) {
            continue;
        }
        assert(r->points);
        if (r->size() < 3) {
            continue;
        }
        solution.emplace_back();
        pushWagyuRing(solution, r);
        for (auto c : r->children) {
            if (c == nullptr) {
                continue;
            }
            assert(c->points);
            if (c->size() < 3) {
                continue;
            }
            pushWagyuRing(solution, c);
        }
        for (auto c : r->children) {
            if (c == nullptr) {
                continue;
            }
            if (!c->children.empty()) {
                buildWagyuResults(solution, c->children);
            }
        }
    }
}

GeometryCollection fixupPolygons(const GeometryCollection& rings) {
    using namespace mapbox::geometry::wagyu;
    
    // This is code that is pulled from the wagyu main class, we 
    // are doing this to have our own custom build result,
    // rather then copying output twice from the wagyu algorithm.

    local_minimum_list<std::int32_t> minima_list;

    for (const auto& ring : rings) {
        // Convert ring to LinearRing type prior to adding them
        add_linear_ring(toWagyuPath(ring), minima_list, polygon_type_subject);
    }

    // Core part of wagyu algorithm
    ring_manager<std::int32_t> manager;
    
    build_hot_pixels(minima_list, manager);
    
    execute_vatti(minima_list, manager, clip_type_union, fill_type_even_odd, fill_type_even_odd);
    
    correct_topology(manager);

    // Finally lets build results
    GeometryCollection result;
 
    // This calls code based on wagyu/build_results.hpp
    // rather then returning a multipolygon however, we are
    // going to return a GeometryCollection
    buildWagyuResults(result, manager.children);

    return result;
}

std::vector<GeometryCollection> classifyRings(const GeometryCollection& rings) {
    std::vector<GeometryCollection> polygons;

    std::size_t len = rings.size();

    if (len <= 1) {
        polygons.push_back(rings);
        return polygons;
    }

    GeometryCollection polygon;
    int8_t ccw = 0;

    for (std::size_t i = 0; i < len; i++) {
        double area = signedArea(rings[i]);

        if (area == 0)
            continue;

        if (ccw == 0)
            ccw = (area < 0 ? -1 : 1);

        if (ccw == (area < 0 ? -1 : 1) && !polygon.empty()) {
            polygons.push_back(polygon);
            polygon.clear();
        }

        polygon.push_back(rings[i]);
    }

    if (!polygon.empty())
        polygons.push_back(polygon);

    return polygons;
}

void limitHoles(GeometryCollection& polygon, uint32_t maxHoles) {
    if (polygon.size() > 1 + maxHoles) {
        std::nth_element(polygon.begin() + 1,
                         polygon.begin() + 1 + maxHoles,
                         polygon.end(),
                         [] (const auto& a, const auto& b) {
                             return std::fabs(signedArea(a)) > std::fabs(signedArea(b));
                         });
        polygon.resize(1 + maxHoles);
    }
}

static Feature::geometry_type convertGeometry(const GeometryTileFeature& geometryTileFeature, const CanonicalTileID& tileID) {
    const double size = util::EXTENT * std::pow(2, tileID.z);
    const double x0 = util::EXTENT * tileID.x;
    const double y0 = util::EXTENT * tileID.y;

    auto tileCoordinatesToLatLng = [&] (const Point<int16_t>& p) {
        double y2 = 180 - (p.y + y0) * 360 / size;
        return Point<double>(
            (p.x + x0) * 360 / size - 180,
            360.0 / M_PI * std::atan(std::exp(y2 * M_PI / 180)) - 90.0
        );
    };

    GeometryCollection geometries = geometryTileFeature.getGeometries();

    switch (geometryTileFeature.getType()) {
        case FeatureType::Unknown: {
            assert(false);
            return Point<double>(NAN, NAN);
        }

        case FeatureType::Point: {
            MultiPoint<double> multiPoint;
            for (const auto& p : geometries.at(0)) {
                multiPoint.push_back(tileCoordinatesToLatLng(p));
            }
            if (multiPoint.size() == 1) {
                return multiPoint[0];
            } else {
                return multiPoint;
            }
        }

        case FeatureType::LineString: {
            MultiLineString<double> multiLineString;
            for (const auto& g : geometries) {
                LineString<double> lineString;
                for (const auto& p : g) {
                    lineString.push_back(tileCoordinatesToLatLng(p));
                }
                multiLineString.push_back(std::move(lineString));
            }
            if (multiLineString.size() == 1) {
                return multiLineString[0];
            } else {
                return multiLineString;
            }
        }

        case FeatureType::Polygon: {
            MultiPolygon<double> multiPolygon;
            for (const auto& pg : classifyRings(geometries)) {
                Polygon<double> polygon;
                for (const auto& r : pg) {
                    LinearRing<double> linearRing;
                    for (const auto& p : r) {
                        linearRing.push_back(tileCoordinatesToLatLng(p));
                    }
                    polygon.push_back(std::move(linearRing));
                }
                multiPolygon.push_back(std::move(polygon));
            }
            if (multiPolygon.size() == 1) {
                return multiPolygon[0];
            } else {
                return multiPolygon;
            }
        }
    }

    // Unreachable, but placate GCC.
    return Point<double>();
}

Feature convertFeature(const GeometryTileFeature& geometryTileFeature, const CanonicalTileID& tileID) {
    Feature feature { convertGeometry(geometryTileFeature, tileID) };
    feature.properties = geometryTileFeature.getProperties();
    feature.id = geometryTileFeature.getID();
    return feature;
}

} // namespace mbgl
