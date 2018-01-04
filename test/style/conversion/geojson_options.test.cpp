#include <mbgl/test/util.hpp>

#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/geojson_options.hpp>

#include <mbgl/util/logging.hpp>

using namespace mbgl::style;
using namespace mbgl::style::conversion;

TEST(GeoJSONOptions, Basic) {
    Error error;
    mbgl::optional<GeoJSONOptions> converted = convertJSON<GeoJSONOptions>("{}", error);
    ASSERT_TRUE((bool) converted);
}

TEST(GeoJSONOptions, ErrorHandling) {
    Error error;
    mbgl::optional<GeoJSONOptions> converted = convertJSON<GeoJSONOptions>(R"JSON({
        "maxzoom": "should not be a string"
    })JSON", error);
    ASSERT_FALSE((bool) converted);
}

TEST(GeoJSONOptions, RetainsDefaults) {
    Error error;
    GeoJSONOptions converted = *convertJSON<GeoJSONOptions>("{}", error);
    GeoJSONOptions defaults;

    // GeoJSON-VT
    ASSERT_EQ(converted.minzoom, defaults.minzoom);
    ASSERT_EQ(converted.maxzoom, defaults.maxzoom);
    ASSERT_EQ(converted.buffer, defaults.buffer);
    ASSERT_EQ(converted.tolerance, defaults.tolerance);

    // Supercluster
    ASSERT_EQ(converted.cluster, defaults.cluster);
    ASSERT_EQ(converted.clusterRadius, defaults.clusterRadius);
    ASSERT_EQ(converted.clusterMaxZoom, defaults.clusterMaxZoom);
}

TEST(GeoJSONOptions, FullConversion) {
    Error error;
    GeoJSONOptions converted = *convertJSON<GeoJSONOptions>(R"JSON({
        "maxzoom": 1,
        "buffer": 2,
        "tolerance": 3,
        "cluster": true,
        "clusterRadius": 4,
        "clusterMaxZoom": 5
    })JSON", error);

    // GeoJSON-VT
    ASSERT_EQ(converted.minzoom, 0);
    ASSERT_EQ(converted.maxzoom, 1);
    ASSERT_EQ(converted.buffer, 2);
    ASSERT_EQ(converted.tolerance, 3);

    // Supercluster
    ASSERT_EQ(converted.cluster, true);
    ASSERT_EQ(converted.clusterRadius, 4);
    ASSERT_EQ(converted.clusterMaxZoom, 5);
}
