#include <mbgl/test/util.hpp>

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/geojson_options.hpp>
#include <mbgl/test/conversion_stubs.hpp>

#include <mbgl/platform/log.hpp>

using namespace mbgl::style;
using namespace mbgl::style::conversion;

TEST(GeoJSONOptions, Basic) {
    ValueMap map;
    Value raw(map);
    Result<GeoJSONOptions> converted = convert<GeoJSONOptions>(raw);
    ASSERT_TRUE((bool) converted);
}

TEST(GeoJSONOptions, ErrorHandling) {
    ValueMap map {{"maxzoom", "should not be a string"}};
    Value raw(map);
    Result<GeoJSONOptions> converted = convert<GeoJSONOptions>(raw);
    ASSERT_FALSE((bool) converted);
}

TEST(GeoJSONOptions, RetainsDefaults) {
    ValueMap map;
    Value raw(map);
    GeoJSONOptions converted = *convert<GeoJSONOptions>(raw);
    GeoJSONOptions defaults;
    
    //GeoJSON-VT
    ASSERT_EQ(converted.maxzoom, defaults.maxzoom);
    ASSERT_EQ(converted.buffer, defaults.buffer);
    ASSERT_EQ(converted.tolerance, defaults.tolerance);
    
    //Supercluster
    ASSERT_EQ(converted.cluster, defaults.cluster);
    ASSERT_EQ(converted.clusterRadius, defaults.clusterRadius);
    ASSERT_EQ(converted.clusterMaxZoom, defaults.clusterMaxZoom);
}


TEST(GeoJSONOptions, FullConversion) {
    ValueMap map {
        //GeoJSON-VT
        {"maxzoom", 1},
        {"buffer", 2},
        {"tolerance", 3},
        
        //Supercluster
        {"cluster", true},
        {"clusterRadius", 4},
        {"clusterMaxZoom", 5}
    };
    Value raw(map);
    GeoJSONOptions converted = *convert<GeoJSONOptions>(raw);
    
    //GeoJSON-VT
    ASSERT_EQ(converted.maxzoom, 1);
    ASSERT_EQ(converted.buffer, 2);
    ASSERT_EQ(converted.tolerance, 3);
    
    //Supercluster
    ASSERT_EQ(converted.cluster, true);
    ASSERT_EQ(converted.clusterRadius, 4);
    ASSERT_EQ(converted.clusterMaxZoom, 5);
}
