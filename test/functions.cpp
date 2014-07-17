#include <iostream>
#include "gtest/gtest.h"

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
    mbgl::StopsFunction<float> slope_1({ { 0, 1.5 }, { 6, 1.5 }, { 8, 3 }, { 22, 3 } });
    EXPECT_EQ(1.5, slope_1.evaluate(0));
    EXPECT_EQ(1.5, slope_1.evaluate(4));
    EXPECT_EQ(1.5, slope_1.evaluate(6));
    ASSERT_FLOAT_EQ(2.12132, slope_1.evaluate(7));
    EXPECT_EQ(3.0, slope_1.evaluate(8));
    EXPECT_EQ(3.0, slope_1.evaluate(9));
    EXPECT_EQ(3.0, slope_1.evaluate(15));
    EXPECT_EQ(3.0, slope_1.evaluate(22));


    // Test constant values in fringe regions.
    mbgl::StopsFunction<float> slope_2({ { 6, 1.5 }, { 8, 3 } });
    EXPECT_EQ(1.5, slope_2.evaluate(0));
    EXPECT_EQ(1.5, slope_2.evaluate(4));
    EXPECT_EQ(1.5, slope_2.evaluate(6));
    ASSERT_FLOAT_EQ(2.12132, slope_2.evaluate(7));
    EXPECT_EQ(3.0, slope_2.evaluate(8));
    EXPECT_EQ(3.0, slope_2.evaluate(9));
    EXPECT_EQ(3.0, slope_2.evaluate(15));
    EXPECT_EQ(3.0, slope_2.evaluate(22));

    // Test no values.
    mbgl::StopsFunction<float> slope_3({});
    EXPECT_EQ(1, slope_3.evaluate(2));
    EXPECT_EQ(1, slope_3.evaluate(6));
    EXPECT_EQ(1, slope_3.evaluate(12));
}


TEST(Function, Linear) {
    mbgl::LinearFunction<float> slope_1(/* val */ 7.5, /* z_base */ 4, /* slope */ 2, /* min */ 7.5, /* max */ 20);
    ASSERT_FLOAT_EQ(7.5, slope_1.evaluate(3));
    ASSERT_FLOAT_EQ(7.5, slope_1.evaluate(4));
    ASSERT_FLOAT_EQ(8.5, slope_1.evaluate(4.5));
    ASSERT_FLOAT_EQ(9.5, slope_1.evaluate(5));
    ASSERT_FLOAT_EQ(11.5, slope_1.evaluate(6));
    ASSERT_FLOAT_EQ(19.5, slope_1.evaluate(10));
    ASSERT_FLOAT_EQ(20, slope_1.evaluate(11));
    ASSERT_FLOAT_EQ(20, slope_1.evaluate(20));
}


TEST(Function, Exponential) {
    mbgl::ExponentialFunction<float> slope_1(/* val */ 7.5, /* z_base */ 4, /* exp_base */ 1.75, /* slope */ 2, /* min */ 7.5, /* max */ 20);
    ASSERT_FLOAT_EQ(8.6428576, slope_1.evaluate(3)); // 7.5 + 1.75^(3 - 4) * 2
    ASSERT_FLOAT_EQ(9.5, slope_1.evaluate(4)); // 7.5 + 1.75^(4 - 4) * 2
    ASSERT_FLOAT_EQ(10.145751, slope_1.evaluate(4.5)); // 7.5 + 1.75^(4.5 - 4) * 2
    ASSERT_FLOAT_EQ(11, slope_1.evaluate(5)); // 7.5 + 1.75^(5 - 4) * 2
    ASSERT_FLOAT_EQ(13.625, slope_1.evaluate(6)); // 7.5 + 1.75^(6 - 4) * 2
    ASSERT_FLOAT_EQ(18.21875, slope_1.evaluate(7)); // 7.5 + 1.75^(7 - 4) * 2
    ASSERT_FLOAT_EQ(20, slope_1.evaluate(8)); // 7.5 + 1.75^(8 - 4) * 2  ==> clamped to 20
    ASSERT_FLOAT_EQ(20, slope_1.evaluate(20)); // 7.5 + 1.75^(20 - 4) * 2 ==> clamped to 20
}



