#include <iostream>
#include <mbgl/test/util.hpp>

#include <mbgl/style/property_evaluator.hpp>
#include <mbgl/style/property_evaluation_parameters.hpp>

using namespace mbgl;
using namespace mbgl::style;

float evaluate(PropertyValue<float> value, float zoom) {
    return value.evaluate(PropertyEvaluator<float>(PropertyEvaluationParameters(zoom), 0));
}
std::string evaluate(PropertyValue<std::string> value, float zoom) {
    return value.evaluate(PropertyEvaluator<std::string>(PropertyEvaluationParameters(zoom), ""));
}

TEST(Function, Constant) {
    EXPECT_EQ(2.0f, evaluate(2, 0));
    EXPECT_EQ(3.8f, evaluate(3.8, 0));
    EXPECT_EQ(22.0f, evaluate(22, 0));
    EXPECT_EQ(2.0f, evaluate(2, 4));
    EXPECT_EQ(3.8f, evaluate(3.8, 4));
    EXPECT_EQ(22.0f, evaluate(22, 4));
    EXPECT_EQ(2.0f, evaluate(2, 22));
    EXPECT_EQ(3.8f, evaluate(3.8, 22));
    EXPECT_EQ(22.0f, evaluate(22, 22));
}

TEST(Function, Stops) {
    // Explicit constant slope in fringe regions.
    Function<float> slope_1({ { 0, 1.5 }, { 6, 1.5 }, { 8, 3 }, { 22, 3 } }, 1.75);
    EXPECT_EQ(1.5, evaluate(slope_1, 0));
    EXPECT_EQ(1.5, evaluate(slope_1, 4));
    EXPECT_EQ(1.5, evaluate(slope_1, 6));
    ASSERT_FLOAT_EQ(2.0454545454545454, evaluate(slope_1, 7));
    EXPECT_EQ(3.0, evaluate(slope_1, 8));
    EXPECT_EQ(3.0, evaluate(slope_1, 9));
    EXPECT_EQ(3.0, evaluate(slope_1, 15));
    EXPECT_EQ(3.0, evaluate(slope_1, 22));


    // Test constant values in fringe regions.
    Function<float> slope_2({ { 6, 1.5 }, { 8, 3 } }, 1.75);
    EXPECT_EQ(1.5, evaluate(slope_2, 0));
    EXPECT_EQ(1.5, evaluate(slope_2, 4));
    EXPECT_EQ(1.5, evaluate(slope_2, 6));
    ASSERT_FLOAT_EQ(2.0454545454545454, evaluate(slope_2, 7));
    EXPECT_EQ(3.0, evaluate(slope_2, 8));
    EXPECT_EQ(3.0, evaluate(slope_2, 9));
    EXPECT_EQ(3.0, evaluate(slope_2, 15));
    EXPECT_EQ(3.0, evaluate(slope_2, 22));

    // Test no values.
    Function<float> slope_3({}, 1.75);
    EXPECT_EQ(1, evaluate(slope_3, 2));
    EXPECT_EQ(1, evaluate(slope_3, 6));
    EXPECT_EQ(1, evaluate(slope_3, 12));


    // Explicit constant slope in fringe regions.
    Function<float> slope_4({ { 0, 2 }, { 8, 10 } }, 1);
    EXPECT_EQ(2, evaluate(slope_4, 0));
    EXPECT_EQ(3, evaluate(slope_4, 1));
    EXPECT_EQ(4, evaluate(slope_4, 2));
    EXPECT_EQ(4.75, evaluate(slope_4, 2.75));
    EXPECT_EQ(10, evaluate(slope_4, 8));

    // discrete values
    Function<std::string> discrete_0({{ 3, "string0"}, {6, "string1"}, {9, "string2"}}, 1);
    EXPECT_EQ("string0", evaluate(discrete_0, 2));
    EXPECT_EQ("string0", evaluate(discrete_0, 4));
    EXPECT_EQ("string1", evaluate(discrete_0, 7));
    EXPECT_EQ("string2", evaluate(discrete_0, 9));
    EXPECT_EQ("string2", evaluate(discrete_0, 10));
}
