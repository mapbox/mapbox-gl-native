#include <mbgl/test/util.hpp>
#include <mbgl/test/fixture_log_observer.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/stub_style_observer.hpp>

#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/sprite/sprite_parser.hpp>
#include <mbgl/style/image_impl.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/string.hpp>

#include <utility>

using namespace mbgl;

TEST(SpriteAtlas, Basic) {
    FixtureLog log;
    SpriteAtlas atlas({ 63, 112 }, 1);

    auto images = parseSprite(util::read_file("test/fixtures/annotations/emerald.png"),
                              util::read_file("test/fixtures/annotations/emerald.json"));
    for (auto& image : images) {
        atlas.addImage(image->impl);
    }

    EXPECT_EQ(1.0f, atlas.getPixelRatio());
    EXPECT_EQ(63u, atlas.getSize().width);
    EXPECT_EQ(112u, atlas.getSize().height);

    auto metro = *atlas.getIcon("metro");
    float imagePixelRatio = metro.relativePixelRatio * atlas.getPixelRatio();
    EXPECT_EQ(0, metro.pos.x);
    EXPECT_EQ(0, metro.pos.y);
    EXPECT_EQ(20, metro.pos.w);
    EXPECT_EQ(20, metro.pos.h);
    EXPECT_EQ(18, metro.width);
    EXPECT_EQ(18, metro.height);
    EXPECT_EQ(18u, metro.width * imagePixelRatio);
    EXPECT_EQ(18u, metro.height * imagePixelRatio);
    EXPECT_EQ(1.0f, imagePixelRatio);


    EXPECT_EQ(63u, atlas.getAtlasImage().size.width);
    EXPECT_EQ(112u, atlas.getAtlasImage().size.height);

    auto pos = *atlas.getIcon("metro");
    EXPECT_DOUBLE_EQ(18, pos.size[0]);
    EXPECT_DOUBLE_EQ(18, pos.size[1]);
    EXPECT_DOUBLE_EQ(1.0f / 63, pos.tl[0]);
    EXPECT_DOUBLE_EQ(1.0f / 112, pos.tl[1]);
    EXPECT_DOUBLE_EQ(19.0f / 63, pos.br[0]);
    EXPECT_DOUBLE_EQ(19.0f / 112, pos.br[1]);

    auto missing = atlas.getIcon("doesnotexist");
    EXPECT_FALSE(missing);

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Info,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't find sprite named 'doesnotexist'",
                  }));

    // Different wrapping mode produces different image.
    auto metro2 = *atlas.getPattern("metro");
    EXPECT_EQ(20, metro2.pos.x);
    EXPECT_EQ(0, metro2.pos.y);
    EXPECT_EQ(20, metro2.pos.w);
    EXPECT_EQ(20, metro2.pos.h);

    test::checkImage("test/fixtures/sprite_atlas/basic", atlas.getAtlasImage());
}

TEST(SpriteAtlas, Size) {
    SpriteAtlas atlas({ 63, 112 }, 1.4);

    auto images = parseSprite(util::read_file("test/fixtures/annotations/emerald.png"),
                              util::read_file("test/fixtures/annotations/emerald.json"));
    for (auto& image : images) {
        atlas.addImage(image->impl);
    }

    EXPECT_DOUBLE_EQ(1.4f, atlas.getPixelRatio());
    EXPECT_EQ(63u, atlas.getSize().width);
    EXPECT_EQ(112u, atlas.getSize().height);

    auto metro = *atlas.getIcon("metro");
    float imagePixelRatio = metro.relativePixelRatio * atlas.getPixelRatio();
    EXPECT_EQ(0, metro.pos.x);
    EXPECT_EQ(0, metro.pos.y);
    EXPECT_EQ(15, metro.pos.w);
    EXPECT_EQ(15, metro.pos.h);
    EXPECT_EQ(18, metro.width);
    EXPECT_EQ(18, metro.height);
    EXPECT_EQ(18u, metro.width * imagePixelRatio);
    EXPECT_EQ(18u, metro.height * imagePixelRatio);
    EXPECT_EQ(1.0f, imagePixelRatio);

    // Now the image was created lazily.
    EXPECT_EQ(89u, atlas.getAtlasImage().size.width);
    EXPECT_EQ(157u, atlas.getAtlasImage().size.height);

    test::checkImage("test/fixtures/sprite_atlas/size", atlas.getAtlasImage());
}

