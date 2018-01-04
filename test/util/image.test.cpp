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

TEST(Image, Resize) {
    AlphaImage image({0, 0});

    image.resize({1, 1});
    EXPECT_EQ(image.size, Size({1, 1}));

    image.fill(100);
    image.resize({2, 1});
    EXPECT_EQ(image.size, Size({2, 1}));
    EXPECT_EQ(image.data[0], 100);
    EXPECT_EQ(image.data[1], 0);

    image.resize({0, 0});
    EXPECT_EQ(image.size, Size({0, 0}));
}

TEST(Image, Copy) {
    PremultipliedImage src5({5, 5});
    PremultipliedImage dst5({5, 5});
    PremultipliedImage src10({10, 10});
    PremultipliedImage dst10({10, 10});

    EXPECT_THROW(PremultipliedImage::copy(src5, dst10, {0, 0}, {0, 0}, {6, 1}), std::out_of_range);
    EXPECT_THROW(PremultipliedImage::copy(src5, dst10, {0, 0}, {0, 0}, {1, 6}), std::out_of_range);
    EXPECT_THROW(PremultipliedImage::copy(src5, dst10, {1, 1}, {0, 0}, {5, 1}), std::out_of_range);
    EXPECT_THROW(PremultipliedImage::copy(src5, dst10, {1, 1}, {0, 0}, {1, 5}), std::out_of_range);

    EXPECT_THROW(PremultipliedImage::copy(src10, dst5, {0, 0}, {0, 0}, {6, 1}), std::out_of_range);
    EXPECT_THROW(PremultipliedImage::copy(src10, dst5, {0, 0}, {0, 0}, {1, 6}), std::out_of_range);
    EXPECT_THROW(PremultipliedImage::copy(src10, dst5, {0, 0}, {1, 1}, {5, 1}), std::out_of_range);
    EXPECT_THROW(PremultipliedImage::copy(src10, dst5, {0, 0}, {1, 1}, {1, 5}), std::out_of_range);

    const uint32_t max = std::numeric_limits<uint32_t>::max();

    EXPECT_THROW(PremultipliedImage::copy(src10, dst10, {max, 0}, {0, 0}, {1, 1}), std::out_of_range);
    EXPECT_THROW(PremultipliedImage::copy(src10, dst10, {0, max}, {0, 0}, {1, 1}), std::out_of_range);
    EXPECT_THROW(PremultipliedImage::copy(src10, dst10, {0, 0}, {max, 0}, {1, 1}), std::out_of_range);
    EXPECT_THROW(PremultipliedImage::copy(src10, dst10, {0, 0}, {0, max}, {1, 1}), std::out_of_range);

    EXPECT_THROW(PremultipliedImage::copy(src10, dst10, {1, 0}, {0, 0}, {max, 1}), std::out_of_range);
    EXPECT_THROW(PremultipliedImage::copy(src10, dst10, {0, 1}, {0, 0}, {1, max}), std::out_of_range);
    EXPECT_THROW(PremultipliedImage::copy(src10, dst10, {0, 0}, {1, 0}, {max, 1}), std::out_of_range);
    EXPECT_THROW(PremultipliedImage::copy(src10, dst10, {0, 0}, {0, 1}, {1, max}), std::out_of_range);
}

TEST(Image, Move) {
    UnassociatedImage rgba({ 1, 1 });
    rgba.data[0] = 255;
    rgba.data[1] = 254;
    rgba.data[2] = 253;
    rgba.data[3] = 128;

    auto moved = std::move(rgba);

    EXPECT_EQ(0u, rgba.size.width);
    EXPECT_EQ(nullptr, rgba.data.get());
    EXPECT_EQ(254, moved.data[1]);
    EXPECT_EQ(1u, moved.size.width);
}

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
    EXPECT_EQ(1u, image.size.width);
    EXPECT_EQ(1u, image.size.height);
    EXPECT_EQ(0u, rgba.size.width);
    EXPECT_EQ(0u, rgba.size.height);
}
