#include <mbgl/test/util.hpp>

#include <mbgl/util/string.hpp>

using namespace mbgl;

TEST(ToString, FloatingPoint) {
    EXPECT_EQ("0", util::toString(0.0));
    EXPECT_EQ("0.0", util::toString(0.0, true));
    EXPECT_EQ("1.33", util::toString(1.33));
    EXPECT_EQ("1.33", util::toString(1.33, true));
    EXPECT_EQ("-1", util::toString(-1.0));
    EXPECT_EQ("-1.0", util::toString(-1.0, true));
    EXPECT_EQ("12340000000", util::toString(12340000000.0));
    EXPECT_EQ("12340000000.0", util::toString(12340000000.0, true));
    EXPECT_EQ("12340000000", util::toString(12340000000.0));
    EXPECT_EQ("12340000000.0", util::toString(12340000000.0, true));
}
