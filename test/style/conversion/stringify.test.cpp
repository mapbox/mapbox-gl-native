#include <mbgl/test/util.hpp>

#include <mbgl/style/expression/literal.hpp>
#include <mbgl/style/conversion/stringify.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>
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
    using namespace mbgl::style::expression;
    
    ASSERT_EQ(stringify(Filter()), "null");
    
    ParsingContext context;
    ASSERT_EQ(stringify(Filter(createCompoundExpression("filter-==", createLiteral("a"), createLiteral(1.0), context))), "[\"filter-==\",\"a\",1.0]");
}

TEST(Stringify, CameraFunction) {
    ASSERT_EQ(stringify(CameraFunction<float>(ExponentialStops<float> { {{0, 1}}, 1 })),
        "[\"interpolate\",[\"linear\"],[\"zoom\"],0.0,1.0]");
    ASSERT_EQ(stringify(CameraFunction<float>(ExponentialStops<float> { {{0, 1}}, 2 })),
        "[\"interpolate\",[\"exponential\",2.0],[\"zoom\"],0.0,1.0]");
    ASSERT_EQ(stringify(CameraFunction<float>(IntervalStops<float> { {{0, 1}} })),
        "[\"step\",[\"zoom\"],0.0,1.0]");
}

TEST(Stringify, SourceFunction) {
    ASSERT_EQ(stringify(SourceFunction<float>("property", ExponentialStops<float> { {{0, 1}}, 2 })),
        "[\"interpolate\",[\"exponential\",2.0],[\"number\",[\"get\",\"property\"]],0.0,1.0]");
    ASSERT_EQ(stringify(SourceFunction<float>("property", IntervalStops<float> { {{0, 1}} })),
        "[\"step\",[\"number\",[\"get\",\"property\"]],0.0,1.0]");
    ASSERT_EQ(stringify(SourceFunction<float>("property", CategoricalStops<float> { {{CategoricalValue(true), 1}} })),
        "[\"case\",[\"boolean\",[\"get\",\"property\"]],1.0,[\"error\"]]");
    ASSERT_EQ(stringify(SourceFunction<float>("property", IdentityStops<float> {})),
        "[\"number\",[\"get\",\"property\"]]");
    ASSERT_EQ(stringify(SourceFunction<float>("property", IdentityStops<float> {}, 0.0f)),
        "[\"number\",[\"get\",\"property\"]]");
}

TEST(Stringify, CompositeFunction) {
    ASSERT_EQ(stringify(CompositeFunction<float>("property",
        CompositeExponentialStops<float> {
            {
                { 0, {{0, 1}} },
                { 1, {{0, 1}} }
            },
            2
        }, 0.0f)),
        "[\"interpolate\","
            "[\"linear\"],"
            "[\"zoom\"],"
            "0.0,[\"interpolate\",[\"exponential\",2.0],[\"number\",[\"get\",\"property\"]],0.0,1.0],"
            "1.0,[\"interpolate\",[\"exponential\",2.0],[\"number\",[\"get\",\"property\"]],0.0,1.0]]");
}

TEST(Stringify, PropertyValue) {
    ASSERT_EQ(stringify(PropertyValue<float>(1)), "1.0");
    ASSERT_EQ(stringify(PropertyValue<float>(CameraFunction<float>(ExponentialStops<float> { {{0, 1}}, 2 }))),
        "[\"interpolate\",[\"exponential\",2.0],[\"zoom\"],0.0,1.0]");
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
    ASSERT_EQ(stringify(layout), "{\"symbol-avoid-edges\":true,\"icon-padding\":2.0}");
}
