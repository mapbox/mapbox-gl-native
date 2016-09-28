#include <mbgl/test/util.hpp>

#include <mbgl/util/constants.hpp>
#include <mbgl/math/clamp.hpp>

using namespace mbgl;

TEST(Math, ClampFloatingPoint) {
    double lowestValue = std::numeric_limits<double>::lowest();
    double maximumValue = std::numeric_limits<double>::max();
    double quietNAN = std::numeric_limits<double>::quiet_NaN();

    ASSERT_DOUBLE_EQ(0., util::clamp(0., quietNAN, quietNAN));
    ASSERT_DOUBLE_EQ(0., util::clamp(0., lowestValue, maximumValue));
}

TEST(Math, ClampIntegral) {
    int32_t lowestValue = std::numeric_limits<int32_t>::lowest();
    int32_t maximumValue = std::numeric_limits<int32_t>::max();
    int32_t quietNAN = std::numeric_limits<int32_t>::quiet_NaN();

    ASSERT_EQ(0, util::clamp(0, quietNAN, quietNAN));
    ASSERT_EQ(0, util::clamp(0, lowestValue, maximumValue));
}
