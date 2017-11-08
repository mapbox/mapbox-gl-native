#include <mbgl/test/util.hpp>
#include <mbgl/style/expression/util.hpp>

using namespace mbgl;
using namespace mbgl::style::expression;

TEST(Expression, Util_rgba) {
    Result<Color> valid = rgba(0, 0, 0, 0);
    ASSERT_TRUE(valid);
    ASSERT_EQ(valid->r, 0);
    ASSERT_EQ(valid->g, 0);
    ASSERT_EQ(valid->b, 0);
    ASSERT_EQ(valid->a, 0);

    ASSERT_FALSE(rgba(0, 0, 0, -0.1));
    ASSERT_FALSE(rgba(0, 0, 0, 1.1));
    ASSERT_FALSE(rgba(0, 0, -1, 1));
    ASSERT_FALSE(rgba(0, 0, 256, 1));
    ASSERT_FALSE(rgba(0, -1, 0, 1));
    ASSERT_FALSE(rgba(0, 256, 0, 1));
    ASSERT_FALSE(rgba(-1, 1, 0, 1));
    ASSERT_FALSE(rgba(-256, 1, 0, 1));
}
