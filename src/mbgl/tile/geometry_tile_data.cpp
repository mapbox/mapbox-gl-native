#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/tile/tile_id.hpp>

#include <clipper/clipper.hpp>

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

static ClipperLib::Path toClipperPath(const GeometryCoordinates& ring) {
    ClipperLib::Path result;
    result.reserve(ring.size());
    for (const auto& p : ring) {
        result.emplace_back(p.x, p.y);
    }
    return result;
}

static GeometryCoordinates fromClipperPath(const ClipperLib::Path& path) {
    GeometryCoordinates result;
    result.reserve(path.size() + 1);

    for (const auto& p : path) {
        using Coordinate = GeometryCoordinates::coordinate_type;
        assert(p.x >= std::numeric_limits<Coordinate>::min());
        assert(p.x <= std::numeric_limits<Coordinate>::max());
        assert(p.y >= std::numeric_limits<Coordinate>::min());
        assert(p.y <= std::numeric_limits<Coordinate>::max());
        result.emplace_back(Coordinate(p.x), Coordinate(p.y));
    }

    // Clipper does not repeat initial point, but our geometry model requires it.
    if (!result.empty()) {
        result.push_back(result.front());
    }

    return result;
}

static void processPolynodeBranch(ClipperLib::PolyNode* polynode, GeometryCollection& rings) {
    // Exterior ring.
    rings.push_back(fromClipperPath(polynode->Contour));
    assert(signedArea(rings.back()) > 0);

    // Interior rings.
    for (auto * ring : polynode->Childs) {
        rings.push_back(fromClipperPath(ring->Contour));
        assert(signedArea(rings.back()) < 0);
    }

    // PolyNodes may be nested in the case of a polygon inside a hole.
    for (auto * ring : polynode->Childs) {
        for (auto * subRing : ring->Childs) {
            processPolynodeBranch(subRing, rings);
        }
    }
}

GeometryCollection fixupPolygons(const GeometryCollection& rings) {
    ClipperLib::Clipper clipper;
    clipper.StrictlySimple(true);

    for (const auto& ring : rings) {
        clipper.AddPath(toClipperPath(ring), ClipperLib::ptSubject, true);
    }

    ClipperLib::PolyTree polygons;
    clipper.Execute(ClipperLib::ctUnion, polygons, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);
    clipper.Clear();

    GeometryCollection result;
    for (auto * polynode : polygons.Childs) {
        processPolynodeBranch(polynode, result);
    }
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
#if !defined(__GNUC__) || __GNUC__ >= 5
    Feature feature { convertGeometry(geometryTileFeature, tileID) };
#else
    Feature feature;
    feature.geometry = convertGeometry(geometryTileFeature, tileID);
#endif
    feature.properties = geometryTileFeature.getProperties();
    feature.id = geometryTileFeature.getID();
    return feature;
}

} // namespace mbgl
