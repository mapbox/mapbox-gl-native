#include <mbgl/test/util.hpp>

#include <mbgl/util/dtoa.hpp>

#include <cfloat>
#include <cmath>

using namespace mbgl;

TEST(Dtoa, Precision) {
    EXPECT_EQ(M_E, std::stod(util::dtoa(M_E)));
    EXPECT_EQ(M_LOG2E, std::stod(util::dtoa(M_LOG2E)));
    EXPECT_EQ(M_LOG10E, std::stod(util::dtoa(M_LOG10E)));
    EXPECT_EQ(M_LN2, std::stod(util::dtoa(M_LN2)));
    EXPECT_EQ(M_LN10, std::stod(util::dtoa(M_LN10)));
    EXPECT_EQ(M_PI, std::stod(util::dtoa(M_PI)));
    EXPECT_EQ(M_PI_2, std::stod(util::dtoa(M_PI_2)));
    EXPECT_EQ(M_PI_4, std::stod(util::dtoa(M_PI_4)));
    EXPECT_EQ(M_1_PI, std::stod(util::dtoa(M_1_PI)));
    EXPECT_EQ(M_2_PI, std::stod(util::dtoa(M_2_PI)));
    EXPECT_EQ(M_2_SQRTPI, std::stod(util::dtoa(M_2_SQRTPI)));
    EXPECT_EQ(M_SQRT2, std::stod(util::dtoa(M_SQRT2)));
    EXPECT_EQ(M_SQRT1_2, std::stod(util::dtoa(M_SQRT1_2)));
}
