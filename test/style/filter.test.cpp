#include <mbgl/test/util.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/test/stub_geometry_tile_feature.hpp>

#include <mbgl/util/rapidjson.hpp>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <mbgl/style/conversion/stringify.hpp>

#include <mbgl/style/filter.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/filter.hpp>
#include <mbgl/util/rapidjson.hpp>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

using namespace mbgl;
using namespace mbgl::style;

bool filter(const char * json,
            const PropertyMap& featureProperties = {{}},
            FeatureIdentifier featureId = {},
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

void invalidFilter(const char * json) {
    conversion::Error error;
    optional<Filter> filter = conversion::convertJSON<Filter>(json, error);
    EXPECT_FALSE(bool(filter));
    EXPECT_NE(error.message, "");
}

void writeJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer, const Value& value) {
    value.match(
        [&] (const NullValue&) { writer.Null(); },
        [&] (bool b) { writer.Bool(b); },
        [&] (uint64_t t) { writer.Uint64(t); },
        [&] (int64_t t) { writer.Int64(t); },
        [&] (double f) {
            // make sure integer values are stringified without trailing ".0".
            f == std::floor(f) ? writer.Int(f) : writer.Double(f);
        },
        [&] (const std::string& s) { writer.String(s); },
        [&] (const std::vector<Value>& arr) {
            writer.StartArray();
            for(const auto& item : arr) {
                writeJSON(writer, item);
            }
            writer.EndArray();
        },
        [](const auto&) {
        });
}

std::string stringifyFilter(const char * json) {
    conversion::Error error;
    optional<Filter> filter = conversion::convertJSON<Filter>(json, error);
    EXPECT_TRUE(bool(filter));
    EXPECT_EQ(error.message, "");

    auto value = filter->serialize();

    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
    writeJSON(writer, value);
    return s.GetString();
}

TEST(Filter, EqualsNull) {
    auto f = R"(["==", "foo", null])";
    ASSERT_TRUE(filter(f, {{ "foo", mapbox::feature::null_value }}));

    ASSERT_FALSE(filter(f, {{ "foo", int64_t(0) }}));
    ASSERT_FALSE(filter(f, {{ "foo", int64_t(1) }}));
    ASSERT_FALSE(filter(f, {{ "foo", std::string("0") }}));
    ASSERT_FALSE(filter(f, {{ "foo", true }}));
    ASSERT_FALSE(filter(f, {{ "foo", false }}));
    ASSERT_FALSE(filter(f, {{ }}));
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
    ASSERT_FALSE(filter(f, {{ "foo", mapbox::feature::null_value }}));
    ASSERT_FALSE(filter(f, {{}}));
}

TEST(Filter, EqualsType) {
    auto f = R"(["==", "$type", "LineString"])";
    ASSERT_FALSE(filter(f, {{}}, {}, FeatureType::Point, {}));
    ASSERT_TRUE(filter(f, {{}}, {}, FeatureType::LineString, {}));
    ASSERT_FALSE(filter(f, {{}}, {}, FeatureType::Point, {}));

    invalidFilter(R"(["==", "$type"])");
    invalidFilter(R"(["==", "$type", null])");
    invalidFilter(R"(["==", "$type", "foo", 1])");
    invalidFilter(R"(["==", "$type", "foo", "Point"])");
}

TEST(Filter, InType) {
    auto f = R"(["in", "$type", "LineString", "Polygon"])";
    ASSERT_FALSE(filter(f, {{}}, {}, FeatureType::Point));
    ASSERT_TRUE(filter(f, {{}}, {}, FeatureType::LineString));
    ASSERT_TRUE(filter(f, {{}}, {}, FeatureType::Polygon));
}

TEST(Filter, InID) {
    auto f = R"(["in", "$id", "123", "1234", 1234])";
    ASSERT_FALSE(filter(f));
    ASSERT_TRUE(filter(f, {{}}, { uint64_t(1234) }));
    ASSERT_TRUE(filter(f, {{}}, { std::string("1234") }));
    ASSERT_FALSE(filter(f, {{}}, { std::string("4321") }));
}

TEST(Filter, Any) {
    ASSERT_FALSE(filter("[\"any\"]"));
    ASSERT_TRUE(filter("[\"any\", [\"==\", \"foo\", 1]]", {{ std::string("foo"), int64_t(1) }}));
    ASSERT_FALSE(filter("[\"any\", [\"==\", \"foo\", 0]]", {{ std::string("foo"), int64_t(1) }}));
    ASSERT_TRUE(filter("[\"any\", [\"==\", \"foo\", 0], [\"==\", \"foo\", 1]]", {{ std::string("foo"), int64_t(1) }}));
}

