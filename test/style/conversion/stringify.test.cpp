#include <mbgl/test/util.hpp>

#include <mbgl/style/expression/literal.hpp>
#include <mbgl/style/conversion/stringify.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>
#include <mbgl/style/expression/dsl.hpp>
#include <mbgl/util/rapidjson.hpp>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

using namespace mbgl;
using namespace mbgl::style;
using namespace mbgl::style::conversion;

template <class T>
std::string stringify(const T& t) {
    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);
    stringify(writer, t);
    return s.GetString();
}

TEST(Stringify, NullValue) {
    ASSERT_EQ(stringify(NullValue()), "null");
}

TEST(Stringify, Boolean) {
    ASSERT_EQ(stringify(true), "true");
    ASSERT_EQ(stringify(false), "false");
}

TEST(Stringify, Uint64) {
    ASSERT_EQ(stringify(uint64_t(0)), "0");
    ASSERT_EQ(stringify(uint64_t(1)), "1");
}

TEST(Stringify, Int64) {
    ASSERT_EQ(stringify(int64_t(0)), "0");
    ASSERT_EQ(stringify(int64_t(-1)), "-1");
}

TEST(Stringify, Double) {
    ASSERT_EQ(stringify(0.0), "0.0");
    ASSERT_EQ(stringify(10.1234), "10.1234");
}

TEST(Stringify, String) {
    ASSERT_EQ(stringify(std::string("test")), "\"test\"");
}

TEST(Stringify, Enum) {
    ASSERT_EQ(stringify(VisibilityType::Visible), "\"visible\"");
}

TEST(Stringify, Color) {
    ASSERT_EQ(stringify(Color::blue()), "\"rgba(0,0,255,1)\"");
}

TEST(Stringify, Array) {
    ASSERT_EQ(stringify(std::array<float, 2> {{ 1, 2 }}), "[1.0,2.0]");
}

TEST(Stringify, Vector) {
    ASSERT_EQ(stringify(std::vector<float> {{ 1, 2 }}), "[1.0,2.0]");
}

TEST(Stringify, Map) {
    ASSERT_EQ(stringify(std::unordered_map<std::string, float> {{ "a", 1 }}), "{\"a\":1.0}");
}

TEST(Stringify, Value) {
    ASSERT_EQ(stringify(Value(true)), "true");
    ASSERT_EQ(stringify(Value(uint64_t(0))), "0");
    ASSERT_EQ(stringify(Value(1.2)), "1.2");
}

TEST(Stringify, Filter) {
    using namespace mbgl::style::expression::dsl;
    ASSERT_EQ(stringify(Filter()), "null");
    ASSERT_EQ(stringify(Filter(eq(literal("a"), literal("b")))), "[\"==\",\"a\",\"b\"]");
}

TEST(Stringify, PropertyExpression) {
    using namespace mbgl::style::expression::dsl;
    ASSERT_EQ(stringify(PropertyExpression<float>(
        interpolate(
            linear(),
            zoom(),
            0.0, literal(1.0),
            1.0, literal(2.0)
        ))),
        "[\"interpolate\",[\"linear\"],[\"zoom\"],0.0,1.0,1.0,2.0]");

    ASSERT_EQ(stringify(PropertyExpression<float>(
        interpolate(
            exponential(2.0),
            number(get("property")),
            0.0, literal(1.0),
            1.0, literal(2.0)
        ))),
        "[\"interpolate\",[\"exponential\",2.0],[\"number\",[\"get\",\"property\"]],0.0,1.0,1.0,2.0]");

    ASSERT_EQ(stringify(PropertyExpression<float>(
        interpolate(
            linear(),
            zoom(),
            0.0, interpolate(exponential(2.0), number(get("property")), 0.0, literal(1.0), 1.0, literal(2.0)),
            1.0, interpolate(exponential(2.0), number(get("property")), 0.0, literal(1.0), 1.0, literal(2.0))
        ))),
        "[\"interpolate\","
            "[\"linear\"],"
            "[\"zoom\"],"
            "0.0,[\"interpolate\",[\"exponential\",2.0],[\"number\",[\"get\",\"property\"]],0.0,1.0,1.0,2.0],"
            "1.0,[\"interpolate\",[\"exponential\",2.0],[\"number\",[\"get\",\"property\"]],0.0,1.0,1.0,2.0]]");
}

TEST(Stringify, PropertyValue) {
    using namespace mbgl::style::expression::dsl;
    ASSERT_EQ(stringify(PropertyValue<float>(1)), "1.0");
    ASSERT_EQ(stringify(PropertyValue<float>(PropertyExpression<float>(
        interpolate(
            exponential(2.0),
            zoom(),
            0.0, literal(1.0),
            1.0, literal(2.0)
        )))),
        "[\"interpolate\",[\"exponential\",2.0],[\"zoom\"],0.0,1.0,1.0,2.0]");
}

TEST(Stringify, Layout) {
    auto stringify = [] (const SymbolLayoutProperties::Unevaluated& layout) {
        rapidjson::StringBuffer s;
        rapidjson::Writer<rapidjson::StringBuffer> writer(s);
        layout.stringify(writer);
        return std::string(s.GetString());
    };

    ASSERT_EQ(stringify(SymbolLayoutProperties::Unevaluated()), "{}");

    SymbolLayoutProperties::Unevaluated layout;
    layout.get<SymbolAvoidEdges>() = true;
    layout.get<IconPadding>() = 2.0;
    ASSERT_EQ(stringify(layout), "{\"icon-padding\":2.0,\"symbol-avoid-edges\":true}");
}
