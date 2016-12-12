#include "util.hpp"
#include <gtest/gtest.h>
#include <mapbox/geojson.hpp>
#include <mapbox/geojsonvt.hpp>
#include <mapbox/geojsonvt/clip.hpp>
#include <mapbox/geojsonvt/convert.hpp>
#include <mapbox/geojsonvt/simplify.hpp>
#include <mapbox/geojsonvt/tile.hpp>
#include <mapbox/geometry.hpp>

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace mapbox::geojsonvt;

GTEST_API_ int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

bool operator==(const detail::vt_point& a, const detail::vt_point& b) {
    EXPECT_DOUBLE_EQ(a.x, b.x);
    EXPECT_DOUBLE_EQ(a.y, b.y);
    return true;
}

TEST(Simplify, Points) {
    std::vector<detail::vt_point> points = {
        { 0.22455, 0.25015 }, { 0.22691, 0.24419 }, { 0.23331, 0.24145 }, { 0.23498, 0.23606 },
        { 0.24421, 0.23276 }, { 0.26259, 0.21531 }, { 0.26776, 0.21381 }, { 0.27357, 0.20184 },
        { 0.27312, 0.19216 }, { 0.27762, 0.18903 }, { 0.28036, 0.18141 }, { 0.28651, 0.17774 },
        { 0.29241, 0.15937 }, { 0.29691, 0.15564 }, { 0.31495, 0.15137 }, { 0.31975, 0.14516 },
        { 0.33033, 0.13757 }, { 0.34148, 0.13996 }, { 0.36998, 0.13789 }, { 0.38739, 0.14251 },
        { 0.39128, 0.13939 }, { 0.40952, 0.14114 }, { 0.41482, 0.13975 }, { 0.42772, 0.12730 },
        { 0.43960, 0.11974 }, { 0.47493, 0.10787 }, { 0.48651, 0.10675 }, { 0.48920, 0.10945 },
        { 0.49379, 0.10863 }, { 0.50474, 0.11966 }, { 0.51296, 0.12235 }, { 0.51863, 0.12089 },
        { 0.52409, 0.12688 }, { 0.52957, 0.12786 }, { 0.53421, 0.14093 }, { 0.53927, 0.14724 },
        { 0.56769, 0.14891 }, { 0.57525, 0.15726 }, { 0.58062, 0.15815 }, { 0.60153, 0.15685 },
        { 0.61774, 0.15986 }, { 0.62200, 0.16704 }, { 0.62955, 0.19460 }, { 0.63890, 0.19561 },
        { 0.64126, 0.20081 }, { 0.65177, 0.20456 }, { 0.67155, 0.22255 }, { 0.68368, 0.21745 },
        { 0.69525, 0.21915 }, { 0.70064, 0.21798 }, { 0.70312, 0.21436 }, { 0.71226, 0.21587 },
        { 0.72149, 0.21281 }, { 0.72781, 0.21336 }, { 0.72998, 0.20873 }, { 0.73532, 0.20820 },
        { 0.73994, 0.20477 }, { 0.76998, 0.20842 }, { 0.77960, 0.21687 }, { 0.78420, 0.21816 },
        { 0.80024, 0.21462 }, { 0.81053, 0.21973 }, { 0.81719, 0.22682 }, { 0.82077, 0.23617 },
        { 0.82723, 0.23616 }, { 0.82989, 0.23989 }, { 0.85100, 0.24894 }, { 0.85988, 0.25549 },
        { 0.86521, 0.26853 }, { 0.85795, 0.28030 }, { 0.86548, 0.29145 }, { 0.86681, 0.29866 },
        { 0.86468, 0.30271 }, { 0.86779, 0.30617 }, { 0.85987, 0.31137 }, { 0.86008, 0.31435 },
        { 0.85829, 0.31494 }, { 0.85810, 0.32760 }, { 0.85454, 0.33540 }, { 0.86092, 0.34300 },
        { 0.85643, 0.35015 }, { 0.85142, 0.35296 }, { 0.84984, 0.35959 }, { 0.85456, 0.36553 },
        { 0.84974, 0.37038 }, { 0.84409, 0.37189 }, { 0.84475, 0.38044 }, { 0.84152, 0.38367 },
        { 0.83957, 0.39040 }, { 0.84559, 0.39905 }, { 0.84840, 0.40755 }, { 0.84371, 0.41130 },
        { 0.84409, 0.41988 }, { 0.83951, 0.43276 }, { 0.84133, 0.44104 }, { 0.84762, 0.44922 },
        { 0.84716, 0.45844 }, { 0.85138, 0.46279 }, { 0.85397, 0.47115 }, { 0.86636, 0.48077 }
    };

    const std::vector<detail::vt_point> simplified{
        { 0.22455, 0.25015 }, { 0.26776, 0.21381 }, { 0.29691, 0.15564 }, { 0.33033, 0.13757 },
        { 0.40952, 0.14114 }, { 0.4396, 0.11974 },  { 0.48651, 0.10675 }, { 0.52957, 0.12786 },
        { 0.53927, 0.14724 }, { 0.56769, 0.14891 }, { 0.61774, 0.15986 }, { 0.62955, 0.1946 },
        { 0.67155, 0.22255 }, { 0.72781, 0.21336 }, { 0.73994, 0.20477 }, { 0.76998, 0.20842 },
        { 0.7842, 0.21816 },  { 0.80024, 0.21462 }, { 0.82077, 0.23617 }, { 0.85988, 0.25549 },
        { 0.86521, 0.26853 }, { 0.85795, 0.2803 },  { 0.86779, 0.30617 }, { 0.85829, 0.31494 },
        { 0.85454, 0.3354 },  { 0.86092, 0.343 },   { 0.84984, 0.35959 }, { 0.85456, 0.36553 },
        { 0.84409, 0.37189 }, { 0.83957, 0.3904 },  { 0.8484, 0.40755 },  { 0.83951, 0.43276 },
        { 0.85397, 0.47115 }, { 0.86636, 0.48077 }
    };

    detail::simplify(points, 0.001);

    std::vector<detail::vt_point> result;
    for (const auto& p : points) {
        if (p.z > 0.005 * 0.005) {
            result.push_back(p);
        }
    }

    ASSERT_EQ(result, simplified);
}

