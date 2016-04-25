#include <iostream>
#include <mbgl/test/util.hpp>

#include <mbgl/style/property_evaluator.hpp>
#include <mbgl/style/style_calculation_parameters.hpp>

using namespace mbgl;

float evaluate(PropertyValue<float> value, float zoom) {
    return PropertyValue<float>::visit(value, PropertyEvaluator<float>(StyleCalculationParameters(zoom), 0));
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
}
