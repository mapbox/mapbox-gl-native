#include <mbgl/test/util.hpp>

#include <mbgl/util/constants.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/color_lab.hpp>

using namespace mbgl;

TEST(Color, CanParse) {
    optional<Color> result = Color::parse("#00ff00");
    ASSERT_DOUBLE_EQ( result->r, 0);
    ASSERT_DOUBLE_EQ( result->g, 1);
    ASSERT_DOUBLE_EQ( result->b, 0);
}

TEST(Color, InvalidColor) {
    optional<Color> result = Color::parse(" not a color value");

    ASSERT_DOUBLE_EQ( result->r, 0);
    ASSERT_DOUBLE_EQ( result->g, 0);
    ASSERT_DOUBLE_EQ( result->b, 0);
}

TEST(Color, ColorLAB) {
    optional<Color> result = Color::parse("#00ff00");
    optional<ColorLAB> labColor = result->to_lab();

    ASSERT_DOUBLE_EQ( labColor->l, 100);
    ASSERT_DOUBLE_EQ( labColor->a, 0);
    ASSERT_DOUBLE_EQ( labColor->b, 0);
}

TEST(Color, ColorHCL) {
    optional<Color> result = Color::parse("#00ff00");
    optional<ColorHCL> hclColor = result->to_hcl();

    ASSERT_DOUBLE_EQ( hclColor->h, 0);
    ASSERT_DOUBLE_EQ( hclColor->c, 0);
    ASSERT_DOUBLE_EQ( hclColor->l, 100);
}
