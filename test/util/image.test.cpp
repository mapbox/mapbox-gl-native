#include <mbgl/test/util.hpp>

#include <mbgl/util/premultiply.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>

using namespace mbgl;

TEST(Image, PNGRoundTrip) {
    PremultipliedImage rgba({ 1, 1 });
    rgba.data[0] = 128;
    rgba.data[1] = 0;
    rgba.data[2] = 0;
    rgba.data[3] = 255;

    PremultipliedImage image = decodeImage(encodePNG(rgba));
    EXPECT_EQ(128, image.data[0]);
    EXPECT_EQ(0, image.data[1]);
    EXPECT_EQ(0, image.data[2]);
    EXPECT_EQ(255, image.data[3]);
}

TEST(Image, PNGRoundTripAlpha) {
    PremultipliedImage rgba({ 1, 1 });
    rgba.data[0] = 128;
    rgba.data[1] = 0;
    rgba.data[2] = 0;
    rgba.data[3] = 128;

    PremultipliedImage image = decodeImage(encodePNG(rgba));
    EXPECT_EQ(128, image.data[0]);
    EXPECT_EQ(0, image.data[1]);
    EXPECT_EQ(0, image.data[2]);
    EXPECT_EQ(128, image.data[3]);
}

TEST(Image, PNGReadNoProfile) {
    PremultipliedImage image = decodeImage(util::read_file("test/fixtures/image/no_profile.png"));
    EXPECT_EQ(128, image.data[0]);
    EXPECT_EQ(0, image.data[1]);
    EXPECT_EQ(0, image.data[2]);
    EXPECT_EQ(255, image.data[3]);
}

TEST(Image, PNGReadNoProfileAlpha) {
    PremultipliedImage image = decodeImage(util::read_file("test/fixtures/image/no_profile_alpha.png"));
    EXPECT_EQ(64, image.data[0]);
    EXPECT_EQ(0, image.data[1]);
    EXPECT_EQ(0, image.data[2]);
    EXPECT_EQ(128, image.data[3]);
}

TEST(Image, PNGReadProfile) {
    PremultipliedImage image = decodeImage(util::read_file("test/fixtures/image/profile.png"));
    EXPECT_EQ(128, image.data[0]);
    EXPECT_EQ(0, image.data[1]);
    EXPECT_EQ(0, image.data[2]);
    EXPECT_EQ(255, image.data[3]);
}

TEST(Image, PNGReadProfileAlpha) {
    PremultipliedImage image = decodeImage(util::read_file("test/fixtures/image/profile_alpha.png"));
    EXPECT_EQ(64, image.data[0]);
    EXPECT_EQ(0, image.data[1]);
    EXPECT_EQ(0, image.data[2]);
    EXPECT_EQ(128, image.data[3]);
}

TEST(Image, PNGTile) {
    PremultipliedImage image = decodeImage(util::read_file("test/fixtures/image/tile.png"));
    EXPECT_EQ(256u, image.size.width);
    EXPECT_EQ(256u, image.size.height);
}

TEST(Image, JPEGTile) {
    PremultipliedImage image = decodeImage(util::read_file("test/fixtures/image/tile.jpeg"));
    EXPECT_EQ(256u, image.size.width);
    EXPECT_EQ(256u, image.size.height);
}

#if !defined(__ANDROID__) && !defined(__APPLE__) && !defined(QT_IMAGE_DECODERS)
TEST(Image, WebPTile) {
    PremultipliedImage image = decodeImage(util::read_file("test/fixtures/image/tile.webp"));
    EXPECT_EQ(256u, image.size.width);
    EXPECT_EQ(256u, image.size.height);
}
#endif // !defined(__ANDROID__) && !defined(__APPLE__) && !defined(QT_IMAGE_DECODERS)

TEST(Image, Premultiply) {
    UnassociatedImage rgba({ 1, 1 });
    rgba.data[0] = 255;
    rgba.data[1] = 254;
    rgba.data[2] = 253;
    rgba.data[3] = 128;

    PremultipliedImage image = util::premultiply(std::move(rgba));
    EXPECT_EQ(128, image.data[0]);
    EXPECT_EQ(127, image.data[1]);
    EXPECT_EQ(127, image.data[2]);
    EXPECT_EQ(128, image.data[3]);
}
