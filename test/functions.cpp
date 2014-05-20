#include <iostream>
#include "gtest/gtest.h"

#include <llmr/style/properties.hpp>

using namespace llmr;


TEST(Function, Null) {
    EXPECT_EQ(0.0f, llmr::functions::null(0, { }));
    EXPECT_EQ(0.0f, llmr::functions::null(0, { 3 }));
    EXPECT_EQ(0.0f, llmr::functions::null(0, { 3, 4 }));
    EXPECT_EQ(0.0f, llmr::functions::null(4, { }));
    EXPECT_EQ(0.0f, llmr::functions::null(4, { 3 }));
    EXPECT_EQ(0.0f, llmr::functions::null(4, { 3, 4 }));
    EXPECT_EQ(0.0f, llmr::functions::null(22, { }));
    EXPECT_EQ(0.0f, llmr::functions::null(22, { 3 }));
    EXPECT_EQ(0.0f, llmr::functions::null(22, { 3, 4 }));
}


TEST(Function, Constant) {
    EXPECT_EQ(2.0f, llmr::functions::constant(0, { 2 }));
    EXPECT_EQ(3.8f, llmr::functions::constant(0, { 3.8 }));
    EXPECT_EQ(22.0f, llmr::functions::constant(0, { 22 }));
    EXPECT_EQ(2.0f, llmr::functions::constant(4, { 2 }));
    EXPECT_EQ(3.8f, llmr::functions::constant(4, { 3.8 }));
    EXPECT_EQ(22.0f, llmr::functions::constant(4, { 22 }));
    EXPECT_EQ(2.0f, llmr::functions::constant(22, { 2 }));
    EXPECT_EQ(3.8f, llmr::functions::constant(22, { 3.8 }));
    EXPECT_EQ(22.0f, llmr::functions::constant(22, { 22 }));

    EXPECT_EQ(0.0f, llmr::functions::constant(0, { }));
    EXPECT_EQ(0.0f, llmr::functions::constant(4, { }));
    EXPECT_EQ(0.0f, llmr::functions::constant(22, { }));

    EXPECT_EQ(4.0f, llmr::functions::constant(0, { 4.0f, 63.2f }));
    EXPECT_EQ(4.0f, llmr::functions::constant(4, { 4.0f, 63.2f }));
    EXPECT_EQ(4.0f, llmr::functions::constant(22, { 4.0f, 63.2f }));
}


TEST(Function, Min) {
    EXPECT_EQ(0, llmr::functions::min(0, { 2 }));
    EXPECT_EQ(0, llmr::functions::min(1.98, { 2 }));
    EXPECT_EQ(1, llmr::functions::min(2, { 2 }));
    EXPECT_EQ(1, llmr::functions::min(22, { 2 }));

    EXPECT_EQ(0, llmr::functions::min(0, { }));
    EXPECT_EQ(0, llmr::functions::min(1.98, { }));
    EXPECT_EQ(0, llmr::functions::min(2, { }));
    EXPECT_EQ(0, llmr::functions::min(22, { }));

    EXPECT_EQ(0, llmr::functions::min(0, { 2, 8, 13 }));
    EXPECT_EQ(0, llmr::functions::min(1.98, { 2, 8, 13 }));
    EXPECT_EQ(1, llmr::functions::min(2, { 2, 8, 13 }));
    EXPECT_EQ(1, llmr::functions::min(22, { 2, 8, 13 }));
}


TEST(Function, Max) {
    EXPECT_EQ(1, llmr::functions::max(0, { 2 }));
    EXPECT_EQ(1, llmr::functions::max(2, { 2 }));
    EXPECT_EQ(0, llmr::functions::max(2.01, { 2 }));
    EXPECT_EQ(0, llmr::functions::max(22, { 2 }));

    EXPECT_EQ(0, llmr::functions::max(0, { }));
    EXPECT_EQ(0, llmr::functions::max(2, { }));
    EXPECT_EQ(0, llmr::functions::max(2.01, { }));
    EXPECT_EQ(0, llmr::functions::max(22, { }));

    EXPECT_EQ(1, llmr::functions::max(0, { 2, 8, 13 }));
    EXPECT_EQ(1, llmr::functions::max(2, { 2, 8, 13 }));
    EXPECT_EQ(0, llmr::functions::max(2.01, { 2, 8, 13 }));
    EXPECT_EQ(0, llmr::functions::max(22, { 2, 8, 13 }));
}


