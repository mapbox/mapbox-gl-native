#include <mbgl/test/util.hpp>

#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/function.hpp>
#include <mbgl/style/conversion/data_driven_property_value.hpp>
#include <mbgl/util/rapidjson.hpp>

using namespace mbgl;
using namespace mbgl::style;
using namespace mbgl::style::conversion;

TEST(StyleConversion, Function) {
    Error error;

    auto parseFunction = [&](const std::string& json) {
        return convertJSON<CameraFunction<float>>(json, error);
    };

    auto fn1 = parseFunction(R"({"stops":[]})");
    ASSERT_FALSE(fn1);
    ASSERT_EQ("function must have at least one stop", error.message);

    auto fn2 = parseFunction(R"({"stops":[1]})");
    ASSERT_FALSE(fn2);
    ASSERT_EQ("function stop must be an array", error.message);

    auto fn3 = parseFunction(R"({"stops":[[]]})");
    ASSERT_FALSE(fn3);
    ASSERT_EQ("function stop must have two elements", error.message);

    auto fn4 = parseFunction(R"({"stops":[[-1,-1]]})");
    ASSERT_TRUE(bool(fn4));

    auto fn5 = parseFunction(R"({"stops":[[0,1,2]]})");
    ASSERT_FALSE(fn5);
    ASSERT_EQ("function stop must have two elements", error.message);

    auto fn6 = parseFunction(R"({"stops":[[0,"x"]]})");
    ASSERT_FALSE(fn6);
    ASSERT_EQ("value must be a number", error.message);

    auto fn7 = parseFunction("{}");
    ASSERT_FALSE(fn7);
    ASSERT_EQ("function value must specify stops", error.message);

    auto fn8 = parseFunction("[]");
    ASSERT_FALSE(fn8);
    ASSERT_EQ("function must be an object", error.message);

    auto fn9 = parseFunction(R"({"stops":[[0,0]],"base":false})");
    ASSERT_FALSE(fn9);
    ASSERT_EQ("function base must be a number", error.message);
}

TEST(StyleConversion, CompositeFunctionExpression) {
    Error error;

    auto parseFunction = [&](const std::string& src) {
        JSDocument doc;
        doc.Parse<0>(src);
        return convert<DataDrivenPropertyValue<float>>(doc, error);
    };

    auto fn1 = parseFunction(R"(["interpolate", ["linear"], ["zoom"], 0, ["number", ["get", "x"]], 10, 10])");
    ASSERT_TRUE(fn1);
    
    auto fn2 = parseFunction(R"(["coalesce", ["interpolate", ["linear"], ["zoom"], 0, ["number", ["get", "x"]], 10, 10], 0])");
    ASSERT_TRUE(fn2);

    auto fn3 = parseFunction(R"(["let", "a", 0, ["interpolate", ["linear"], ["zoom"], 0, ["number", ["get", "x"]], 10, 10] ])");
    ASSERT_TRUE(fn3);

    auto fn4 = parseFunction(R"(["coalesce", ["let", "a", 0, ["interpolate", ["linear"], ["zoom"], 0, ["number", ["get", "x"]], 10, 10], 0 ])");
    ASSERT_TRUE(fn4);

    auto fn5 = parseFunction(R"(["coalesce", ["interpolate", ["linear"], ["number", ["get", "x"]], 0, ["zoom"], 10, 10], 0])");
    ASSERT_FALSE(fn5);
    ASSERT_EQ(R"("zoom" expression may only be used as input to a top-level "step" or "interpolate" expression.)", error.message);
}