TEST(Clip, Polylines) {
    const detail::vt_line_string points1{ { 0, 0 },   { 50, 0 },  { 50, 10 }, { 20, 10 },
                                          { 20, 20 }, { 30, 20 }, { 30, 30 }, { 50, 30 },
                                          { 50, 40 }, { 25, 40 }, { 25, 50 }, { 0, 50 },
                                          { 0, 60 },  { 25, 60 }, { 30, 60 } };

    const detail::vt_line_string points2{ { 0, 0 }, { 50, 0 }, { 50, 10 }, { 0, 10 } };

    const auto clip = detail::clipper<0>{ 10, 40 };

    const auto clipped1 = clip(points1);
    const auto clipped2 = clip(points2);

    const detail::vt_geometry expected1{ detail::vt_multi_line_string{
        { { 10, 0 }, { 40, 0 } },
        { { 40, 10 }, { 20, 10 }, { 20, 20 }, { 30, 20 }, { 30, 30 }, { 40, 30 } },
        { { 40, 40 }, { 25, 40 }, { 25, 50 }, { 10, 50 } },
        { { 10, 60 }, { 25, 60 }, { 30, 60 } } } };

    const detail::vt_geometry expected2{ detail::vt_multi_line_string{
        { { 10, 0 }, { 40, 0 } }, { { 40, 10 }, { 10, 10 } } } };

    ASSERT_EQ(expected1, clipped1);
    ASSERT_EQ(expected2, clipped2);
}

TEST(Clip, Polygons) {
    const detail::vt_polygon points1{ { { 0, 0 },
                                        { 50, 0 },
                                        { 50, 10 },
                                        { 20, 10 },
                                        { 20, 20 },
                                        { 30, 20 },
                                        { 30, 30 },
                                        { 50, 30 },
                                        { 50, 40 },
                                        { 25, 40 },
                                        { 25, 50 },
                                        { 0, 50 },
                                        { 0, 60 },
                                        { 25, 60 },
                                        { 0, 0 } } };

    const detail::vt_polygon points2{ { { 0, 0 }, { 50, 0 }, { 50, 10 }, { 0, 10 }, { 0, 0 } } };

    const auto clip = detail::clipper<0>{ 10, 40 };

    const auto clipped1 = clip(points1);
    const auto clipped2 = clip(points2);

    const detail::vt_geometry expected1{ detail::vt_polygon{ { { { 10, 0 },
                                                                 { 40, 0 },
                                                                 { 40, 10 },
                                                                 { 20, 10 },
                                                                 { 20, 20 },
                                                                 { 30, 20 },
                                                                 { 30, 30 },
                                                                 { 40, 30 },
                                                                 { 40, 40 },
                                                                 { 25, 40 },
                                                                 { 25, 50 },
                                                                 { 10, 50 },
                                                                 { 10, 60 },
                                                                 { 25, 60 },
                                                                 { 10, 24 },
                                                                 { 10, 0 } } } } };

    const detail::vt_geometry expected2{ detail::vt_polygon{
        { { { 10, 0 }, { 40, 0 }, { 40, 10 }, { 10, 10 }, { 10, 0 } } } } };

    ASSERT_EQ(expected1, clipped1);
    ASSERT_EQ(expected2, clipped2);
}

