#include <mbgl/test/util.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/test/stub_geometry_tile_feature.hpp>

#include <mbgl/style/filter.hpp>
#include <mbgl/style/filter_evaluator.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/filter.hpp>

using namespace mbgl;
using namespace mbgl::style;

Filter parse(const char * expression) {
    conversion::Error error;
    optional<Filter> filter = conversion::convertJSON<Filter>(expression, error);
    EXPECT_TRUE(bool(filter));
    return *filter;
}

StubGeometryTileFeature feature(const PropertyMap& properties) {
    return StubGeometryTileFeature { properties };
}

TEST(Filter, EqualsString) {
    Filter f = parse(R"(["==", "foo", "bar"])");
    ASSERT_TRUE(f(feature({{ "foo", std::string("bar") }})));
    ASSERT_FALSE(f(feature({{ "foo", std::string("baz") }})));
}

TEST(Filter, EqualsNumber) {
    Filter f = parse(R"(["==", "foo", 0])");
    ASSERT_TRUE(f(feature({{ "foo", int64_t(0) }})));
    ASSERT_TRUE(f(feature({{ "foo", uint64_t(0) }})));
    ASSERT_TRUE(f(feature({{ "foo", double(0) }})));
    ASSERT_FALSE(f(feature({{ "foo", int64_t(1) }})));
    ASSERT_FALSE(f(feature({{ "foo", uint64_t(1) }})));
    ASSERT_FALSE(f(feature({{ "foo", double(1) }})));
    ASSERT_FALSE(f(feature({{ "foo", std::string("0") }})));
    ASSERT_FALSE(f(feature({{ "foo", false }})));
    ASSERT_FALSE(f(feature({{ "foo", true }})));
    ASSERT_FALSE(f(feature({{ "foo", mapbox::geometry::null_value }})));
    ASSERT_FALSE(f(feature({{}})));
}

TEST(Filter, EqualsType) {
    Filter f = parse(R"(["==", "$type", "LineString"])");
    ASSERT_FALSE(f(StubGeometryTileFeature({}, FeatureType::Point, {}, {{}})));
    ASSERT_TRUE(f(StubGeometryTileFeature({}, FeatureType::LineString, {}, {{}})));
}

TEST(Filter, InType) {
    Filter f = parse(R"(["in", "$type", "LineString", "Polygon"])");
    ASSERT_FALSE(f(StubGeometryTileFeature({}, FeatureType::Point, {}, {{}})));
    ASSERT_TRUE(f(StubGeometryTileFeature({}, FeatureType::LineString, {}, {{}})));
    ASSERT_TRUE(f(StubGeometryTileFeature({}, FeatureType::Polygon, {}, {{}})));
}

TEST(Filter, Any) {
    ASSERT_FALSE(parse("[\"any\"]")(feature({{}})));
    ASSERT_TRUE(parse("[\"any\", [\"==\", \"foo\", 1]]")(
                         feature({{ std::string("foo"), int64_t(1) }})));
    ASSERT_FALSE(parse("[\"any\", [\"==\", \"foo\", 0]]")(
                         feature({{ std::string("foo"), int64_t(1) }})));
    ASSERT_TRUE(parse("[\"any\", [\"==\", \"foo\", 0], [\"==\", \"foo\", 1]]")(
                         feature({{ std::string("foo"), int64_t(1) }})));
}

TEST(Filter, All) {
    ASSERT_TRUE(parse("[\"all\"]")(feature({{}})));
    ASSERT_TRUE(parse("[\"all\", [\"==\", \"foo\", 1]]")(
                         feature({{ std::string("foo"), int64_t(1) }})));
    ASSERT_FALSE(parse("[\"all\", [\"==\", \"foo\", 0]]")(
                         feature({{ std::string("foo"), int64_t(1) }})));
    ASSERT_FALSE(parse("[\"all\", [\"==\", \"foo\", 0], [\"==\", \"foo\", 1]]")(
                         feature({{ std::string("foo"), int64_t(1) }})));
}

TEST(Filter, None) {
    ASSERT_TRUE(parse("[\"none\"]")(feature({{}})));
    ASSERT_FALSE(parse("[\"none\", [\"==\", \"foo\", 1]]")(
                         feature({{ std::string("foo"), int64_t(1) }})));
    ASSERT_TRUE(parse("[\"none\", [\"==\", \"foo\", 0]]")(
                         feature({{ std::string("foo"), int64_t(1) }})));
    ASSERT_FALSE(parse("[\"none\", [\"==\", \"foo\", 0], [\"==\", \"foo\", 1]]")(
                         feature({{ std::string("foo"), int64_t(1) }})));
}

TEST(Filter, Has) {
    ASSERT_TRUE(parse("[\"has\", \"foo\"]")(
                          feature({{ std::string("foo"), int64_t(1) }})));
    ASSERT_TRUE(parse("[\"has\", \"foo\"]")(
                          feature({{ std::string("foo"), int64_t(0) }})));
    ASSERT_TRUE(parse("[\"has\", \"foo\"]")(
                          feature({{ std::string("foo"), false }})));
    ASSERT_FALSE(parse("[\"has\", \"foo\"]")(
                          feature({{}})));
}

TEST(Filter, NotHas) {
    ASSERT_FALSE(parse("[\"!has\", \"foo\"]")(
                          feature({{ std::string("foo"), int64_t(1) }})));
    ASSERT_FALSE(parse("[\"!has\", \"foo\"]")(
                          feature({{ std::string("foo"), int64_t(0) }})));
    ASSERT_FALSE(parse("[\"!has\", \"foo\"]")(
                          feature({{ std::string("foo"), false }})));
    ASSERT_TRUE(parse("[\"!has\", \"foo\"]")(
                          feature({{}})));
}

TEST(Filter, ID) {
    StubGeometryTileFeature feature1 { FeatureIdentifier{ uint64_t{ 1234 } }, {}, {}, {{}}};

    ASSERT_TRUE(parse("[\"==\", \"$id\", 1234]")(feature1));
    ASSERT_FALSE(parse("[\"==\", \"$id\", \"1234\"]")(feature1));
    
    StubGeometryTileFeature feature2 { {{ "id", uint64_t(1234) }} };

    ASSERT_FALSE(parse("[\"==\", \"$id\", 1234]")(feature2));
}
