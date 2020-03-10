#include <mbgl/test/util.hpp>

#include <mapbox/geojson_impl.hpp>
#include <mapbox/geometry/geometry.hpp>
#include <mbgl/util/geometry_buffer.hpp>

#include <mapbox/cheap_ruler.hpp>
#include <mbgl/util/feature.hpp>

#include <string>

#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometry.hpp>

using namespace mbgl;

namespace {
mapbox::geometry::geometry<double> getGeometry(const std::string& geo) {
    mapbox::geojson::geojson geojson{mapbox::geojson::parse(geo)};
    return geojson.match(
        [](const mapbox::geometry::geometry<double>& geometrySet) { return mbgl::Feature(geometrySet).geometry; },
        [](const mapbox::feature::feature<double>& feature) { return mbgl::Feature(feature).geometry; },
        [](const mapbox::feature::feature_collection<double>& features) {
            return mbgl::Feature(features.front()).geometry;
        },
        [](const auto&) { return mapbox::geometry::empty(); });
}

} // namespace

TEST(GeometryBuffer, BufferForPoint) {
    static const std::string point = R"data({
            "type": "Point",
            "coordinates": [1, 1]
        })data";

    const auto geometry = getGeometry(point);
    ASSERT_NE(geometry, mapbox::geometry::empty());
    // Generate a polygon buffer with radius 50 meters and 10 points
    const auto geoPolygons = mbgl::GeometryBuffer(geometry, 500.0, 10).getGeometryBuffer();

    // Check result: only one polygon with one ring
    ASSERT_TRUE(geoPolygons);
    ASSERT_EQ(1, geoPolygons->size());
    const auto& polygon = (*geoPolygons)[0];
    ASSERT_EQ(1, polygon.size());
    const auto& ring = polygon[0];
    ASSERT_EQ(11, ring.size());

    mapbox::cheap_ruler::CheapRuler ruler(1.0, mapbox::cheap_ruler::CheapRuler::Unit::Meters);
    const mapbox::geometry::point<double> center(1, 1);
    for (size_t i = 0; i < ring.size(); i++) {
        auto actualDistance = ruler.distance(center, ring[i]);
        // Allow 2.5 meter's error, which is 5 meters per 1km
        EXPECT_NEAR(500.0, actualDistance, 2.5);
    }
}

TEST(GeometryBuffer, BufferForPoints) {
    static const std::string points = R"data({
               "type": "MultiPoint",
               "coordinates": [[60, 50], [60.3, 50.2]]
           })data";

    const auto geometry = getGeometry(points);
    ASSERT_NE(geometry, mapbox::geometry::empty());
    // Generate a polygon buffer with radius 10 meters and 5 points for circle
    const auto geoPolygons = mbgl::GeometryBuffer(geometry, 10.0, 5).getGeometryBuffer();

    // Check result: 2 polygons with one ring in each
    ASSERT_TRUE(geoPolygons);
    ASSERT_EQ(2, geoPolygons->size());

    const auto& polygon1 = (*geoPolygons)[0];
    ASSERT_EQ(1, polygon1.size());
    const auto& ring1 = polygon1[0];
    ASSERT_EQ(6, ring1.size());
    mapbox::cheap_ruler::CheapRuler ruler1(50.0, mapbox::cheap_ruler::CheapRuler::Unit::Meters);
    const mapbox::geometry::point<double> center1(60, 50);
    for (size_t i = 0; i < ring1.size(); i++) {
        auto actualDistance = ruler1.distance(center1, ring1[i]);
        // Allow 0.5 meter's error
        EXPECT_NEAR(10.0, actualDistance, 0.05);
    }
    const auto& polygon2 = (*geoPolygons)[1];
    ASSERT_EQ(1, polygon2.size());
    const auto& ring2 = polygon2[0];
    ASSERT_EQ(6, ring2.size());
    mapbox::cheap_ruler::CheapRuler ruler2(50.2, mapbox::cheap_ruler::CheapRuler::Unit::Meters);
    const mapbox::geometry::point<double> center2(60.3, 50.2);
    for (size_t i = 0; i < ring2.size(); i++) {
        auto actualDistance = ruler2.distance(center2, ring2[i]);
        // Allow 0.05 meter's error, which is 5 meters per 1km
        EXPECT_NEAR(10.0, actualDistance, 0.05);
    }
}

