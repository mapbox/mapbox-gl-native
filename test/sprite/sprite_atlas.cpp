#include <mbgl/test/util.hpp>
#include <mbgl/test/fixture_log_observer.hpp>

#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/sprite/sprite_store.hpp>
#include <mbgl/sprite/sprite_parser.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/image.hpp>

using namespace mbgl;

namespace {

auto readImage(const std::string& name) {
    return decodeImage(util::read_file(name));
}

auto imageFromAtlas(const SpriteAtlas& atlas) {
    const size_t bytes = atlas.getTextureWidth() * atlas.getTextureHeight() * 4;
    auto data = std::make_unique<uint8_t[]>(bytes);
    const auto src = reinterpret_cast<const uint8_t*>(atlas.getData());
    std::copy(src, src + bytes, data.get());
    return PremultipliedImage{ atlas.getTextureWidth(), atlas.getTextureHeight(), std::move(data) };
}

} // namespace

TEST(Sprite, SpriteAtlas) {
    FixtureLog log;

    auto spriteParseResult = parseSprite(util::read_file("test/fixtures/annotations/emerald.png"),
                                         util::read_file("test/fixtures/annotations/emerald.json"));

    SpriteStore store(1);
    store.setSprites(spriteParseResult.get<Sprites>());

    SpriteAtlas atlas(63, 112, 1, store);

    EXPECT_EQ(1.0f, atlas.getPixelRatio());
    EXPECT_EQ(63, atlas.getWidth());
    EXPECT_EQ(112, atlas.getHeight());
    EXPECT_EQ(63, atlas.getTextureWidth());
    EXPECT_EQ(112, atlas.getTextureHeight());

    // Image hasn't been created yet.
    EXPECT_FALSE(atlas.getData());

    auto metro = *atlas.getImage("metro", false);
    EXPECT_EQ(0, metro.pos.x);
    EXPECT_EQ(0, metro.pos.y);
    EXPECT_EQ(20, metro.pos.w);
    EXPECT_EQ(20, metro.pos.h);
    EXPECT_EQ(18, metro.spriteImage->getWidth());
    EXPECT_EQ(18, metro.spriteImage->getHeight());
    EXPECT_EQ(18u, metro.spriteImage->image.width);
    EXPECT_EQ(18u, metro.spriteImage->image.height);
    EXPECT_EQ(1.0f, metro.spriteImage->pixelRatio);

    EXPECT_TRUE(atlas.getData());

    auto pos = *atlas.getPosition("metro", false);
    EXPECT_DOUBLE_EQ(18, pos.size[0]);
    EXPECT_DOUBLE_EQ(18, pos.size[1]);
    EXPECT_DOUBLE_EQ(1.0f / 63, pos.tl[0]);
    EXPECT_DOUBLE_EQ(1.0f / 112, pos.tl[1]);
    EXPECT_DOUBLE_EQ(19.0f / 63, pos.br[0]);
    EXPECT_DOUBLE_EQ(19.0f / 112, pos.br[1]);

    auto missing = atlas.getImage("doesnotexist", false);
    EXPECT_FALSE(missing);

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Info,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't find sprite named 'doesnotexist'",
                  }));

    // Different wrapping mode produces different image.
    auto metro2 = *atlas.getImage("metro", true);
    EXPECT_EQ(20, metro2.pos.x);
    EXPECT_EQ(0, metro2.pos.y);
    EXPECT_EQ(20, metro2.pos.w);
    EXPECT_EQ(20, metro2.pos.h);

    EXPECT_EQ(readImage("test/fixtures/annotations/result-spriteatlas.png"), imageFromAtlas(atlas));
}

TEST(Sprite, SpriteAtlasSize) {
    auto spriteParseResult = parseSprite(util::read_file("test/fixtures/annotations/emerald.png"),
                                         util::read_file("test/fixtures/annotations/emerald.json"));

    SpriteStore store(1);
    store.setSprites(spriteParseResult.get<Sprites>());

    SpriteAtlas atlas(63, 112, 1.4, store);

    EXPECT_DOUBLE_EQ(1.4f, atlas.getPixelRatio());
    EXPECT_EQ(63, atlas.getWidth());
    EXPECT_EQ(112, atlas.getHeight());
    EXPECT_EQ(89, atlas.getTextureWidth());
    EXPECT_EQ(157, atlas.getTextureHeight());

    auto metro = *atlas.getImage("metro", false);
    EXPECT_EQ(0, metro.pos.x);
    EXPECT_EQ(0, metro.pos.y);
    EXPECT_EQ(16, metro.pos.w);
    EXPECT_EQ(16, metro.pos.h);
    EXPECT_EQ(18, metro.spriteImage->getWidth());
    EXPECT_EQ(18, metro.spriteImage->getHeight());
    EXPECT_EQ(18u, metro.spriteImage->image.width);
    EXPECT_EQ(18u, metro.spriteImage->image.height);
    EXPECT_EQ(1.0f, metro.spriteImage->pixelRatio);

    EXPECT_EQ(readImage("test/fixtures/annotations/result-spriteatlassize.png"),
              imageFromAtlas(atlas));
}

TEST(Sprite, SpriteAtlasUpdates) {
    SpriteStore store(1);

    SpriteAtlas atlas(32, 32, 1, store);

    EXPECT_EQ(1.0f, atlas.getPixelRatio());
    EXPECT_EQ(32, atlas.getWidth());
    EXPECT_EQ(32, atlas.getHeight());
    EXPECT_EQ(32, atlas.getTextureWidth());
    EXPECT_EQ(32, atlas.getTextureHeight());

    store.setSprite("one", std::make_shared<SpriteImage>(PremultipliedImage(16, 12), 1));
    auto one = *atlas.getImage("one", false);
    EXPECT_EQ(0, one.pos.x);
    EXPECT_EQ(0, one.pos.y);
    EXPECT_EQ(20, one.pos.w);
    EXPECT_EQ(16, one.pos.h);
    EXPECT_EQ(16, one.spriteImage->getWidth());
    EXPECT_EQ(12, one.spriteImage->getHeight());
    EXPECT_EQ(16u, one.spriteImage->image.width);
    EXPECT_EQ(12u, one.spriteImage->image.height);
    EXPECT_EQ(1.0f, one.spriteImage->pixelRatio);

    EXPECT_EQ(readImage("test/fixtures/annotations/result-spriteatlas-empty.png"),
              imageFromAtlas(atlas));

    // Update sprite
    PremultipliedImage image2(16, 12);
    for (size_t i = 0; i < image2.size(); i++) {
        image2.data.get()[i] = 255;
    }
    auto newSprite = std::make_shared<SpriteImage>(std::move(image2), 1);
    store.setSprite("one", newSprite);
    ASSERT_EQ(newSprite, store.getSprite("one"));

    // Atlas texture hasn't changed yet.
    EXPECT_EQ(readImage("test/fixtures/annotations/result-spriteatlas-empty.png"),
              imageFromAtlas(atlas));

    atlas.updateDirty();

    // Now the atlas texture has changed.
    EXPECT_EQ(readImage("test/fixtures/annotations/result-spriteatlas-updated.png"),
              imageFromAtlas(atlas));
}
