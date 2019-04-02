#include <mbgl/test/util.hpp>

#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/property_value.hpp>
#include <mbgl/style/expression/dsl.hpp>

using namespace mbgl;
using namespace mbgl::style;
using namespace mbgl::style::conversion;

TEST(StyleConversion, Function) {
    Error error;

    auto parseFunction = [&](const std::string& json) {
        return convertJSON<PropertyValue<float>>(json, error, false, false);
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
    ASSERT_EQ("value must be a number", error.message);

    auto fn9 = parseFunction(R"({"stops":[[0,0]],"base":false})");
    ASSERT_FALSE(fn9);
    ASSERT_EQ("function base must be a number", error.message);
}

TEST(StyleConversion, CompositeFunctionExpression) {
    Error error;

    auto parseFunction = [&](const std::string& json) {
        return convertJSON<PropertyValue<float>>(json, error, true, false);
    };

    auto fn1 = parseFunction(R"(["interpolate", ["linear"], ["zoom"], 0, ["number", ["get", "x"]], 10, 10])");
    ASSERT_TRUE(fn1);

    auto fn2 = parseFunction(R"(["coalesce", ["interpolate", ["linear"], ["zoom"], 0, ["number", ["get", "x"]], 10, 10], 0])");
    ASSERT_TRUE(fn2);

    auto fn3 = parseFunction(R"(["let", "a", 0, ["interpolate", ["linear"], ["zoom"], 0, ["number", ["get", "x"]], 10, 10] ])");
    ASSERT_TRUE(fn3);

    auto fn4 = parseFunction(R"(["coalesce", ["let", "a", 0, ["interpolate", ["linear"], ["zoom"], 0, ["number", ["get", "x"]], 10, 10]], 0])");
    ASSERT_TRUE(fn4);

    auto fn5 = parseFunction(R"(["coalesce", ["interpolate", ["linear"], ["number", ["get", "x"]], 0, ["zoom"], 10, 10], 0])");
    ASSERT_FALSE(fn5);
    ASSERT_EQ(R"("zoom" expression may only be used as input to a top-level "step" or "interpolate" expression.)", error.message);
}

TEST(StyleConversion, TokenStrings) {
    ASSERT_FALSE(hasTokens(""));
    ASSERT_FALSE(hasTokens("{"));
    ASSERT_FALSE(hasTokens("{token"));
    ASSERT_TRUE(hasTokens("{token}"));
    ASSERT_TRUE(hasTokens("token {token}"));
    ASSERT_TRUE(hasTokens("{token} {token}"));

    using namespace mbgl::style::expression::dsl;
    ASSERT_EQ(*convertTokenStringToExpression("{token}"), *toString(get(literal("token"))));
    ASSERT_EQ(*convertTokenStringToExpression("token {token}"), *concat(vec(literal("token "), get(literal("token")))));
    ASSERT_EQ(*convertTokenStringToExpression("{token} token"), *concat(vec(get(literal("token")), literal(" token"))));
    ASSERT_EQ(*convertTokenStringToExpression("{token} {token}"), *concat(vec(get(literal("token")), literal(" "), get(literal("token")))));
    ASSERT_EQ(*convertTokenStringToExpression("{token} {token"), *concat(vec(get(literal("token")), literal(" "), literal("{token"))));
    ASSERT_EQ(*convertTokenStringToExpression("{token {token}"), *concat(vec(literal("{token "), get(literal("token")))));
}


TEST(StyleConversion, FormattedIdentityFunction) {
    // See https://github.com/mapbox/mapbox-gl-js/issues/7311
    // We never introduced this bug on gl-native, but we _almost_ did
    Error error;

    auto parseFunction = [&](const std::string& json) {
        return convertJSON<PropertyValue<mbgl::style::expression::Formatted>>(json, error, true, false);
    };
    
    using namespace mbgl::style::expression::dsl;

    auto fn1 = parseFunction(R"({ "property": "name", "type": "identity" })");
    ASSERT_TRUE(bool(fn1));
    ASSERT_TRUE(fn1->isExpression());
    ASSERT_EQ(fn1->asExpression().getExpression().serialize(), format(get(literal("name")))->serialize());
}
