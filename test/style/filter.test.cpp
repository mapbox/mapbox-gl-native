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

bool filter(const char * json,
            const PropertyMap& featureProperties = {{}},
            optional<FeatureIdentifier> featureId = {},
            FeatureType featureType = FeatureType::Point,
            GeometryCollection featureGeometry = {},
            float zoom = 0.0f) {
    conversion::Error error;
    optional<Filter> filter = conversion::convertJSON<Filter>(json, error);
    EXPECT_TRUE(bool(filter));
    EXPECT_EQ(error.message, "");
    
    StubGeometryTileFeature feature { featureId, featureType, featureGeometry, featureProperties };
    expression::EvaluationContext context = { zoom, &feature };
    
    return (*filter)(context);
}

TEST(Filter, EqualsString) {
    auto f = R"(["==", "foo", "bar"])";
    ASSERT_TRUE(filter(f, {{ "foo", std::string("bar") }}));
    ASSERT_FALSE(filter(f, {{ "foo", std::string("baz") }}));
}

TEST(Filter, EqualsNumber) {
    auto f = R"(["==", "foo", 0])";
    ASSERT_TRUE(filter(f, {{ "foo", int64_t(0) }}));
    ASSERT_TRUE(filter(f, {{ "foo", uint64_t(0) }}));
    ASSERT_TRUE(filter(f, {{ "foo", double(0) }}));
    ASSERT_FALSE(filter(f, {{ "foo", int64_t(1) }}));
    ASSERT_FALSE(filter(f, {{ "foo", uint64_t(1) }}));
    ASSERT_FALSE(filter(f, {{ "foo", double(1) }}));
    ASSERT_FALSE(filter(f, {{ "foo", std::string("0") }}));
    ASSERT_FALSE(filter(f, {{ "foo", false }}));
    ASSERT_FALSE(filter(f, {{ "foo", true }}));
    ASSERT_FALSE(filter(f, {{ "foo", mapbox::geometry::null_value }}));
    ASSERT_FALSE(filter(f, {{}}));
}

TEST(Filter, EqualsType) {
    auto f = R"(["==", "$type", "LineString"])";
    ASSERT_FALSE(filter(f, {{}}, {}, FeatureType::Point, {}));
    ASSERT_TRUE(filter(f, {{}}, {}, FeatureType::LineString, {}));
}

TEST(Filter, InType) {
    auto f = R"(["in", "$type", "LineString", "Polygon"])";
    ASSERT_FALSE(filter(f, {{}}, {}, FeatureType::Point));
    ASSERT_TRUE(filter(f, {{}}, {}, FeatureType::LineString));
    ASSERT_TRUE(filter(f, {{}}, {}, FeatureType::Polygon));
}

TEST(Filter, Any) {
    ASSERT_FALSE(filter("[\"any\"]"));
    ASSERT_TRUE(filter("[\"any\", [\"==\", \"foo\", 1]]", {{ std::string("foo"), int64_t(1) }}));
    ASSERT_FALSE(filter("[\"any\", [\"==\", \"foo\", 0]]", {{ std::string("foo"), int64_t(1) }}));
    ASSERT_TRUE(filter("[\"any\", [\"==\", \"foo\", 0], [\"==\", \"foo\", 1]]", {{ std::string("foo"), int64_t(1) }}));
}

TEST(Filter, All) {
    ASSERT_TRUE(filter("[\"all\"]", {{}}));
    ASSERT_TRUE(filter("[\"all\", [\"==\", \"foo\", 1]]", {{ std::string("foo"), int64_t(1) }}));
    ASSERT_FALSE(filter("[\"all\", [\"==\", \"foo\", 0]]", {{ std::string("foo"), int64_t(1) }}));
    ASSERT_FALSE(filter("[\"all\", [\"==\", \"foo\", 0], [\"==\", \"foo\", 1]]", {{ std::string("foo"), int64_t(1) }}));
}

TEST(Filter, None) {
    ASSERT_TRUE(filter("[\"none\"]"));
    ASSERT_FALSE(filter("[\"none\", [\"==\", \"foo\", 1]]", {{ std::string("foo"), int64_t(1) }}));
    ASSERT_TRUE(filter("[\"none\", [\"==\", \"foo\", 0]]", {{ std::string("foo"), int64_t(1) }}));
    ASSERT_FALSE(filter("[\"none\", [\"==\", \"foo\", 0], [\"==\", \"foo\", 1]]", {{ std::string("foo"), int64_t(1) }}));
}

TEST(Filter, Has) {
    ASSERT_TRUE(filter("[\"has\", \"foo\"]", {{ std::string("foo"), int64_t(1) }}));
    ASSERT_TRUE(filter("[\"has\", \"foo\"]", {{ std::string("foo"), int64_t(0) }}));
    ASSERT_TRUE(filter("[\"has\", \"foo\"]", {{ std::string("foo"), false }}));
    ASSERT_FALSE(filter("[\"has\", \"foo\"]"));
}

TEST(Filter, NotHas) {
    ASSERT_FALSE(filter("[\"!has\", \"foo\"]", {{ std::string("foo"), int64_t(1) }}));
    ASSERT_FALSE(filter("[\"!has\", \"foo\"]", {{ std::string("foo"), int64_t(0) }}));
    ASSERT_FALSE(filter("[\"!has\", \"foo\"]", {{ std::string("foo"), false }}));
    ASSERT_TRUE(filter("[\"!has\", \"foo\"]"));
}

TEST(Filter, ID) {
    FeatureIdentifier id1 { uint64_t{ 1234 } };
    ASSERT_TRUE(filter("[\"==\", \"$id\", 1234]", {{}}, id1));
    ASSERT_FALSE(filter("[\"==\", \"$id\", \"1234\"]", {{}}, id1));
    
    ASSERT_FALSE(filter("[\"==\", \"$id\", 1234]", {{ "id", uint64_t(1234) }}));
}

TEST(Filter, Expression) {
    ASSERT_TRUE(filter("[\"==\", [\"+\", 1, 1], 2]"));
    ASSERT_FALSE(filter("[\"==\", [\"+\", 1, 1], 4]"));
}

TEST(Filter, PropertyExpression) {
    ASSERT_TRUE(filter("[\"==\", [\"get\", \"two\"], 2]", {{"two", int64_t(2)}}));
    ASSERT_FALSE(filter("[\"==\", [\"get\", \"two\"], 4]", {{"two", int64_t(2)}}));
}

TEST(Filter, ZoomExpressionNested) {
    ASSERT_TRUE(filter(R"(["==", ["get", "two"], ["zoom"]])", {{"two", int64_t(2)}}, {}, FeatureType::Point, {}, 2.0f));
    ASSERT_FALSE(filter(R"(["==", ["get", "two"], ["+", ["zoom"], 1]])", {{"two", int64_t(2)}}, {}, FeatureType::Point, {}, 2.0f));
}
