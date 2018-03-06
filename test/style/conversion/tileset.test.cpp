#include <mbgl/test/util.hpp>

#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/tileset.hpp>

#include <mbgl/util/logging.hpp>

using namespace mbgl;
using namespace mbgl::style::conversion;

TEST(Tileset, Empty) {
    Error error;
    mbgl::optional<Tileset> converted = convertJSON<Tileset>("{}", error);
    EXPECT_FALSE((bool) converted);
}

TEST(Tileset, ErrorHandling) {
    Error error;
    mbgl::optional<Tileset> converted = convertJSON<Tileset>(R"JSON({
        "tiles": "should not be a string"
    })JSON", error);
    EXPECT_FALSE((bool) converted);
}

TEST(Tileset, InvalidBounds) {
    {
        Error error;
        mbgl::optional<Tileset> converted = convertJSON<Tileset>(R"JSON({
            "tiles": ["http://mytiles"],
            "bounds": [73, -180, -73, -120]
        })JSON", error);

        EXPECT_FALSE((bool) converted);
    }
    {
        Error error;
        mbgl::optional<Tileset> converted = convertJSON<Tileset>(R"JSON({
            "tiles": ["http://mytiles"],
            "bounds": [-120]
        })JSON", error);

        EXPECT_FALSE((bool) converted);
    }
    {
        Error error;
        mbgl::optional<Tileset> converted = convertJSON<Tileset>(R"JSON({
            "tiles": ["http://mytiles"],
            "bounds": "should not be a string"
        })JSON", error);

        EXPECT_FALSE((bool) converted);
    }
}

TEST(Tileset, ValidWorldBounds) {
    Error error;
    mbgl::optional<Tileset> converted = convertJSON<Tileset>(R"JSON({
        "tiles": ["http://mytiles"],
        "bounds": [-180, -90, 180, 90]
    })JSON", error);
    EXPECT_TRUE((bool) converted);
    EXPECT_EQ(converted->bounds, LatLngBounds::hull({90, -180}, {-90, 180}));
}

TEST(Tileset, FullConversion) {
    Error error;
    Tileset converted = *convertJSON<Tileset>(R"JSON({
        "tiles": ["http://mytiles"],
        "scheme": "xyz",
        "minzoom": 1,
        "maxzoom": 2,
        "attribution": "mapbox",
        "bounds": [-180, -73, -120, 73]
    })JSON", error);

    EXPECT_EQ(converted.tiles[0], "http://mytiles");
    EXPECT_EQ(converted.scheme, Tileset::Scheme::XYZ);
    EXPECT_EQ(converted.zoomRange.min, 1);
    EXPECT_EQ(converted.zoomRange.max, 2);
    EXPECT_EQ(converted.attribution, "mapbox");
    EXPECT_EQ(converted.bounds, LatLngBounds::hull({73, -180}, {-73, -120}));
}
