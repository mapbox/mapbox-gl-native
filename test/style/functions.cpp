#include <iostream>
#include <mbgl/test/util.hpp>

#include <mbgl/style/function.hpp>
#include <mbgl/style/function_evaluator.hpp>
#include <mbgl/style/style_calculation_parameters.hpp>

using namespace mbgl;

TEST(Function, Constant) {
    NormalFunctionEvaluator<float> evaluate;
    EXPECT_EQ(2.0f, evaluate(Function<float>(2), StyleCalculationParameters(0)));
    EXPECT_EQ(3.8f, evaluate(Function<float>(3.8), StyleCalculationParameters(0)));
    EXPECT_EQ(22.0f, evaluate(Function<float>(22), StyleCalculationParameters(0)));
    EXPECT_EQ(2.0f, evaluate(Function<float>(2), StyleCalculationParameters(4)));
    EXPECT_EQ(3.8f, evaluate(Function<float>(3.8), StyleCalculationParameters(4)));
    EXPECT_EQ(22.0f, evaluate(Function<float>(22), StyleCalculationParameters(4)));
    EXPECT_EQ(2.0f, evaluate(Function<float>(2), StyleCalculationParameters(22)));
    EXPECT_EQ(3.8f, evaluate(Function<float>(3.8), StyleCalculationParameters(22)));
    EXPECT_EQ(22.0f, evaluate(Function<float>(22), StyleCalculationParameters(22)));
}

TEST(Function, Stops) {
    NormalFunctionEvaluator<float> evaluate;

    // Explicit constant slope in fringe regions.
    Function<float> slope_1({ { 0, 1.5 }, { 6, 1.5 }, { 8, 3 }, { 22, 3 } }, 1.75);
    EXPECT_EQ(1.5, evaluate(slope_1, StyleCalculationParameters(0)));
    EXPECT_EQ(1.5, evaluate(slope_1, StyleCalculationParameters(4)));
    EXPECT_EQ(1.5, evaluate(slope_1, StyleCalculationParameters(6)));
    ASSERT_FLOAT_EQ(2.0454545454545454, evaluate(slope_1, StyleCalculationParameters(7)));
    EXPECT_EQ(3.0, evaluate(slope_1, StyleCalculationParameters(8)));
    EXPECT_EQ(3.0, evaluate(slope_1, StyleCalculationParameters(9)));
    EXPECT_EQ(3.0, evaluate(slope_1, StyleCalculationParameters(15)));
    EXPECT_EQ(3.0, evaluate(slope_1, StyleCalculationParameters(22)));


    // Test constant values in fringe regions.
    Function<float> slope_2({ { 6, 1.5 }, { 8, 3 } }, 1.75);
    EXPECT_EQ(1.5, evaluate(slope_2, StyleCalculationParameters(0)));
    EXPECT_EQ(1.5, evaluate(slope_2, StyleCalculationParameters(4)));
    EXPECT_EQ(1.5, evaluate(slope_2, StyleCalculationParameters(6)));
    ASSERT_FLOAT_EQ(2.0454545454545454, evaluate(slope_2, StyleCalculationParameters(7)));
    EXPECT_EQ(3.0, evaluate(slope_2, StyleCalculationParameters(8)));
    EXPECT_EQ(3.0, evaluate(slope_2, StyleCalculationParameters(9)));
    EXPECT_EQ(3.0, evaluate(slope_2, StyleCalculationParameters(15)));
    EXPECT_EQ(3.0, evaluate(slope_2, StyleCalculationParameters(22)));

    // Test no values.
    Function<float> slope_3({}, 1.75);
    EXPECT_EQ(1, evaluate(slope_3, StyleCalculationParameters(2)));
    EXPECT_EQ(1, evaluate(slope_3, StyleCalculationParameters(6)));
    EXPECT_EQ(1, evaluate(slope_3, StyleCalculationParameters(12)));


    // Explicit constant slope in fringe regions.
    Function<float> slope_4({ { 0, 2 }, { 8, 10 } }, 1);
    EXPECT_EQ(2, evaluate(slope_4, StyleCalculationParameters(0)));
    EXPECT_EQ(3, evaluate(slope_4, StyleCalculationParameters(1)));
    EXPECT_EQ(4, evaluate(slope_4, StyleCalculationParameters(2)));
    EXPECT_EQ(4.75, evaluate(slope_4, StyleCalculationParameters(2.75)));
    EXPECT_EQ(10, evaluate(slope_4, StyleCalculationParameters(8)));
}
