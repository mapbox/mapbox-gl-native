#include <mbgl/test/util.hpp>

#include <mbgl/util/dtoa.hpp>

#include <cfloat>
#include <cmath>

using namespace mbgl;

TEST(Dtoa, Precision) {
    EXPECT_EQ(M_E, util::stod(util::dtoa(M_E)));
    EXPECT_EQ(M_LOG2E, util::stod(util::dtoa(M_LOG2E)));
    EXPECT_EQ(M_LOG10E, util::stod(util::dtoa(M_LOG10E)));
    EXPECT_EQ(M_LN2, util::stod(util::dtoa(M_LN2)));
    EXPECT_EQ(M_LN10, util::stod(util::dtoa(M_LN10)));
    EXPECT_EQ(M_PI, util::stod(util::dtoa(M_PI)));
    EXPECT_EQ(M_PI_2, util::stod(util::dtoa(M_PI_2)));
    EXPECT_EQ(M_PI_4, util::stod(util::dtoa(M_PI_4)));
    EXPECT_EQ(M_1_PI, util::stod(util::dtoa(M_1_PI)));
    EXPECT_EQ(M_2_PI, util::stod(util::dtoa(M_2_PI)));
    EXPECT_EQ(M_2_SQRTPI, util::stod(util::dtoa(M_2_SQRTPI)));
    EXPECT_EQ(M_SQRT2, util::stod(util::dtoa(M_SQRT2)));
    EXPECT_EQ(M_SQRT1_2, util::stod(util::dtoa(M_SQRT1_2)));
}
