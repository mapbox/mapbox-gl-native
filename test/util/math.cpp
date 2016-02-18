#include <mbgl/test/util.hpp>

#include <mbgl/util/constants.hpp>
#include <mbgl/util/math.hpp>

using namespace mbgl;

TEST(Math, MinMaxFloatingPoint) {
    double minimumValue = std::numeric_limits<double>::min();
    double lowestValue = std::numeric_limits<double>::lowest();
    double maximumValue = std::numeric_limits<double>::max();
    double quietNAN = std::numeric_limits<double>::quiet_NaN();

    ASSERT_DOUBLE_EQ(lowestValue, util::min(minimumValue, lowestValue));
    ASSERT_DOUBLE_EQ(lowestValue, util::min(double(NAN), lowestValue));
    ASSERT_DOUBLE_EQ(lowestValue, util::min(quietNAN, lowestValue));
    ASSERT_DOUBLE_EQ(0., util::min(2., 1., 0.));

    ASSERT_DOUBLE_EQ(maximumValue, util::max(double(NAN), maximumValue));
    ASSERT_DOUBLE_EQ(maximumValue, util::max(quietNAN, maximumValue));
    ASSERT_DOUBLE_EQ(10., util::max(8., 9., 10.));

    ASSERT_DOUBLE_EQ(0., util::clamp(0., quietNAN, quietNAN));
    ASSERT_DOUBLE_EQ(0., util::clamp(0., lowestValue, maximumValue));
}

TEST(Math, MinMaxIntegral) {
    int32_t minimumValue = std::numeric_limits<int32_t>::min();
    int32_t lowestValue = std::numeric_limits<int32_t>::lowest();
    int32_t maximumValue = std::numeric_limits<int32_t>::max();
    int32_t quietNAN = std::numeric_limits<int32_t>::quiet_NaN();

    ASSERT_EQ(lowestValue, util::min(minimumValue, lowestValue));
    ASSERT_EQ(lowestValue, util::min(quietNAN, lowestValue));
    ASSERT_EQ(0, util::min(2, 1, 0));

    ASSERT_EQ(maximumValue, util::max(quietNAN, maximumValue));
    ASSERT_EQ(10, util::max(8, 9, 10));

    ASSERT_EQ(0, util::clamp(0, quietNAN, quietNAN));
    ASSERT_EQ(0, util::clamp(0, lowestValue, maximumValue));
}