TEST(Clip, Points) {
    const detail::vt_multi_point points1{ { 0, 0 },   { 50, 0 },  { 50, 10 }, { 20, 10 },
                                          { 20, 20 }, { 30, 20 }, { 30, 30 }, { 50, 30 },
                                          { 50, 40 }, { 25, 40 }, { 25, 50 }, { 0, 50 },
                                          { 0, 60 },  { 25, 60 } };

    const detail::vt_multi_point points2{ { 0, 0 }, { 50, 0 }, { 50, 10 }, { 0, 10 } };

    const auto clip = detail::clipper<0>{ 10, 40 };

    const auto clipped1 = clip(points1);
    const auto clipped2 = clip(points2);

    const detail::vt_geometry expected1{ detail::vt_multi_point{
        { { 20, 10 }, { 20, 20 }, { 30, 20 }, { 30, 30 }, { 25, 40 }, { 25, 50 }, { 25, 60 } } } };
    const detail::vt_geometry expected2{ detail::vt_multi_point{ {} } };

    ASSERT_EQ(expected1, clipped1);
    ASSERT_EQ(expected2, clipped2);
}

TEST(GetTile, USStates) {
    const auto geojson = mapbox::geojson::parse(loadFile("test/fixtures/us-states.json"));
    GeoJSONVT index{ geojson.get<mapbox::geojson::feature_collection>() };

    const auto features = index.getTile(7, 37, 48).features;
    const auto expected = parseJSONTile(loadFile("test/fixtures/us-states-z7-37-48.json"));
    ASSERT_EQ(features == expected, true);

    const auto square = parseJSONTile(loadFile("test/fixtures/us-states-square.json"));
    ASSERT_EQ(square == index.getTile(9, 148, 192).features, true);  // clipped square
    ASSERT_EQ(square == index.getTile(11, 592, 768).features, true); // clipped square

    ASSERT_EQ(empty_tile == index.getTile(11, 800, 400), true);   // non-existing tile
    ASSERT_EQ(&empty_tile == &index.getTile(11, 800, 400), true); // non-existing tile

    // This test does not make sense in C++, since the parameters are cast to integers anyway.
    // ASSERT_EQ(isEmpty(index.getTile(-5, 123.25, 400.25)), true); // invalid tile

    ASSERT_EQ(37, index.total);
}

TEST(GetTile, AntimeridianTriangle) {
    const auto geojson = mapbox::geojson::parse(loadFile("test/fixtures/dateline-triangle.json"));

    // Using default options.
    GeoJSONVT index { geojson };

    struct TileCoordinate {
        uint8_t z;
        uint32_t x;
        uint32_t y;
    };

    std::vector<TileCoordinate> tileCoordinates {
        { 1, 0, 0 },
        { 1, 0, 1 },
        { 1, 1, 0 },
        { 1, 1, 1 }
    };

    for (const auto tileCoordinate : tileCoordinates) {
        auto tile = index.getTile(tileCoordinate.z, tileCoordinate.x, tileCoordinate.y);
        ASSERT_EQ(tile.num_points, tile.num_simplified);
        ASSERT_EQ(tile.features.size(), 1);
    }
}

TEST(GetTile, Projection) {
    const auto geojson = mapbox::geojson::parse(loadFile("test/fixtures/linestring.json"));

    mapbox::geojsonvt::Options options;
    options.maxZoom = 20;
    options.extent = 8192;
    options.tolerance = 0;

    GeoJSONVT index{ geojson, options };

    struct TileCoordinate {
        uint8_t z;
        uint32_t x;
        uint32_t y;
    };

    std::vector<TileCoordinate> tileCoordinates{
        { 0, 0, 0 },           { 1, 0, 0 },           { 2, 0, 1 },
        { 3, 1, 3 },           { 4, 2, 6 },           { 5, 5, 12 },
        { 6, 10, 24 },         { 7, 20, 49 },         { 8, 40, 98 },
        { 9, 81, 197 },        { 10, 163, 395 },      { 11, 327, 791 },
        { 12, 655, 1583 },     { 13, 1310, 3166 },    { 14, 2620, 6332 },
        { 15, 5241, 12664 },   { 16, 10482, 25329 },  { 17, 20964, 50660 },
        { 18, 41929, 101320 }, { 19, 83859, 202640 }, { 20, 167719, 405281 },
    };

    for (const auto tileCoordinate : tileCoordinates) {
        auto tile = index.getTile(tileCoordinate.z, tileCoordinate.x, tileCoordinate.y);
        ASSERT_EQ(tile.num_points, tile.num_simplified);
        ASSERT_EQ(tile.features.size(), 1);
        const auto& geometry = tile.features.front().geometry;
        ASSERT_TRUE(geometry.is<mapbox::geometry::line_string<int16_t>>());
        const auto& lineString = geometry.get<mapbox::geometry::line_string<int16_t>>();
        ASSERT_EQ(lineString.size(), 2);

        const double totalFeatures = (1u << tileCoordinate.z) * 8192.0;

        const auto toWebMercatorLon = [&](const mapbox::geometry::point<int16_t>& point) -> double {
            const double x0 = 8192.0 * tileCoordinate.x;
            return (x0 + point.x) * 360.0 / totalFeatures - 180.0;
        };

        const auto toWebMercatorLat = [&](const mapbox::geometry::point<int16_t>& point) -> double {
            const double y0 = 8192.0 * tileCoordinate.y;
            const double y2 = 180.0 - (y0 + point.y) * 360.0 / totalFeatures;
            return 360.0 / M_PI * std::atan(std::exp(y2 * M_PI / 180.0)) - 90.0;
        };

        const auto tolerance = 0.1 / (1 + tileCoordinate.z);

        ASSERT_NEAR(-122.41822421550751, toWebMercatorLon(lineString[0]), tolerance);
        ASSERT_NEAR(37.77852514599172, toWebMercatorLat(lineString[0]), tolerance);

        ASSERT_NEAR(-122.41707086563109, toWebMercatorLon(lineString[1]), tolerance);
        ASSERT_NEAR(37.780424620898664, toWebMercatorLat(lineString[1]), tolerance);
    }
}

