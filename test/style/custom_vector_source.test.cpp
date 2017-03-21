#include <mbgl/test/util.hpp>

#include <mbgl/style/sources/custom_vector_source.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/geo.hpp>

using namespace mbgl;

TEST(CustomVectorSource, EmptyData) {
    mbgl::style::GeoJSONOptions options;

    const auto callback = [](CanonicalTileID) {};

    auto testSource =
        std::make_unique<mbgl::style::CustomVectorSource>("source", options, callback);

    mbgl::FeatureCollection featureCollection;
    testSource->setTileData(CanonicalTileID(0, 0, 0), mbgl::GeoJSON{ featureCollection });
}

TEST(CustomVectorSource, Geometry) {
    mbgl::style::GeoJSONOptions options;

    const auto callback = [](CanonicalTileID) {};

    auto testSource =
        std::make_unique<mbgl::style::CustomVectorSource>("source", options, callback);

    Polygon<double> polygon = { { { { 0, 0 }, { 0, 45 }, { 45, 45 }, { 45, 0 } } } };
    testSource->setTileData(CanonicalTileID(0, 0, 0), mbgl::GeoJSON{ polygon });
}

TEST(CustomVectorSource, ReloadWithNoTiles) {
    mbgl::style::GeoJSONOptions options;

    bool called = false;
    const auto callback = [&called](CanonicalTileID) { called = true; };

    auto testSource =
        std::make_unique<mbgl::style::CustomVectorSource>("source", options, callback);
    testSource->reloadRegion(mbgl::LatLngBounds::world(), 0);
    EXPECT_EQ(called, false);
}
