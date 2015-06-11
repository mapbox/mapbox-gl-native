#include "../fixtures/util.hpp"

#include <mbgl/annotation/sprite_image.hpp>
#include <mbgl/util/exception.hpp>

using namespace mbgl;

TEST(Annotations, SpriteImageZeroWidth) {
    try {
        SpriteImage(0, 16, 2, "");
        FAIL() << "Expected exception";
    } catch (util::SpriteImageException& ex) {
        EXPECT_STREQ("Sprite image dimensions may not be zero", ex.what());
    }
}

TEST(Annotations, SpriteImageZeroHeight) {
    try {
        SpriteImage(16, 0, 2, "");
        FAIL() << "Expected exception";
    } catch (util::SpriteImageException& ex) {
        EXPECT_STREQ("Sprite image dimensions may not be zero", ex.what());
    }
}

TEST(Annotations, SpriteImageZeroRatio) {
    try {
        SpriteImage(16, 16, 0, "");
        FAIL() << "Expected exception";
    } catch (util::SpriteImageException& ex) {
        EXPECT_STREQ("Sprite image dimensions may not be zero", ex.what());
    }
}

TEST(Annotations, SpriteImageMismatchedData) {
    try {
        SpriteImage(16, 16, 2, "");
        FAIL() << "Expected exception";
    } catch (util::SpriteImageException& ex) {
        EXPECT_STREQ("Sprite image pixel count mismatch", ex.what());
    }
}

TEST(Annotations, SpriteImage) {
    std::string pixels(32 * 24 * 4, '\0');
    SpriteImage sprite(16, 12, 2, std::move(pixels));
    EXPECT_EQ(16, sprite.width);
    EXPECT_EQ(32, sprite.pixelWidth);
    EXPECT_EQ(12, sprite.height);
    EXPECT_EQ(24, sprite.pixelHeight);
    EXPECT_EQ(2, sprite.ratio);
    EXPECT_EQ(32u * 24 * 4, sprite.data.size());
}

TEST(Annotations, SpriteImageFractionalRatio) {
    std::string pixels(20 * 12 * 4, '\0');
    SpriteImage sprite(13, 8, 1.5, std::move(pixels));
    EXPECT_EQ(13, sprite.width);
    EXPECT_EQ(20, sprite.pixelWidth);
    EXPECT_EQ(8, sprite.height);
    EXPECT_EQ(12, sprite.pixelHeight);
    EXPECT_EQ(1.5, sprite.ratio);
    EXPECT_EQ(20u * 12 * 4, sprite.data.size());
}
