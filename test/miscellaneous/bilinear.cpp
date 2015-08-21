#include "../fixtures/util.hpp"
#include <mbgl/util/compression.hpp>
#include <mbgl/util/scaling.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>

#include <algorithm>
#include <cstring>

using namespace mbgl;

TEST(Bilinear, Scaling) {
    // We're reading from a custom "image format" that has a uint32_t width/height prefix and then
    // raw RGBA data. We can't use the built-in PNG reading routines because they are reading
    // premultiplied images by default.
    const std::string sprite = util::decompress(util::read_file("test/fixtures/sprites/bright.bin"));
    const uint8_t *src = reinterpret_cast<const uint8_t *>(sprite.data());
    ASSERT_GT(sprite.length(), 8u);
    const uint32_t width = src[0] << 24 | src[1] << 16 | src[2] << 8 | src[3];
    const uint32_t height = src[4] << 24 | src[5] << 16 | src[6] << 8 | src[7];
    ASSERT_EQ(sprite.length(), 2 * sizeof(uint32_t) + width * height * sizeof(uint32_t));

    const uint32_t *srcData = reinterpret_cast<const uint32_t *>(src + 8);
    const vec2<uint32_t> srcSize { width, height };
    const vec2<uint32_t> dstSize { 128, 128 };
    auto dst = std::make_unique<uint32_t[]>(dstSize.x * dstSize.y);
    uint32_t *dstData = dst.get();
    std::fill(dstData, dstData + dstSize.x * dstSize.y, 0xFFFF00FF);

    util::bilinearScale(srcData, srcSize, { 0, 0, 24, 24 }, dstData, dstSize, { 8, 8, 24, 24 }, false);
    util::bilinearScale(srcData, srcSize, { 26, 0, 24, 24 }, dstData, dstSize, { 0, 40, 48, 48 }, false);
    util::bilinearScale(srcData, srcSize, { 26, 26, 24, 24 }, dstData, dstSize, { 52, 40, 36, 36 }, false);
    util::bilinearScale(srcData, srcSize, { 26, 26, 24, 24 }, dstData, dstSize, { 52, 40, 36, 36 }, false);
    util::bilinearScale(srcData, srcSize, { 104, 0, 24, 24 }, dstData, dstSize, { 96, 0, 48, 48 }, false);
    util::bilinearScale(srcData, srcSize, { 52, 260, 24, 24 }, dstData, dstSize, { 108, 108, 38, 38 }, false);
    util::bilinearScale(srcData, srcSize, { 380, 0, 24, 24 }, dstData, dstSize, { 36, 0, 24, 24 }, false);
    util::bilinearScale(srcData, srcSize, { 396, 396, 24, 24 }, dstData, dstSize, { 0, 0, 50, 50 }, false);
    util::bilinearScale(srcData, srcSize, { 380, 182, 12, 12 }, dstData, dstSize, { 52, 80, 24, 24 }, false);

    // From the bottom
    util::bilinearScale(srcData, srcSize, { 252, 380, 12, 12 }, dstData, dstSize, { 0, 90, 12, 12 }, false);
    util::bilinearScale(srcData, srcSize, { 252, 380, 12, 12 }, dstData, dstSize, { 18, 90, 24, 24 }, false);

    const std::string data { reinterpret_cast<char *>(dstData), dstSize.x * dstSize.y * sizeof(uint32_t) };
    util::write_file("test/fixtures/sprites/atlas_actual.png", util::compress_png(dstSize.x, dstSize.y, dstData));
    util::write_file("test/fixtures/sprites/atlas_actual.bin", util::compress(data));

    const std::string reference = util::decompress(util::read_file("test/fixtures/sprites/atlas_reference.bin"));

    EXPECT_EQ(reference.size(), data.size());
    EXPECT_TRUE(0 == std::memcmp(data.data(), reference.data(), data.size()));
}
