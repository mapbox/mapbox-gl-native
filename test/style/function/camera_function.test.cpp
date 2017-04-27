#include <iostream>
#include <mbgl/test/util.hpp>

#include <mbgl/renderer/property_evaluator.hpp>
#include <mbgl/renderer/property_evaluation_parameters.hpp>

using namespace mbgl;
using namespace mbgl::style;

float evaluate(PropertyValue<float> value, float zoom) {
    return value.evaluate(PropertyEvaluator<float>(PropertyEvaluationParameters(zoom), 0));
}
std::string evaluate(PropertyValue<std::string> value, float zoom) {
    return value.evaluate(PropertyEvaluator<std::string>(PropertyEvaluationParameters(zoom), ""));
}
bool evaluate(PropertyValue<bool> value, float zoom) {
    return value.evaluate(PropertyEvaluator<bool>(PropertyEvaluationParameters(zoom), false));
}

TEST(CameraFunction, Constant) {
    EXPECT_EQ(2.0f, evaluate(PropertyValue<float>(2.0), 0));
    EXPECT_EQ(3.8f, evaluate(PropertyValue<float>(3.8), 0));
    EXPECT_EQ(22.0f, evaluate(PropertyValue<float>(22.0), 0));
    EXPECT_EQ(2.0f, evaluate(PropertyValue<float>(2.0), 4));
    EXPECT_EQ(3.8f, evaluate(PropertyValue<float>(3.8), 4));
    EXPECT_EQ(22.0f, evaluate(PropertyValue<float>(22.0), 4));
    EXPECT_EQ(2.0f, evaluate(PropertyValue<float>(2.0), 22));
    EXPECT_EQ(3.8f, evaluate(PropertyValue<float>(3.8), 22));
    EXPECT_EQ(22.0f, evaluate(PropertyValue<float>(22.0), 22));
}

TEST(CameraFunction, Stops) {
    // Explicit constant slope in fringe regions.
    CameraFunction<float> slope_1(ExponentialStops<float> { { { 0, 1.5 }, { 6, 1.5 }, { 8, 3 }, { 22, 3 } }, 1.75});
    EXPECT_EQ(1.5, evaluate(slope_1, 0));
    EXPECT_EQ(1.5, evaluate(slope_1, 4));
    EXPECT_EQ(1.5, evaluate(slope_1, 6));
    ASSERT_FLOAT_EQ(2.0454545454545454, evaluate(slope_1, 7));
    EXPECT_EQ(3.0, evaluate(slope_1, 8));
    EXPECT_EQ(3.0, evaluate(slope_1, 9));
    EXPECT_EQ(3.0, evaluate(slope_1, 15));
    EXPECT_EQ(3.0, evaluate(slope_1, 22));


    // Test constant values in fringe regions.
    CameraFunction<float> slope_2(ExponentialStops<float> { { { 6, 1.5 }, { 8, 3 } }, 1.75 });
    EXPECT_EQ(1.5, evaluate(slope_2, 0));
    EXPECT_EQ(1.5, evaluate(slope_2, 4));
    EXPECT_EQ(1.5, evaluate(slope_2, 6));
    ASSERT_FLOAT_EQ(2.0454545454545454, evaluate(slope_2, 7));
    EXPECT_EQ(3.0, evaluate(slope_2, 8));
    EXPECT_EQ(3.0, evaluate(slope_2, 9));
    EXPECT_EQ(3.0, evaluate(slope_2, 15));
    EXPECT_EQ(3.0, evaluate(slope_2, 22));


    // Explicit constant slope in fringe regions.
    CameraFunction<float> slope_4(ExponentialStops<float> { { { 0, 2 }, { 8, 10 } }, 1 });
    EXPECT_EQ(2, evaluate(slope_4, 0));
    EXPECT_EQ(3, evaluate(slope_4, 1));
    EXPECT_EQ(4, evaluate(slope_4, 2));
    EXPECT_EQ(4.75, evaluate(slope_4, 2.75));
    EXPECT_EQ(10, evaluate(slope_4, 8));

    // discrete values
    CameraFunction<std::string> discrete_0(IntervalStops<std::string> { {{3, "string0"}, {6, "string1"}, {9, "string2"}} });
    EXPECT_EQ("string0", evaluate(discrete_0, 2));
    EXPECT_EQ("string0", evaluate(discrete_0, 4));
    EXPECT_EQ("string1", evaluate(discrete_0, 7));
    EXPECT_EQ("string2", evaluate(discrete_0, 9));
    EXPECT_EQ("string2", evaluate(discrete_0, 10));

    CameraFunction<bool> discreteBool(IntervalStops<bool> { {{1, false}, {3, true}} });
    EXPECT_FALSE(evaluate(discreteBool, 0));
    EXPECT_FALSE(evaluate(discreteBool, 1));
    EXPECT_FALSE(evaluate(discreteBool, 2));
    EXPECT_TRUE(evaluate(discreteBool, 3));
    EXPECT_TRUE(evaluate(discreteBool, 4));
}
