#include "../fixtures/util.hpp"

#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/annotation/sprite_store.hpp>
#include <mbgl/annotation/sprite_parser.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/image.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include <boost/crc.hpp>
#pragma GCC diagnostic pop

namespace {

// from https://gist.github.com/ArtemGr/997887
uint64_t crc64(const char* data, size_t size) {
    boost::crc_optimal<64, 0x04C11DB7, 0, 0, false, false> crc;
    crc.process_bytes(data, size);
    return crc.checksum();
}

} // anonymous namespace

using namespace mbgl;

using namespace mbgl;

TEST(Annotations, SpriteAtlas) {
    SpriteStore store;
    store.setSprites(parseSprite(util::read_file("test/fixtures/annotations/emerald.png"),
                                 util::read_file("test/fixtures/annotations/emerald.json")));

    SpriteAtlas atlas(63, 112, 1, store);

    EXPECT_EQ(1.0f, atlas.getPixelRatio());
    EXPECT_EQ(63, atlas.getWidth());
    EXPECT_EQ(112, atlas.getHeight());
    EXPECT_EQ(63, atlas.getTextureWidth());
    EXPECT_EQ(112, atlas.getTextureHeight());

    // Image hasn't been created yet.
    EXPECT_TRUE(atlas.getData());

    auto metro = atlas.getImage("metro", false);
    EXPECT_EQ(0, metro.pos.x);
    EXPECT_EQ(0, metro.pos.y);
    EXPECT_EQ(20, metro.pos.w);
    EXPECT_EQ(20, metro.pos.h);
    EXPECT_EQ(18, metro.pos.originalW);
    EXPECT_EQ(18, metro.pos.originalH);
    EXPECT_EQ(18, metro.texture->width);
    EXPECT_EQ(18, metro.texture->height);
    EXPECT_EQ(18, metro.texture->pixelWidth);
    EXPECT_EQ(18, metro.texture->pixelHeight);
    EXPECT_EQ(1.0f, metro.texture->pixelRatio);

    auto pos = atlas.getPosition("metro", false);
    EXPECT_DOUBLE_EQ(20, pos.size[0]);
    EXPECT_DOUBLE_EQ(20, pos.size[1]);
    EXPECT_DOUBLE_EQ(1.0f / 63, pos.tl[0]);
    EXPECT_DOUBLE_EQ(1.0f / 112, pos.tl[1]);
    EXPECT_DOUBLE_EQ(21.0f / 63, pos.br[0]);
    EXPECT_DOUBLE_EQ(21.0f / 112, pos.br[1]);

    // Different wrapping mode produces different image.
    auto metro2 = atlas.getImage("metro", true);
    EXPECT_EQ(20, metro2.pos.x);
    EXPECT_EQ(0, metro2.pos.y);
    EXPECT_EQ(20, metro2.pos.w);
    EXPECT_EQ(20, metro2.pos.h);
    EXPECT_EQ(18, metro2.pos.originalW);
    EXPECT_EQ(18, metro2.pos.originalH);

    const size_t bytes = atlas.getTextureWidth() * atlas.getTextureHeight() * 4;
    const auto hash = crc64(reinterpret_cast<const char*>(atlas.getData()), bytes);
    EXPECT_EQ(0x9875fc0595489a9fu, hash) << std::hex << hash;

    // util::write_file(
    //     "test/fixtures/annotations/atlas1.png",
    //     util::compress_png(atlas.getTextureWidth(), atlas.getTextureHeight(), atlas.getData()));
}

TEST(Annotations, SpriteAtlasSize) {
    SpriteStore store;
    store.setSprites(parseSprite(util::read_file("test/fixtures/annotations/emerald.png"),
                                 util::read_file("test/fixtures/annotations/emerald.json")));
    SpriteAtlas atlas(63, 112, 1.4, store);

    EXPECT_DOUBLE_EQ(1.4f, atlas.getPixelRatio());
    EXPECT_EQ(63, atlas.getWidth());
    EXPECT_EQ(112, atlas.getHeight());
    EXPECT_EQ(89, atlas.getTextureWidth());
    EXPECT_EQ(157, atlas.getTextureHeight());

    auto metro = atlas.getImage("metro", false);
    EXPECT_EQ(0, metro.pos.x);
    EXPECT_EQ(0, metro.pos.y);
    EXPECT_EQ(20, metro.pos.w);
    EXPECT_EQ(20, metro.pos.h);
    EXPECT_EQ(18, metro.pos.originalW);
    EXPECT_EQ(18, metro.pos.originalH);
    EXPECT_EQ(18, metro.texture->width);
    EXPECT_EQ(18, metro.texture->height);
    EXPECT_EQ(18, metro.texture->pixelWidth);
    EXPECT_EQ(18, metro.texture->pixelHeight);
    EXPECT_EQ(1.0f, metro.texture->pixelRatio);

    const size_t bytes = atlas.getTextureWidth() * atlas.getTextureHeight() * 4;
    const auto hash = crc64(reinterpret_cast<const char*>(atlas.getData()), bytes);
    EXPECT_EQ(0x2cdda7dbb04d116du, hash) << std::hex << hash;

    // util::write_file(
    //     "test/fixtures/annotations/atlas2.png",
    //     util::compress_png(atlas.getTextureWidth(), atlas.getTextureHeight(), atlas.getData()));
}
