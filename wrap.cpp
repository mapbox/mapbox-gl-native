#include <mbgl/test/util.hpp>

#include <mbgl/util/constants.hpp>
#include <mbgl/math/wrap.hpp>

using namespace mbgl;

TEST(Math, WrapHigherValue) {
    ASSERT_DOUBLE_EQ(4, util::wrap(16, 0, 12));
}

TEST(Math, WrapLowerValue) {
    ASSERT_DOUBLE_EQ(9, util::wrap(-3, 0, 12));
}

TEST(Math, WrapInRangeValue) {
    ASSERT_DOUBLE_EQ(4, util::wrap(4, 0, 12));
}

TEST(Math, WrapMaxValue) {
    ASSERT_DOUBLE_EQ(0, util::wrap(12, 0, 12));
}

TEST(Math, WrapMinValue) {
    ASSERT_DOUBLE_EQ(0, util::wrap(0, 0, 12));
}