std::map<std::string, mapbox::geometry::feature_collection<int16_t>>
genTiles(const std::string& data, uint8_t maxZoom = 0, uint32_t maxPoints = 10000) {
    Options options;
    options.maxZoom = 14;
    options.indexMaxZoom = maxZoom;
    options.indexMaxPoints = maxPoints;

    const auto geojson = mapbox::geojson::parse(data);
    GeoJSONVT index{ geojson, options };

    std::map<std::string, mapbox::geometry::feature_collection<int16_t>> output;

    for (const auto& pair : index.getInternalTiles()) {
        auto& tile = pair.second;
        const std::string key = std::string("z") + std::to_string(tile.z) + "-" +
                                std::to_string(tile.x) + "-" + std::to_string(tile.y);
        output.emplace(key, index.getTile(tile.z, tile.x, tile.y).features);
    }

    return output;
}

struct Arguments {
    Arguments(const std::string inputFile_,
              const std::string expectedFile_,
              const uint32_t maxZoom_ = 0,
              const uint32_t maxPoints_ = 10000)
        : inputFile(inputFile_),
          expectedFile(expectedFile_),
          maxZoom(maxZoom_),
          maxPoints(maxPoints_){};

    const std::string inputFile;
    const std::string expectedFile;
    const uint32_t maxZoom;
    const uint32_t maxPoints;
};

::std::ostream& operator<<(::std::ostream& os, const Arguments& a) {
    return os << a.inputFile << " (" << a.maxZoom << ", " << a.maxPoints << ")";
}

class TileTest : public ::testing::TestWithParam<Arguments> {};

TEST_P(TileTest, Tiles) {
    const auto& params = GetParam();

    const auto actual = genTiles(loadFile(params.inputFile), params.maxZoom, params.maxPoints);
    const auto expected = parseJSONTiles(loadFile(params.expectedFile));

    ASSERT_EQ(expected == actual, true);
}

TEST(GenTiles, InvalidGeoJSON) {
    try {
        genTiles("{\"type\": \"Pologon\"}");
        FAIL() << "Expected exception";
    } catch (const std::runtime_error& ex) {
        ASSERT_STREQ("Pologon geometry must have a coordinates property", ex.what());
    }
}

TEST(GenTiles, EmptyGeoJSON) {
    const auto tiles = genTiles(loadFile("test/fixtures/empty.json"));
    ASSERT_EQ(1, tiles.size());
}

TEST(GenTiles, NoObjectGeoJSON) {
    try {
        genTiles("42");
        FAIL() << "Expected exception";
    } catch (const std::runtime_error& ex) {
        ASSERT_STREQ("GeoJSON must be an object", ex.what());
    }
}

INSTANTIATE_TEST_CASE_P(
    Full,
    TileTest,
    ::testing::ValuesIn(std::vector<Arguments>{
        { "test/fixtures/us-states.json", "test/fixtures/us-states-tiles.json", 7, 200 },
        { "test/fixtures/dateline.json", "test/fixtures/dateline-tiles.json", 7, 200 },
        { "test/fixtures/feature.json", "test/fixtures/feature-tiles.json" },
        { "test/fixtures/collection.json", "test/fixtures/collection-tiles.json" },
        { "test/fixtures/single-geom.json", "test/fixtures/single-geom-tiles.json" } }));