TEST(SpriteAtlas, Updates) {
    SpriteAtlas atlas({ 32, 32 }, 1);

    EXPECT_EQ(1.0f, atlas.getPixelRatio());
    EXPECT_EQ(32u, atlas.getSize().width);
    EXPECT_EQ(32u, atlas.getSize().height);

    atlas.addImage(makeMutable<style::Image::Impl>("one", PremultipliedImage({ 16, 12 }), 1));
    auto one = *atlas.getIcon("one");
    float imagePixelRatio = one.relativePixelRatio * atlas.getPixelRatio();
    EXPECT_EQ(0, one.pos.x);
    EXPECT_EQ(0, one.pos.y);
    EXPECT_EQ(18, one.pos.w);
    EXPECT_EQ(14, one.pos.h);
    EXPECT_EQ(16, one.width);
    EXPECT_EQ(12, one.height);
    EXPECT_EQ(16u, one.width * imagePixelRatio);
    EXPECT_EQ(12u, one.height * imagePixelRatio);
    EXPECT_EQ(1.0f, imagePixelRatio);

    // Now the image was created lazily.
    EXPECT_EQ(32u, atlas.getAtlasImage().size.width);
    EXPECT_EQ(32u, atlas.getAtlasImage().size.height);

    test::checkImage("test/fixtures/sprite_atlas/updates_before", atlas.getAtlasImage());

    // Update image
    PremultipliedImage image2({ 16, 12 });
    for (size_t i = 0; i < image2.bytes(); i++) {
        image2.data.get()[i] = 255;
    }
    atlas.addImage(makeMutable<style::Image::Impl>("one", std::move(image2), 1));

    test::checkImage("test/fixtures/sprite_atlas/updates_after", atlas.getAtlasImage());
}

TEST(SpriteAtlas, AddRemove) {
    FixtureLog log;
    SpriteAtlas atlas({ 32, 32 }, 1);

    atlas.addImage(makeMutable<style::Image::Impl>("one", PremultipliedImage({ 16, 16 }), 2));
    atlas.addImage(makeMutable<style::Image::Impl>("two", PremultipliedImage({ 16, 16 }), 2));
    atlas.addImage(makeMutable<style::Image::Impl>("three", PremultipliedImage({ 16, 16 }), 2));

    atlas.removeImage("one");
    atlas.removeImage("two");

    EXPECT_NE(nullptr, atlas.getImage("three"));
    EXPECT_EQ(nullptr, atlas.getImage("two"));
    EXPECT_EQ(nullptr, atlas.getImage("four"));

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Info,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't find sprite named 'two'",
                  }));
    EXPECT_EQ(1u, log.count({
                      EventSeverity::Info,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't find sprite named 'four'",
                  }));
}

TEST(SpriteAtlas, RemoveReleasesBinPackRect) {
    FixtureLog log;

    SpriteAtlas atlas({ 36, 36 }, 1);

    atlas.addImage(makeMutable<style::Image::Impl>("big", PremultipliedImage({ 32, 32 }), 1));
    EXPECT_TRUE(atlas.getIcon("big"));

    atlas.removeImage("big");

    atlas.addImage(makeMutable<style::Image::Impl>("big", PremultipliedImage({ 32, 32 }), 1));
    EXPECT_TRUE(atlas.getIcon("big"));
    EXPECT_TRUE(log.empty());
}
