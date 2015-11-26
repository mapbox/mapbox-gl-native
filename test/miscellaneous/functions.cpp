#include <iostream>
#include "../fixtures/util.hpp"

#include <mbgl/style/function.hpp>
#include <mbgl/style/style_calculation_parameters.hpp>

using namespace mbgl;

TEST(Function, Constant) {
    EXPECT_EQ(2.0f, mbgl::Function<float>(2).evaluate(StyleCalculationParameters(0)));
    EXPECT_EQ(3.8f, mbgl::Function<float>(3.8).evaluate(StyleCalculationParameters(0)));
    EXPECT_EQ(22.0f, mbgl::Function<float>(22).evaluate(StyleCalculationParameters(0)));
    EXPECT_EQ(2.0f, mbgl::Function<float>(2).evaluate(StyleCalculationParameters(4)));
    EXPECT_EQ(3.8f, mbgl::Function<float>(3.8).evaluate(StyleCalculationParameters(4)));
    EXPECT_EQ(22.0f, mbgl::Function<float>(22).evaluate(StyleCalculationParameters(4)));
    EXPECT_EQ(2.0f, mbgl::Function<float>(2).evaluate(StyleCalculationParameters(22)));
    EXPECT_EQ(3.8f, mbgl::Function<float>(3.8).evaluate(StyleCalculationParameters(22)));
    EXPECT_EQ(22.0f, mbgl::Function<float>(22).evaluate(StyleCalculationParameters(22)));
}

TEST(Function, Stops) {
    // Explicit constant slope in fringe regions.
    mbgl::Function<float> slope_1({ { 0, 1.5 }, { 6, 1.5 }, { 8, 3 }, { 22, 3 } }, 1.75);
    EXPECT_EQ(1.5, slope_1.evaluate(StyleCalculationParameters(0)));
    EXPECT_EQ(1.5, slope_1.evaluate(StyleCalculationParameters(4)));
    EXPECT_EQ(1.5, slope_1.evaluate(StyleCalculationParameters(6)));
    ASSERT_FLOAT_EQ(2.0454545454545454, slope_1.evaluate(StyleCalculationParameters(7)));
    EXPECT_EQ(3.0, slope_1.evaluate(StyleCalculationParameters(8)));
    EXPECT_EQ(3.0, slope_1.evaluate(StyleCalculationParameters(9)));
    EXPECT_EQ(3.0, slope_1.evaluate(StyleCalculationParameters(15)));
    EXPECT_EQ(3.0, slope_1.evaluate(StyleCalculationParameters(22)));


    // Test constant values in fringe regions.
    mbgl::Function<float> slope_2({ { 6, 1.5 }, { 8, 3 } }, 1.75);
    EXPECT_EQ(1.5, slope_2.evaluate(StyleCalculationParameters(0)));
    EXPECT_EQ(1.5, slope_2.evaluate(StyleCalculationParameters(4)));
    EXPECT_EQ(1.5, slope_2.evaluate(StyleCalculationParameters(6)));
    ASSERT_FLOAT_EQ(2.0454545454545454, slope_2.evaluate(StyleCalculationParameters(7)));
    EXPECT_EQ(3.0, slope_2.evaluate(StyleCalculationParameters(8)));
    EXPECT_EQ(3.0, slope_2.evaluate(StyleCalculationParameters(9)));
    EXPECT_EQ(3.0, slope_2.evaluate(StyleCalculationParameters(15)));
    EXPECT_EQ(3.0, slope_2.evaluate(StyleCalculationParameters(22)));

    // Test no values.
    mbgl::Function<float> slope_3({}, 1.75);
    EXPECT_EQ(1, slope_3.evaluate(StyleCalculationParameters(2)));
    EXPECT_EQ(1, slope_3.evaluate(StyleCalculationParameters(6)));
    EXPECT_EQ(1, slope_3.evaluate(StyleCalculationParameters(12)));


    // Explicit constant slope in fringe regions.
    mbgl::Function<float> slope_4({ { 0, 2 }, { 8, 10 } }, 1);
    EXPECT_EQ(2, slope_4.evaluate(StyleCalculationParameters(0)));
    EXPECT_EQ(3, slope_4.evaluate(StyleCalculationParameters(1)));
    EXPECT_EQ(4, slope_4.evaluate(StyleCalculationParameters(2)));
    EXPECT_EQ(4.75, slope_4.evaluate(StyleCalculationParameters(2.75)));
    EXPECT_EQ(10, slope_4.evaluate(StyleCalculationParameters(8)));
}
