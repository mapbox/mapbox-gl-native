#include <mbgl/test/util.hpp>

#include <mbgl/style/image.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/exception.hpp>

using namespace mbgl;

TEST(StyleImage, ZeroWidth) {
    try {
        style::Image("test", PremultipliedImage({ 0, 16 }), 2.0);
        FAIL() << "Expected exception";
    } catch (util::StyleImageException& ex) {
        EXPECT_STREQ("dimensions may not be zero", ex.what());
    }
}

TEST(StyleImage, ZeroHeight) {
    try {
        style::Image("test", PremultipliedImage({ 16, 0 }), 2.0);
        FAIL() << "Expected exception";
    } catch (util::StyleImageException& ex) {
        EXPECT_STREQ("dimensions may not be zero", ex.what());
    }
}

TEST(StyleImage, ZeroRatio) {
    try {
        style::Image("test", PremultipliedImage({ 16, 16 }), 0.0);
        FAIL() << "Expected exception";
    } catch (util::StyleImageException& ex) {
        EXPECT_STREQ("pixelRatio may not be <= 0", ex.what());
    }
}

TEST(StyleImage, Retina) {
    style::Image image("test", PremultipliedImage({ 32, 24 }), 2.0);
    EXPECT_EQ(32u, image.getImage().size.width);
    EXPECT_EQ(24u, image.getImage().size.height);
    EXPECT_EQ(2, image.getPixelRatio());
}

TEST(StyleImage, FractionalRatio) {
    style::Image image("test", PremultipliedImage({ 20, 12 }), 1.5);
    EXPECT_EQ(20u, image.getImage().size.width);
    EXPECT_EQ(12u, image.getImage().size.height);
    EXPECT_EQ(1.5, image.getPixelRatio());
}

TEST(StyleImage, InvalidStretchX) {
    // out of left bound
    try {
        style::Image("test", PremultipliedImage({16, 16}), 1, {{-1, 3}});
        FAIL() << "Expected exception";
    } catch (util::StyleImageException& ex) {
        EXPECT_STREQ("stretchX is out of bounds or overlapping", ex.what());
    }

    // overlapping
    try {
        style::Image("test", PremultipliedImage({16, 16}), 1, {{0, 3}, {2., 4.}});
        FAIL() << "Expected exception";
    } catch (util::StyleImageException& ex) {
        EXPECT_STREQ("stretchX is out of bounds or overlapping", ex.what());
    }
}

TEST(StyleImage, InvalidStretchY) {
    // out of bottom bound
    try {
        style::Image("test", PremultipliedImage({16, 16}), 1, {}, {{14, 20}});
        FAIL() << "Expected exception";
    } catch (util::StyleImageException& ex) {
        EXPECT_STREQ("stretchX is out of bounds or overlapping", ex.what());
    }

    // must be sorted
    try {
        style::Image("test", PremultipliedImage({16, 16}), 1, {}, {{4, 8}, {2, 3}});
        FAIL() << "Expected exception";
    } catch (util::StyleImageException& ex) {
        EXPECT_STREQ("stretchX is out of bounds or overlapping", ex.what());
    }
}

TEST(StyleImage, InvalidContent) {
    // bottom right out of bounds
    try {
        style::Image("test", PremultipliedImage({16, 16}), 1, {}, {}, style::ImageContent{0, 0, 24, 28});
        FAIL() << "Expected exception";
    } catch (util::StyleImageException& ex) {
        EXPECT_STREQ("content area is invalid", ex.what());
    }

    // bottom right < top left
    try {
        style::Image("test", PremultipliedImage({16, 16}), 1, {}, {}, style::ImageContent{14, 14, 12, 10});
        FAIL() << "Expected exception";
    } catch (util::StyleImageException& ex) {
        EXPECT_STREQ("content area is invalid", ex.what());
    }

    // top left out of bounds
    try {
        style::Image("test", PremultipliedImage({16, 16}), 1, {}, {}, style::ImageContent{-2, -8, 12, 10});
        FAIL() << "Expected exception";
    } catch (util::StyleImageException& ex) {
        EXPECT_STREQ("content area is invalid", ex.what());
    }
}

TEST(StyleImage, StretchContent) {
    style::Image image(
        "test", PremultipliedImage({16, 16}), 1, {{2, 14}}, {{0, 4}, {12, 16}}, style::ImageContent{2, 2, 14, 14});
    EXPECT_EQ(16u, image.getImage().size.width);
    EXPECT_EQ(16u, image.getImage().size.height);
    EXPECT_EQ(1.0, image.getPixelRatio());
    EXPECT_EQ((style::ImageStretches{{2, 14}}), image.getStretchX());
    EXPECT_EQ((style::ImageStretches{{0, 4}, {12, 16}}), image.getStretchY());
    EXPECT_EQ((style::ImageContent{2, 2, 14, 14}), image.getContent());
}
