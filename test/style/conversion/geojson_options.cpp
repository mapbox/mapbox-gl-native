#include <mbgl/test/util.hpp>

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/geojson_options.hpp>
#include <mbgl/test/conversion_stubs.hpp>

#include <mbgl/platform/log.hpp>

using namespace mbgl::style;
using namespace mbgl::style::conversion;

TEST(GeoJSONOptions, Basic) {
    HolderMap map;
    Holder raw(map);
    Result<GeoJSONOptions> converted = convert<GeoJSONOptions>(raw);
    ASSERT_TRUE((bool) converted);
}

TEST(GeoJSONOptions, ErrorHandling) {
    HolderMap map {{"maxzoom","should not be a string"}};
    Holder raw(map);
    Result<GeoJSONOptions> converted = convert<GeoJSONOptions>(raw);
    ASSERT_FALSE((bool) converted);
}
