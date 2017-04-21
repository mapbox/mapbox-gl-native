#include <mbgl/test/util.hpp>

#include <mbgl/style/image.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/exception.hpp>

using namespace mbgl;

TEST(StyleImage, ZeroWidth) {
    try {
        style::Image(PremultipliedImage({ 0, 16 }), 2.0);
        FAIL() << "Expected exception";
    } catch (util::SpriteImageException& ex) {
        EXPECT_STREQ("Sprite image dimensions may not be zero", ex.what());
    }
}

TEST(StyleImage, ZeroHeight) {
    try {
        style::Image(PremultipliedImage({ 16, 0 }), 2.0);
        FAIL() << "Expected exception";
    } catch (util::SpriteImageException& ex) {
        EXPECT_STREQ("Sprite image dimensions may not be zero", ex.what());
    }
}

TEST(StyleImage, ZeroRatio) {
    try {
        style::Image(PremultipliedImage({ 16, 16 }), 0.0);
        FAIL() << "Expected exception";
    } catch (util::SpriteImageException& ex) {
        EXPECT_STREQ("Sprite pixelRatio may not be <= 0", ex.what());
    }
}

TEST(StyleImage, Retina) {
    style::Image image(PremultipliedImage({ 32, 24 }), 2.0);
    EXPECT_EQ(16, image.getWidth());
    EXPECT_EQ(32u, image.image.size.width);
    EXPECT_EQ(12, image.getHeight());
    EXPECT_EQ(24u, image.image.size.height);
    EXPECT_EQ(2, image.pixelRatio);
}

TEST(StyleImage, FractionalRatio) {
    style::Image image(PremultipliedImage({ 20, 12 }), 1.5);
    EXPECT_EQ(float(20.0 / 1.5), image.getWidth());
    EXPECT_EQ(20u, image.image.size.width);
    EXPECT_EQ(float(12.0 / 1.5), image.getHeight());
    EXPECT_EQ(12u, image.image.size.height);
    EXPECT_EQ(1.5, image.pixelRatio);
}