TEST(GeometryBuffer, BufferForLine) {
    static const std::string line = R"data({
               "type": "LineString",
               "coordinates": [[
                 24.93696391582489,
                 60.16926456940512
               ],
               [
                 24.938240647315975,
                 60.16974755943638
               ]]
           })data";

    const auto geometry = getGeometry(line);
    ASSERT_NE(geometry, mapbox::geometry::empty());
    // Generate a polygon buffer with radius 50 meters and 5 points for circle
    const auto geoPolygons = mbgl::GeometryBuffer(geometry, 100.0, 5).getGeometryBuffer();

    // Check result: only one polygon with one ring
    ASSERT_TRUE(geoPolygons);
    ASSERT_EQ(1, geoPolygons->size());

    const auto& polygon1 = (*geoPolygons)[0];
    ASSERT_EQ(1, polygon1.size());
    const auto& ring1 = polygon1[0];
    ASSERT_EQ(10, ring1.size());
    const mapbox::geometry::line_string<double> lineString{{24.93696391582489, 60.16926456940512},
                                                           {24.938240647315975, 60.16974755943638}};
    mapbox::cheap_ruler::CheapRuler ruler2(lineString[0].y, mapbox::cheap_ruler::CheapRuler::Unit::Meters);
    for (size_t i = 0; i < ring1.size(); i++) {
        auto nearestPoint = std::get<0>(ruler2.pointOnLine(lineString, ring1[i]));
        auto actualDistance = ruler2.distance(nearestPoint, ring1[i]);
        // Allow 0.5 meter's error, which is 5 meters per 1km
        EXPECT_NEAR(100.0, actualDistance, 0.5);
    }
}

TEST(GeometryBuffer, BufferForLines) {
    static const std::string lines = R"data({
              "type": "MultiLineString",
              "coordinates": [[[
                24.93696391582489,
                60.16926456940512
              ],
              [
                24.938240647315975,
                60.16974755943638
              ]],
              [[
                24.93122935295105,
                60.167383242200444
              ],
              [
                24.931269586086273,
                60.16739658532588
              ]]
             ]
          })data";

    const auto geometry = getGeometry(lines);
    ASSERT_NE(geometry, mapbox::geometry::empty());
    // Generate a polygon buffer with radius 50 meters and 15 points for circle
    const auto geoPolygons = mbgl::GeometryBuffer(geometry, 50.0, 15).getGeometryBuffer();

    // Check result: 2 polygons with one ring in each
    ASSERT_TRUE(geoPolygons);
    ASSERT_EQ(2, geoPolygons->size());

    const auto& polygon1 = (*geoPolygons)[0];
    ASSERT_EQ(1, polygon1.size());
    const auto& ring1 = polygon1[0];
    ASSERT_EQ(20, ring1.size());
    const mapbox::geometry::line_string<double> lineString{{24.93696391582489, 60.16926456940512},
                                                           {24.938240647315975, 60.16974755943638}};
    mapbox::cheap_ruler::CheapRuler ruler1(lineString[0].y, mapbox::cheap_ruler::CheapRuler::Unit::Meters);
    for (size_t i = 0; i < ring1.size(); i++) {
        auto nearestPoint = std::get<0>(ruler1.pointOnLine(lineString, ring1[i]));
        auto actualDistance = ruler1.distance(nearestPoint, ring1[i]);
        // Allow 0.25 meter's error, which is 5 meters per 1km
        EXPECT_NEAR(50.0, actualDistance, 0.25);
    }

    const auto& polygon2 = (*geoPolygons)[1];
    ASSERT_EQ(1, polygon2.size());
    const auto& ring2 = polygon2[0];
    ASSERT_EQ(20, ring2.size());
    const mapbox::geometry::line_string<double> lineString2{{24.93122935295105, 60.167383242200444},
                                                            {24.931269586086273, 60.16739658532588}};
    mapbox::cheap_ruler::CheapRuler ruler2(lineString2[0].y, mapbox::cheap_ruler::CheapRuler::Unit::Meters);
    for (size_t i = 0; i < ring2.size(); i++) {
        auto nearestPoint = std::get<0>(ruler2.pointOnLine(lineString, ring1[i]));
        auto actualDistance = ruler2.distance(nearestPoint, ring1[i]);
        // Allow 0.25 meter's error, which is 5 meters per 1km
        EXPECT_NEAR(50.0, actualDistance, 0.25);
    }
}
