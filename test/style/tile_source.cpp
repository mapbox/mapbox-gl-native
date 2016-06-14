#include <mbgl/test/util.hpp>

#include <mbgl/style/tile_source.hpp>
#include <mbgl/util/io.hpp>

using namespace mbgl;
using namespace mbgl::style;

TEST(TileSource, ParseTileJSONRaster) {
    auto result = TileSource::parseTileJSON(
        util::read_file("test/fixtures/style_parser/tilejson.raster.json"),
        "mapbox://mapbox.satellite",
        SourceType::Raster,
        256);

    EXPECT_EQ(0, result.zoomRange.min);
    EXPECT_EQ(15, result.zoomRange.max);
    EXPECT_EQ("attribution", result.attribution);
#if !defined(__ANDROID__) && !defined(__APPLE__)
    EXPECT_EQ("mapbox://tiles/mapbox.satellite/{z}/{x}/{y}{ratio}.webp", result.tiles[0]);
#else
    EXPECT_EQ("mapbox://tiles/mapbox.satellite/{z}/{x}/{y}{ratio}.png", result.tiles[0]);
#endif
}

TEST(TileSource, ParseTileJSONVector) {
    auto result = TileSource::parseTileJSON(
        util::read_file("test/fixtures/style_parser/tilejson.vector.json"),
        "mapbox://mapbox.streets",
        SourceType::Vector,
        256);

    EXPECT_EQ(0, result.zoomRange.min);
    EXPECT_EQ(15, result.zoomRange.max);
    EXPECT_EQ("attribution", result.attribution);
    EXPECT_EQ("mapbox://tiles/mapbox.streets/{z}/{x}/{y}.vector.pbf", result.tiles[0]);
}