TEST(Function, Stops) {
    // Explicit constant slope in fringe regions.
    std::vector<float> slope_1 = { 0, 1.5, /**/ 6, 1.5, /**/ 8, 3, /**/ 22, 3 };
    EXPECT_EQ(1.5, llmr::functions::stops(0, slope_1));
    EXPECT_EQ(1.5, llmr::functions::stops(4, slope_1));
    EXPECT_EQ(1.5, llmr::functions::stops(6, slope_1));
    ASSERT_FLOAT_EQ(2.12132, llmr::functions::stops(7, slope_1));
    EXPECT_EQ(3.0, llmr::functions::stops(8, slope_1));
    EXPECT_EQ(3.0, llmr::functions::stops(9, slope_1));
    EXPECT_EQ(3.0, llmr::functions::stops(15, slope_1));
    EXPECT_EQ(3.0, llmr::functions::stops(22, slope_1));


    // Test constant values in fringe regions.
    std::vector<float> slope_2 = { 6, 1.5, /**/ 8, 3 };
    EXPECT_EQ(1.5, llmr::functions::stops(0, slope_2));
    EXPECT_EQ(1.5, llmr::functions::stops(4, slope_2));
    EXPECT_EQ(1.5, llmr::functions::stops(6, slope_2));
    ASSERT_FLOAT_EQ(2.12132, llmr::functions::stops(7, slope_2));
    EXPECT_EQ(3.0, llmr::functions::stops(8, slope_2));
    EXPECT_EQ(3.0, llmr::functions::stops(9, slope_2));
    EXPECT_EQ(3.0, llmr::functions::stops(15, slope_2));
    EXPECT_EQ(3.0, llmr::functions::stops(22, slope_2));


    // Test uneven number of values.
    std::vector<float> slope_3 = { 6, 1.5, 8 };
    EXPECT_EQ(0, llmr::functions::stops(2, slope_3));
    EXPECT_EQ(0, llmr::functions::stops(6, slope_3));
    EXPECT_EQ(0, llmr::functions::stops(12, slope_3));


    // Test no values.
    std::vector<float> slope_4 = { };
    EXPECT_EQ(1, llmr::functions::stops(2, slope_4));
    EXPECT_EQ(1, llmr::functions::stops(6, slope_4));
    EXPECT_EQ(1, llmr::functions::stops(12, slope_4));
}


TEST(Function, Linear) {
    std::vector<float> slope_1 = { /* z_base */ 4, /* val */ 7.5, /* slope */ 2, /* min */ 7.5, /* max */ 20 };
    ASSERT_FLOAT_EQ(7.5, llmr::functions::linear(3, slope_1));
    ASSERT_FLOAT_EQ(7.5, llmr::functions::linear(4, slope_1));
    ASSERT_FLOAT_EQ(8.5, llmr::functions::linear(4.5, slope_1));
    ASSERT_FLOAT_EQ(9.5, llmr::functions::linear(5, slope_1));
    ASSERT_FLOAT_EQ(11.5, llmr::functions::linear(6, slope_1));
    ASSERT_FLOAT_EQ(19.5, llmr::functions::linear(10, slope_1));
    ASSERT_FLOAT_EQ(20, llmr::functions::linear(11, slope_1));
    ASSERT_FLOAT_EQ(20, llmr::functions::linear(20, slope_1));

    // Invalid number of arguments ==> 0
    std::vector<float> slope_2 = {  };
    ASSERT_FLOAT_EQ(0, llmr::functions::linear(3, slope_2));
    ASSERT_FLOAT_EQ(0, llmr::functions::linear(8, slope_2));
    ASSERT_FLOAT_EQ(0, llmr::functions::linear(10, slope_2));
}


TEST(Function, Exponential) {
    std::vector<float> slope_1 = { /* z_base */ 4, /* val */ 7.5, /* slope */ 2, /* min */ 7.5, /* max */ 20 };
    ASSERT_FLOAT_EQ(8.6428576, llmr::functions::exponential(3, slope_1)); // 7.5 + 1.75^(3 - 4) * 2
    ASSERT_FLOAT_EQ(9.5, llmr::functions::exponential(4, slope_1)); // 7.5 + 1.75^(4 - 4) * 2
    ASSERT_FLOAT_EQ(10.145751, llmr::functions::exponential(4.5, slope_1)); // 7.5 + 1.75^(4.5 - 4) * 2
    ASSERT_FLOAT_EQ(11, llmr::functions::exponential(5, slope_1)); // 7.5 + 1.75^(5 - 4) * 2
    ASSERT_FLOAT_EQ(13.625, llmr::functions::exponential(6, slope_1)); // 7.5 + 1.75^(6 - 4) * 2
    ASSERT_FLOAT_EQ(18.21875, llmr::functions::exponential(7, slope_1)); // 7.5 + 1.75^(7 - 4) * 2
    ASSERT_FLOAT_EQ(20, llmr::functions::exponential(8, slope_1)); // 7.5 + 1.75^(8 - 4) * 2  ==> clamped to 20
    ASSERT_FLOAT_EQ(20, llmr::functions::exponential(20, slope_1)); // 7.5 + 1.75^(20 - 4) * 2 ==> clamped to 20

    // Invalid number of arguments ==> 0
    std::vector<float> slope_2 = {  };
    ASSERT_FLOAT_EQ(0, llmr::functions::exponential(3, slope_2));
    ASSERT_FLOAT_EQ(0, llmr::functions::exponential(8, slope_2));
    ASSERT_FLOAT_EQ(0, llmr::functions::exponential(10, slope_2));
}



