#include <mbgl/test/util.hpp>

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/rapidjson_conversion.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/function.hpp>
#include <mbgl/util/rapidjson.hpp>

using namespace mbgl;
using namespace mbgl::style;
using namespace mbgl::style::conversion;

auto parseFunction(const std::string& src) {
    JSDocument doc;
    doc.Parse<0>(src);
    return convert<Function<float>>(doc);
}

TEST(StyleConversion, Function) {
    auto fn1 = parseFunction("{\"stops\":[]}");
    ASSERT_FALSE(fn1);
    ASSERT_EQ("function must have at least one stop", fn1.error().message);

    auto fn2 = parseFunction("{\"stops\":[1]}");
    ASSERT_FALSE(fn2);
    ASSERT_EQ("function stop must be an array", fn2.error().message);

    auto fn3 = parseFunction("{\"stops\":[[]]}");
    ASSERT_FALSE(fn3);
    ASSERT_EQ("function stop must have two elements", fn3.error().message);

    auto fn4 = parseFunction("{\"stops\":[[-1,-1]]}");
    ASSERT_TRUE(bool(fn4));

    auto fn5 = parseFunction("{\"stops\":[[0,1,2]]}");
    ASSERT_FALSE(fn5);
    ASSERT_EQ("function stop must have two elements", fn5.error().message);

    auto fn6 = parseFunction("{\"stops\":[[0,\"x\"]]}");
    ASSERT_FALSE(fn6);
    ASSERT_EQ("value must be a number", fn6.error().message);

    auto fn7 = parseFunction("{}");
    ASSERT_FALSE(fn7);
    ASSERT_EQ("function value must specify stops", fn7.error().message);

    auto fn8 = parseFunction("[]");
    ASSERT_FALSE(fn8);
    ASSERT_EQ("function must be an object", fn8.error().message);

    auto fn9 = parseFunction("{\"stops\":[[0,0]],\"base\":false}");
    ASSERT_FALSE(fn9);
    ASSERT_EQ("function base must be a number", fn9.error().message);
}
