#include <iostream>
#include "../fixtures/util.hpp"

#include <mbgl/map/vector_tile.hpp>
#include <mbgl/style/filter_expression.hpp>
#include <mbgl/style/filter_expression_private.hpp>

#include <map>

using namespace mbgl;

typedef std::multimap<std::string, mbgl::Value> Properties;

class Extractor {
public:
    inline Extractor(const Properties& properties_, FeatureType type_)
        : properties(properties_)
        , type(type_)
    {}

    mapbox::util::optional<Value> getValue(const std::string &key) const {
        if (key == "$type")
            return Value(uint64_t(type));
        auto it = properties.find(key);
        if (it == properties.end())
            return mapbox::util::optional<Value>();
        return it->second;
    }

    FeatureType getType() const {
        return type;
    }

private:
    const Properties properties;
    FeatureType type;
};

FilterExpression parse(const char * expression) {
    rapidjson::Document doc;
    doc.Parse<0>(expression);
    return parseFilterExpression(doc);
}

bool evaluate(const FilterExpression& expression, const Properties& properties, FeatureType type = FeatureType::Unknown) {
    return mbgl::evaluate(expression, Extractor(properties, type));
}

TEST(FilterComparison, EqualsString) {
    FilterExpression f = parse("[\"==\", \"foo\", \"bar\"]");
    ASSERT_TRUE(evaluate(f, {{ "foo", std::string("bar") }}));
    ASSERT_FALSE(evaluate(f, {{ "foo", std::string("baz") }}));
};

TEST(FilterComparison, EqualsNumber) {
    FilterExpression f = parse("[\"==\", \"foo\", 0]");
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

TEST(FilterComparison, EqualsType) {
    FilterExpression f = parse("[\"==\", \"$type\", \"LineString\"]");
    ASSERT_FALSE(evaluate(f, {{}}, FeatureType::Point));
    ASSERT_TRUE(evaluate(f, {{}}, FeatureType::LineString));
}

TEST(FilterComparison, InType) {
    FilterExpression f = parse("[\"in\", \"$type\", \"LineString\", \"Polygon\"]");
    ASSERT_FALSE(evaluate(f, {{}}, FeatureType::Point));
    ASSERT_TRUE(evaluate(f, {{}}, FeatureType::LineString));
    ASSERT_TRUE(evaluate(f, {{}}, FeatureType::Polygon));
}

TEST(FilterComparison, Any) {
    ASSERT_FALSE(evaluate(parse("[\"any\"]"), {{}}));
    ASSERT_TRUE(evaluate(parse("[\"any\", [\"==\", \"foo\", 1]]"),
                         {{ std::string("foo"), int64_t(1) }}));
    ASSERT_FALSE(evaluate(parse("[\"any\", [\"==\", \"foo\", 0]]"),
                          {{ std::string("foo"), int64_t(1) }}));
    ASSERT_TRUE(evaluate(parse("[\"any\", [\"==\", \"foo\", 0], [\"==\", \"foo\", 1]]"),
                         {{ std::string("foo"), int64_t(1) }}));
}

TEST(FilterComparison, All) {
    ASSERT_TRUE(evaluate(parse("[\"all\"]"), {{}}));
    ASSERT_TRUE(evaluate(parse("[\"all\", [\"==\", \"foo\", 1]]"),
                         {{ std::string("foo"), int64_t(1) }}));
    ASSERT_FALSE(evaluate(parse("[\"all\", [\"==\", \"foo\", 0]]"),
                          {{ std::string("foo"), int64_t(1) }}));
    ASSERT_FALSE(evaluate(parse("[\"all\", [\"==\", \"foo\", 0], [\"==\", \"foo\", 1]]"),
                          {{ std::string("foo"), int64_t(1) }}));
}

TEST(FilterComparison, None) {
    ASSERT_TRUE(evaluate(parse("[\"none\"]"), {{}}));
    ASSERT_FALSE(evaluate(parse("[\"none\", [\"==\", \"foo\", 1]]"),
                          {{ std::string("foo"), int64_t(1) }}));
    ASSERT_TRUE(evaluate(parse("[\"none\", [\"==\", \"foo\", 0]]"),
                         {{ std::string("foo"), int64_t(1) }}));
    ASSERT_FALSE(evaluate(parse("[\"none\", [\"==\", \"foo\", 0], [\"==\", \"foo\", 1]]"),
                          {{ std::string("foo"), int64_t(1) }}));
}
