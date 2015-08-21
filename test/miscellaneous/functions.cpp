#include <iostream>
#include "../fixtures/util.hpp"

#include <mbgl/style/function_properties.hpp>

using namespace mbgl;

TEST(Function, Constant) {
    EXPECT_EQ(2.0f, mbgl::ConstantFunction<float>(2).evaluate(0));
    EXPECT_EQ(3.8f, mbgl::ConstantFunction<float>(3.8).evaluate(0));
    EXPECT_EQ(22.0f, mbgl::ConstantFunction<float>(22).evaluate(0));
    EXPECT_EQ(2.0f, mbgl::ConstantFunction<float>(2).evaluate(4));
    EXPECT_EQ(3.8f, mbgl::ConstantFunction<float>(3.8).evaluate(4));
    EXPECT_EQ(22.0f, mbgl::ConstantFunction<float>(22).evaluate(4));
    EXPECT_EQ(2.0f, mbgl::ConstantFunction<float>(2).evaluate(22));
    EXPECT_EQ(3.8f, mbgl::ConstantFunction<float>(3.8).evaluate(22));
    EXPECT_EQ(22.0f, mbgl::ConstantFunction<float>(22).evaluate(22));
}

TEST(Function, Stops) {
    // Explicit constant slope in fringe regions.
    mbgl::StopsFunction<float> slope_1({ { 0, 1.5 }, { 6, 1.5 }, { 8, 3 }, { 22, 3 } }, 1.75);
    EXPECT_EQ(1.5, slope_1.evaluate(0));
    EXPECT_EQ(1.5, slope_1.evaluate(4));
    EXPECT_EQ(1.5, slope_1.evaluate(6));
    ASSERT_FLOAT_EQ(2.0454545454545454, slope_1.evaluate(7));
    EXPECT_EQ(3.0, slope_1.evaluate(8));
    EXPECT_EQ(3.0, slope_1.evaluate(9));
    EXPECT_EQ(3.0, slope_1.evaluate(15));
    EXPECT_EQ(3.0, slope_1.evaluate(22));


    // Test constant values in fringe regions.
    mbgl::StopsFunction<float> slope_2({ { 6, 1.5 }, { 8, 3 } }, 1.75);
    EXPECT_EQ(1.5, slope_2.evaluate(0));
    EXPECT_EQ(1.5, slope_2.evaluate(4));
    EXPECT_EQ(1.5, slope_2.evaluate(6));
    ASSERT_FLOAT_EQ(2.0454545454545454, slope_2.evaluate(7));
    EXPECT_EQ(3.0, slope_2.evaluate(8));
    EXPECT_EQ(3.0, slope_2.evaluate(9));
    EXPECT_EQ(3.0, slope_2.evaluate(15));
    EXPECT_EQ(3.0, slope_2.evaluate(22));

    // Test no values.
    mbgl::StopsFunction<float> slope_3({}, 1.75);
    EXPECT_EQ(1, slope_3.evaluate(2));
    EXPECT_EQ(1, slope_3.evaluate(6));
    EXPECT_EQ(1, slope_3.evaluate(12));


    // Explicit constant slope in fringe regions.
    mbgl::StopsFunction<float> slope_4({ { 0, 2 }, { 8, 10 } }, 1);
    EXPECT_EQ(2, slope_4.evaluate(0));
    EXPECT_EQ(3, slope_4.evaluate(1));
    EXPECT_EQ(4, slope_4.evaluate(2));
    EXPECT_EQ(4.75, slope_4.evaluate(2.75));
    EXPECT_EQ(10, slope_4.evaluate(8));
}
