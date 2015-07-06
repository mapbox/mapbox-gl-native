#include "../fixtures/util.hpp"
#include "../fixtures/fixture_log_observer.hpp"

#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/annotation/sprite_store.hpp>
#include <mbgl/annotation/sprite_parser.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/image.hpp>

using namespace mbgl;

TEST(Annotations, SpriteAtlas) {
    FixtureLog log;

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


    auto missing = atlas.getImage("doesnotexist", false);
    EXPECT_FALSE(missing.pos.hasArea());
    EXPECT_EQ(0, missing.pos.x);
    EXPECT_EQ(0, missing.pos.y);
    EXPECT_EQ(0, missing.pos.w);
    EXPECT_EQ(0, missing.pos.h);
    EXPECT_EQ(0, missing.pos.originalW);
    EXPECT_EQ(0, missing.pos.originalH);
    EXPECT_FALSE(missing.texture);

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Info,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't find sprite named 'doesnotexist'",
                  }));

    // Different wrapping mode produces different image.
    auto metro2 = atlas.getImage("metro", true);
    EXPECT_EQ(20, metro2.pos.x);
    EXPECT_EQ(0, metro2.pos.y);
    EXPECT_EQ(20, metro2.pos.w);
    EXPECT_EQ(20, metro2.pos.h);
    EXPECT_EQ(18, metro2.pos.originalW);
    EXPECT_EQ(18, metro2.pos.originalH);

    const size_t bytes = atlas.getTextureWidth() * atlas.getTextureHeight() * 4;
    const auto hash = test::crc64(reinterpret_cast<const char*>(atlas.getData()), bytes);
    EXPECT_EQ(0x9875FC0595489A9Fu, hash) << std::hex << hash;

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
    const auto hash = test::crc64(reinterpret_cast<const char*>(atlas.getData()), bytes);
    EXPECT_EQ(0x2CDDA7DBB04D116Du, hash) << std::hex << hash;

    // util::write_file(
    //     "test/fixtures/annotations/atlas2.png",
    //     util::compress_png(atlas.getTextureWidth(), atlas.getTextureHeight(), atlas.getData()));
}

TEST(Annotations, SpriteAtlasUpdates) {
    SpriteStore store;

    SpriteAtlas atlas(32, 32, 1, store);

    EXPECT_EQ(1.0f, atlas.getPixelRatio());
    EXPECT_EQ(32, atlas.getWidth());
    EXPECT_EQ(32, atlas.getHeight());
    EXPECT_EQ(32, atlas.getTextureWidth());
    EXPECT_EQ(32, atlas.getTextureHeight());

    store.setSprite("one", std::make_shared<SpriteImage>(16, 12, 1, std::string(16 * 12 * 4, '\x00')));
    auto one = atlas.getImage("one", false);
    EXPECT_EQ(0, one.pos.x);
    EXPECT_EQ(0, one.pos.y);
    EXPECT_EQ(20, one.pos.w);
    EXPECT_EQ(16, one.pos.h);
    EXPECT_EQ(16, one.pos.originalW);
    EXPECT_EQ(12, one.pos.originalH);
    EXPECT_EQ(16, one.texture->width);
    EXPECT_EQ(12, one.texture->height);
    EXPECT_EQ(16, one.texture->pixelWidth);
    EXPECT_EQ(12, one.texture->pixelHeight);
    EXPECT_EQ(1.0f, one.texture->pixelRatio);

    const size_t bytes = atlas.getTextureWidth() * atlas.getTextureHeight() * 4;
    const auto hash = test::crc64(reinterpret_cast<const char*>(atlas.getData()), bytes);
    EXPECT_EQ(0x0000000000000000u, hash) << std::hex << hash;

    // Update sprite
    auto newSprite = std::make_shared<SpriteImage>(16, 12, 1, std::string(16 * 12 * 4, '\xFF'));
    store.setSprite("one", newSprite);
    ASSERT_EQ(newSprite, store.getSprite("one"));

    // Atlas texture hasn't changed yet.
    const auto hash2 = test::crc64(reinterpret_cast<const char*>(atlas.getData()), bytes);
    EXPECT_EQ(0x0000000000000000u, hash2) << std::hex << hash2;

    atlas.updateDirty();

    // Now the atlas texture has changed.
    const auto hash3 = test::crc64(reinterpret_cast<const char*>(atlas.getData()), bytes);
    EXPECT_EQ(0x4E6D4900CD2D9149u, hash3) << std::hex << hash3;

    // util::write_file(
    //     "test/fixtures/annotations/atlas3.png",
    //     util::compress_png(atlas.getTextureWidth(), atlas.getTextureHeight(), atlas.getData()));
}