TEST(Filter, AnyExpression) {
    ASSERT_FALSE(filter("[\"any\"]"));
    ASSERT_TRUE(filter("[\"any\", [\"==\", [\"get\", \"foo\"], 1]]", {{ std::string("foo"), int64_t(1) }}));
    ASSERT_FALSE(filter("[\"any\", [\"==\", [\"get\", \"foo\"], 0]]", {{ std::string("foo"), int64_t(1) }}));
    ASSERT_TRUE(filter("[\"any\", [\"==\", [\"get\", \"foo\"], 0], [\"==\", [\"get\", \"foo\"], 1]]", {{ std::string("foo"), int64_t(1) }}));
}

TEST(Filter, All) {
    ASSERT_TRUE(filter("[\"all\"]", {{}}));
    ASSERT_TRUE(filter("[\"all\", [\"==\", \"foo\", 1]]", {{ std::string("foo"), int64_t(1) }}));
    ASSERT_FALSE(filter("[\"all\", [\"==\", \"foo\", 0]]", {{ std::string("foo"), int64_t(1) }}));
    ASSERT_FALSE(filter("[\"all\", [\"==\", \"foo\", 0], [\"==\", \"foo\", 1]]", {{ std::string("foo"), int64_t(1) }}));
}

TEST(Filter, AllExpression) {
    ASSERT_TRUE(filter("[\"all\", [\"==\", [\"get\", \"foo\"], 1]]", {{ std::string("foo"), int64_t(1) }}));
    ASSERT_FALSE(filter("[\"all\", [\"==\", [\"get\", \"foo\"], 0]]", {{ std::string("foo"), int64_t(1) }}));
    ASSERT_FALSE(filter("[\"all\", [\"==\", [\"get\", \"foo\"], 0], [\"==\", [\"get\", \"foo\"], 1]]", {{ std::string("foo"), int64_t(1) }}));
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
    ASSERT_FALSE(filter("[\"has\", \"$id\"]"));
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

    FeatureIdentifier id2 { std::string{ "1" } };
    ASSERT_FALSE(filter("[\"<\", \"$id\", \"0\"]", {{}}, id2));
    ASSERT_TRUE(filter("[\"<\", \"$id\", \"1234\"]", {{}}, id2));

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

TEST(Filter, LegacyProperty) {
    ASSERT_TRUE(filter("[\"<=\", \"two\", 2]", {{"two", int64_t(2)}}));
    ASSERT_FALSE(filter("[\"==\", \"two\", 4]", {{"two", int64_t(2)}}));

    ASSERT_FALSE(filter("[\"<=\", \"two\", \"2\"]", {{"two", int64_t(2)}}));
    ASSERT_FALSE(filter("[\"==\", \"bool\", false]", {{"two", true}}));

    ASSERT_TRUE(filter("[\"<=\", \"two\", \"2\"]", {{"two", std::string("2")}}));
    ASSERT_FALSE(filter("[\"<\", \"two\", \"1\"]", {{"two", std::string("2")}}));
    ASSERT_FALSE(filter("[\"==\", \"two\", 4]", {{"two", std::string("2")}}));
}

TEST(Filter, Serialize) {
    std::string json = R"(["any",["==","foo",0],["==","foo",1]])";
    EXPECT_EQ(stringifyFilter(json.c_str()), std::string(json));
    
    json = R"(["<=","two",2])";
    EXPECT_EQ(stringifyFilter(json.c_str()), std::string(json));

    //Constant folding for Expression filters
    json = R"(["==",["+",1,1],2])";
    EXPECT_EQ(stringifyFilter(json.c_str()), std::string("true"));

    json = R"(["all",["==",["get","foo"],0],["==",["get","foo"],1]])";
    EXPECT_EQ(stringifyFilter(json.c_str()), std::string(json));
}

TEST(Filter, ExpressionLegacyMix) {
    conversion::Error error;
    optional<Filter> filter = conversion::convertJSON<Filter>(R"(["any", ["all", ["==", ["geometry-type"], "LineString"]], ["==", "x", 1]])", error);
    EXPECT_FALSE(bool(filter));
    EXPECT_TRUE(error.message.size() > 0);
}

TEST(Filter, ZoomExpressionNested) {
    ASSERT_TRUE(filter(R"(["==", ["get", "two"], ["zoom"]])", {{"two", int64_t(2)}}, {}, FeatureType::Point, {}, 2.0f));
    ASSERT_FALSE(filter(R"(["==", ["get", "two"], ["+", ["zoom"], 1]])", {{"two", int64_t(2)}}, {}, FeatureType::Point, {}, 2.0f));
}

TEST(Filter, Internal) {
    filter(R"(["filter-==","class","snow"])");
}

TEST(Filter, Short) {
    filter(R"(["==", ["id"], "foo"])");
}

TEST(Filter, LegacyExpressionInvalidType) {
    const JSValue value("string");
    conversion::Error error;
    optional<Filter> result = conversion::convert<Filter>(conversion::Convertible(&value), error);
    EXPECT_FALSE(result);
}
