#include <mbgl/test/util.hpp>

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/geojson_options.hpp>
#include <mbgl/test/conversion_stubs.hpp>

#include <mbgl/util/logging.hpp>

using namespace mbgl::style;
using namespace mbgl::style::conversion;

TEST(GeoJSONOptions, Basic) {
    ValueMap map;
    Value raw(map);
    Error error;
    mbgl::optional<GeoJSONOptions> converted = convert<GeoJSONOptions>(raw, error);
    ASSERT_TRUE((bool) converted);
}

TEST(GeoJSONOptions, ErrorHandling) {
    ValueMap map {{"maxzoom", std::string{"should not be a string"}}};
    Value raw(map);
    Error error;
    mbgl::optional<GeoJSONOptions> converted = convert<GeoJSONOptions>(raw, error);
    ASSERT_FALSE((bool) converted);
}

TEST(GeoJSONOptions, RetainsDefaults) {
    ValueMap map;
    Value raw(map);
    Error error;
    GeoJSONOptions converted = *convert<GeoJSONOptions>(raw, error);
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
    ValueMap map {
        // GeoJSON-VT
        {"maxzoom", 1.0f},
        {"buffer", 2.0f},
        {"tolerance", 3.0f},

        // Supercluster
        {"cluster", true},
        {"clusterRadius", 4.0f},
        {"clusterMaxZoom", 5.0f}
    };
    Value raw(map);
    Error error;
    GeoJSONOptions converted = *convert<GeoJSONOptions>(raw, error);

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
