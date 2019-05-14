#include <mbgl/test/util.hpp>

#include <mbgl/util/string.hpp>

#include <cstdint>

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

TEST(ToHex, SIZE_T) {
#if INTPTR_MAX == INT32_MAX
    EXPECT_EQ("a715b247", util::toHex((uint32_t)0xa715b247));
#elif INTPTR_MAX == INT64_MAX
    EXPECT_EQ("a715b247df38cc29", util::toHex((uint64_t)0xa715b247df38cc29));
#endif

}
