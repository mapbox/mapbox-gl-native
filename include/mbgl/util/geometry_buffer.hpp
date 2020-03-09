#pragma once

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

#include <rapidjson/prettywriter.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/stringbuffer.h>

#include <mbgl/util/geometry.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/variant.hpp>

#include <mapbox/geojson_impl.hpp>
#include <mapbox/geometry/geometry.hpp>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

namespace mbgl {
namespace {

typedef double coordinate_type;

typedef boost::geometry::model::d2::point_xy<coordinate_type> BoostPoint;
typedef boost::geometry::model::multi_point<BoostPoint> BoostMultiPoint;
typedef boost::geometry::model::linestring<BoostPoint> BoostLineString;
typedef boost::geometry::model::multi_linestring<BoostLineString> BoostMultiLineString;
typedef boost::geometry::model::polygon<BoostPoint> BoostPolygon;
typedef boost::geometry::model::multi_polygon<BoostPolygon> BoostMultiPolygon;

typedef boost::geometry::strategy::buffer::distance_symmetric<coordinate_type> DistanceStrategy;
typedef boost::geometry::strategy::buffer::join_round JoinStrategy;
typedef boost::geometry::strategy::buffer::end_round EndStrategy;
typedef boost::geometry::strategy::buffer::point_circle CircleStrategy;
typedef boost::geometry::strategy::buffer::side_straight SideStrategy;

static const double earthRadius = 6371008.8;

void generateFile(const std::string& filename, const std::string& data) {
    FILE* fd = fopen(filename.c_str(), "wb");
    if (fd) {
        fwrite(data.data(), sizeof(std::string::value_type), data.size(), fd);
        fclose(fd);
    } else {
        throw std::runtime_error(std::string("Failed to open file ") + filename);
    }
}

double mercatorXfromLng(double lng) {
    return (180 + lng) / 360;
}

double mercatorYfromLat(double lat) {
    return (180 - (180 / M_PI * std::log(std::tan(M_PI / 4 + lat * M_PI / 360)))) / 360;
}

double lngFromMercatorX(double x) {
    return x * 360 - 180;
}

double latFromMercatorY(double y) {
    const auto y2 = 180 - y * 360;
    return 360 / M_PI * std::atan(std::exp(y2 * M_PI / 180)) - 90;
}

BoostPoint mercatorPointFromLngLat(const mapbox::geometry::point<double> point) {
    return {mercatorXfromLng(point.x), mercatorYfromLat(point.y)};
}

mapbox::geometry::point<double> lngLatPointFromMercator(const BoostPoint& point) {
    return {lngFromMercatorX(point.x()), latFromMercatorY(point.y())};
}

// Determine the Mercator scale factor for a given latitude, see
// https://en.wikipedia.org/wiki/Mercator_projection#Scale_factor
//
// At the equator the scale factor will be 1, which increases at higher latitudes.
double mercatorScale(double latitude) {
    return 1 / std::cos(latitude * M_PI / 180);
}

// Returns the distance of 1 meter in `MercatorCoordinate` units at this latitude.
//
// For coordinates in real world units using meters, this naturally provides the scale
// to transform into `MercatorCoordinate`s.
double meterInMercatorCoordinateUnits(double lattitude) {
    // 1 meter / circumference at equator in meters * Mercator projection scale factor at this latitude
    static const double earthCircumfrence = 2 * M_PI * earthRadius; // meters
    return 1 / earthCircumfrence * mercatorScale(lattitude);
}

// Convert polygon geometry from mercator to lnglat
mapbox::geometry::multi_polygon<double> lngLatPolygonFromMercator(const BoostMultiPolygon& polygons) {
    mapbox::geometry::multi_polygon<double> mapboxPolygons;
    mapboxPolygons.reserve(polygons.size());
    for (const auto& polygon : polygons) {
        mapbox::geometry::polygon<double> mapboxPolygon;
        const auto& outerRing = polygon.outer();
        const auto& innerRings = polygon.inners();
        mapboxPolygon.reserve(1 + innerRings.size());

        mapbox::geometry::linear_ring<double> ring;
        ring.reserve(outerRing.size());
        for (const auto& p : outerRing) {
            ring.emplace_back(lngLatPointFromMercator(p));
        }
        mapboxPolygon.push_back(std::move(ring));

        for (const auto& r : innerRings) {
            ring.reserve(r.size());
            for (const auto& p : r) {
                ring.emplace_back(lngLatPointFromMercator(p));
            }
            mapboxPolygon.push_back(std::move(ring));
        }
        mapboxPolygons.push_back(std::move(mapboxPolygon));
    }
    return mapboxPolygons;
}

// Convert geometry to geojson
std::string getGeoStringBuffer(const mapbox::geometry::geometry<double> geometry, bool singleLine = false) {
    using JSValue = rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator>;
    unsigned indent = 2;
    rapidjson::CrtAllocator allocator;
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    if (singleLine) {
        writer.SetFormatOptions(rapidjson::kFormatSingleLineArray);
    }
    writer.SetIndent(' ', indent);
    JSValue json(rapidjson::kObjectType);
    json.AddMember("type", "Feature", allocator);
    JSValue v;
    v.SetObject();
    json.AddMember("properties", v, allocator);
    json.AddMember("geometry", mapbox::geojson::convert(geometry, allocator), allocator);
    json.Accept(writer);

    return buffer.GetString();
}

// Generate polygon buffer based on the type of geometry inputs
mapbox::geometry::multi_polygon<double> getPolygonBuffer(const mapbox::geometry::geometry<double>& geometries,
                                                         const double distanceInMeter,
                                                         const int pointsPerCircle) {
    double bufferDistance = 0;
    return geometries.match(
        [&distanceInMeter, &bufferDistance, &pointsPerCircle](const mapbox::geometry::point<double>& point) {
            // Convert meter to boost geometry distance
            bufferDistance = meterInMercatorCoordinateUnits(point.y) * distanceInMeter;

            const auto boostGeometry = mercatorPointFromLngLat(point);
            BoostMultiPolygon polygons;
            boost::geometry::buffer(boostGeometry,
                                    polygons,
                                    DistanceStrategy(bufferDistance),
                                    SideStrategy(),
                                    JoinStrategy(pointsPerCircle),
                                    EndStrategy(pointsPerCircle),
                                    CircleStrategy(pointsPerCircle));

            assert(polygons.size() > 0);
            return lngLatPolygonFromMercator(polygons);
        },
        [&distanceInMeter, &bufferDistance, &pointsPerCircle](const mapbox::geometry::multi_point<double>& points) {
            assert(points.size());
            // Convert meter to boost geometry distance
            bufferDistance = meterInMercatorCoordinateUnits(points.front().y) * distanceInMeter;

            BoostMultiPoint boostPoints;
            boostPoints.reserve(points.size());
            for (const auto& p : points) {
                boostPoints.push_back(mercatorPointFromLngLat(p));
            }
            BoostMultiPolygon polygons;
            boost::geometry::buffer(boostPoints,
                                    polygons,
                                    DistanceStrategy(bufferDistance),
                                    SideStrategy(),
                                    JoinStrategy(pointsPerCircle),
                                    EndStrategy(pointsPerCircle),
                                    CircleStrategy(pointsPerCircle));

            assert(polygons.size() > 0);
            return lngLatPolygonFromMercator(polygons);
        },
        [&distanceInMeter, &bufferDistance, &pointsPerCircle](const mapbox::geometry::line_string<double>& line) {
            // Convert meter to boost geometry distance
            assert(line.size());
            bufferDistance = meterInMercatorCoordinateUnits(line.front().y) * distanceInMeter;

            BoostLineString boostLine;
            boostLine.reserve(line.size());
            for (const auto& p : line) {
                boostLine.push_back(mercatorPointFromLngLat(p));
            }
            BoostMultiPolygon polygons;
            boost::geometry::buffer(boostLine,
                                    polygons,
                                    DistanceStrategy(bufferDistance),
                                    SideStrategy(),
                                    JoinStrategy(pointsPerCircle),
                                    EndStrategy(pointsPerCircle),
                                    CircleStrategy(pointsPerCircle));

            assert(polygons.size() > 0);
            return lngLatPolygonFromMercator(polygons);
        },
        [&distanceInMeter, &bufferDistance, &pointsPerCircle](
            const mapbox::geometry::multi_line_string<double>& lines) {
            // Convert meter to boost geometry distance
            assert(lines.size());
            bufferDistance = meterInMercatorCoordinateUnits(lines.front().front().y) * distanceInMeter;

            BoostMultiLineString boostLines;
            boostLines.reserve(lines.size());
            for (const auto& line : lines) {
                BoostLineString boostLine;
                boostLine.reserve(line.size());
                for (const auto& p : line) {
                    boostLine.push_back(mercatorPointFromLngLat(p));
                }
                boostLines.push_back(boostLine);
            }

            BoostMultiPolygon polygons;
            boost::geometry::buffer(boostLines,
                                    polygons,
                                    DistanceStrategy(bufferDistance),
                                    SideStrategy(),
                                    JoinStrategy(pointsPerCircle),
                                    EndStrategy(pointsPerCircle),
                                    CircleStrategy(pointsPerCircle));

            assert(polygons.size() > 0);
            return lngLatPolygonFromMercator(polygons);
        },
        [](const auto&) {
            mbgl::Log::Warning(mbgl::Event::General, "GeometryBuffer only supports Point/LineString geometry type.");
            return mapbox::geometry::multi_polygon<double>();
        });
}
} // namespace

class GeometryBuffer {
public:
    GeometryBuffer(const mapbox::geometry::geometry<double>& geometries_,
                   const double bufferRadiusInMeter_,
                   const uint32_t pointsPerCircle_)
        : geometries(geometries_), bufferRadiusInMeter(bufferRadiusInMeter_), pointsPerCircle(pointsPerCircle_){};

    mbgl::optional<mapbox::geometry::multi_polygon<double>> getGeometryBuffer() {
        auto ret = getPolygonBuffer(geometries, bufferRadiusInMeter, pointsPerCircle);
        if (ret.empty()) {
            return nullopt;
        }
        return ret;
    }

    std::string getGeoJSONBuffer(bool singleLine = false) {
        const auto geoBuffer = getGeometryBuffer();
        if (geoBuffer) {
            return getGeoStringBuffer(*geoBuffer, singleLine);
        }
        return {};
    }

    static std::string geoJSONFromGeometryBuffer(const mapbox::geometry::multi_polygon<double>& geometryBuffer,
                                                 bool singleLine = false) {
        return getGeoStringBuffer(geometryBuffer, singleLine);
    }

    static void writeGeoJSON(const std::string& path, const std::string& json) { generateFile(path, json); }

private:
    mapbox::geometry::geometry<double> geometries;
    double bufferRadiusInMeter;
    uint32_t pointsPerCircle;
};

} // namespace mbgl
