#include <mbgl/test/util.hpp>

#include <mbgl/style/filter.hpp>
#include <mbgl/style/filter_evaluator.hpp>
#include <mbgl/style/rapidjson_conversion.hpp>
#include <mbgl/style/conversion.hpp>

#include <rapidjson/document.h>

#include <map>

using namespace mbgl;
using namespace mbgl::style;

typedef std::multimap<std::string, mbgl::Value> Properties;

Filter parse(const char * expression) {
    rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> doc;
    doc.Parse<0>(expression);
    return conversion::convertFilter(doc).get<Filter>();
}

bool evaluate(const Filter& filter, const Properties& properties, FeatureType type = FeatureType::Unknown) {
    return filter(type, [&] (const std::string& key) -> optional<Value> {
        auto it = properties.find(key);
        if (it == properties.end())
            return {};
        return it->second;
    });
}

TEST(Filter, EqualsString) {
    Filter f = parse("[\"==\", \"foo\", \"bar\"]");
    ASSERT_TRUE(evaluate(f, {{ "foo", std::string("bar") }}));
    ASSERT_FALSE(evaluate(f, {{ "foo", std::string("baz") }}));
}

TEST(Filter, EqualsNumber) {
    Filter f = parse("[\"==\", \"foo\", 0]");
    ASSERT_TRUE(evaluate(f, {{ "foo", int64_t(0) }}));
    ASSERT_TRUE(evaluate(f, {{ "foo", uint64_t(0) }}));
    ASSERT_TRUE(evaluate(f, {{ "foo", double(0) }}));
    ASSERT_FALSE(evaluate(f, {{ "foo", int64_t(1) }}));
    ASSERT_FALSE(evaluate(f, {{ "foo", uint64_t(1) }}));
    ASSERT_FALSE(evaluate(f, {{ "foo", double(1) }}));
    ASSERT_FALSE(evaluate(f, {{ "foo", std::string("0") }}));
    ASSERT_FALSE(evaluate(f, {{ "foo", false }}));
    ASSERT_FALSE(evaluate(f, {{ "foo", true }}));
    ASSERT_FALSE(evaluate(f, {{}}));
}

TEST(Filter, EqualsType) {
    Filter f = parse("[\"==\", \"$type\", \"LineString\"]");
    ASSERT_FALSE(evaluate(f, {{}}, FeatureType::Point));
    ASSERT_TRUE(evaluate(f, {{}}, FeatureType::LineString));
}

TEST(Filter, InType) {
    Filter f = parse("[\"in\", \"$type\", \"LineString\", \"Polygon\"]");
    ASSERT_FALSE(evaluate(f, {{}}, FeatureType::Point));
    ASSERT_TRUE(evaluate(f, {{}}, FeatureType::LineString));
    ASSERT_TRUE(evaluate(f, {{}}, FeatureType::Polygon));
}

TEST(Filter, Any) {
    ASSERT_FALSE(evaluate(parse("[\"any\"]"), {{}}));
    ASSERT_TRUE(evaluate(parse("[\"any\", [\"==\", \"foo\", 1]]"),
                         {{ std::string("foo"), int64_t(1) }}));
    ASSERT_FALSE(evaluate(parse("[\"any\", [\"==\", \"foo\", 0]]"),
                          {{ std::string("foo"), int64_t(1) }}));
    ASSERT_TRUE(evaluate(parse("[\"any\", [\"==\", \"foo\", 0], [\"==\", \"foo\", 1]]"),
                         {{ std::string("foo"), int64_t(1) }}));
}

TEST(Filter, All) {
    ASSERT_TRUE(evaluate(parse("[\"all\"]"), {{}}));
    ASSERT_TRUE(evaluate(parse("[\"all\", [\"==\", \"foo\", 1]]"),
                         {{ std::string("foo"), int64_t(1) }}));
    ASSERT_FALSE(evaluate(parse("[\"all\", [\"==\", \"foo\", 0]]"),
                          {{ std::string("foo"), int64_t(1) }}));
    ASSERT_FALSE(evaluate(parse("[\"all\", [\"==\", \"foo\", 0], [\"==\", \"foo\", 1]]"),
                          {{ std::string("foo"), int64_t(1) }}));
}

TEST(Filter, None) {
    ASSERT_TRUE(evaluate(parse("[\"none\"]"), {{}}));
    ASSERT_FALSE(evaluate(parse("[\"none\", [\"==\", \"foo\", 1]]"),
                          {{ std::string("foo"), int64_t(1) }}));
    ASSERT_TRUE(evaluate(parse("[\"none\", [\"==\", \"foo\", 0]]"),
                         {{ std::string("foo"), int64_t(1) }}));
    ASSERT_FALSE(evaluate(parse("[\"none\", [\"==\", \"foo\", 0], [\"==\", \"foo\", 1]]"),
                          {{ std::string("foo"), int64_t(1) }}));
}

TEST(Filter, Has) {
    ASSERT_TRUE(evaluate(parse("[\"has\", \"foo\"]"),
                          {{ std::string("foo"), int64_t(1) }}));
    ASSERT_TRUE(evaluate(parse("[\"has\", \"foo\"]"),
                          {{ std::string("foo"), int64_t(0) }}));
    ASSERT_TRUE(evaluate(parse("[\"has\", \"foo\"]"),
                          {{ std::string("foo"), false }}));
    ASSERT_FALSE(evaluate(parse("[\"has\", \"foo\"]"),
                          {{}}));
}

TEST(Filter, NotHas) {
    ASSERT_FALSE(evaluate(parse("[\"!has\", \"foo\"]"),
                          {{ std::string("foo"), int64_t(1) }}));
    ASSERT_FALSE(evaluate(parse("[\"!has\", \"foo\"]"),
                          {{ std::string("foo"), int64_t(0) }}));
    ASSERT_FALSE(evaluate(parse("[\"!has\", \"foo\"]"),
                          {{ std::string("foo"), false }}));
    ASSERT_TRUE(evaluate(parse("[\"!has\", \"foo\"]"),
                          {{}}));
}
