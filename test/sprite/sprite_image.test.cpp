#include <mbgl/test/util.hpp>

#include <mbgl/sprite/sprite_image.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/exception.hpp>

using namespace mbgl;

TEST(Sprite, SpriteImageZeroWidth) {
    PremultipliedImage image({ 0, 16 });
    try {
        SpriteImage(std::move(image), 2.0);
        FAIL() << "Expected exception";
    } catch (util::SpriteImageException& ex) {
        EXPECT_STREQ("Sprite image dimensions may not be zero", ex.what());
    }
}

TEST(Sprite, SpriteImageZeroHeight) {
    PremultipliedImage image({ 16, 0 });
    try {
        SpriteImage(std::move(image), 2.0);
        FAIL() << "Expected exception";
    } catch (util::SpriteImageException& ex) {
        EXPECT_STREQ("Sprite image dimensions may not be zero", ex.what());
    }
}

TEST(Sprite, SpriteImageZeroRatio) {
    PremultipliedImage image({ 16, 16 });
    try {
        SpriteImage(std::move(image), 0.0);
        FAIL() << "Expected exception";
    } catch (util::SpriteImageException& ex) {
        EXPECT_STREQ("Sprite pixelRatio may not be <= 0", ex.what());
    }
}

TEST(Sprite, SpriteImage) {
    PremultipliedImage image({ 32, 24 });
    SpriteImage sprite(std::move(image), 2.0);
    EXPECT_EQ(16, sprite.getWidth());
    EXPECT_EQ(32u, sprite.image.size.width);
    EXPECT_EQ(12, sprite.getHeight());
    EXPECT_EQ(24u, sprite.image.size.height);
    EXPECT_EQ(2, sprite.pixelRatio);
}

TEST(Sprite, SpriteImageFractionalRatio) {
    PremultipliedImage image({ 20, 12 });
    SpriteImage sprite(std::move(image), 1.5);
    EXPECT_EQ(float(20.0 / 1.5), sprite.getWidth());
    EXPECT_EQ(20u, sprite.image.size.width);
    EXPECT_EQ(float(12.0 / 1.5), sprite.getHeight());
    EXPECT_EQ(12u, sprite.image.size.height);
    EXPECT_EQ(1.5, sprite.pixelRatio);
}
