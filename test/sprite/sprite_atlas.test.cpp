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
    SpriteAtlas atlas;

    auto images = parseSprite(util::read_file("test/fixtures/annotations/emerald.png"),
                              util::read_file("test/fixtures/annotations/emerald.json"));
    for (auto& image : images) {
        atlas.addImage(image->baseImpl);
    }

    auto metro = *atlas.getIcon("metro");
    EXPECT_EQ(1, metro.tl()[0]);
    EXPECT_EQ(1, metro.tl()[1]);
    EXPECT_EQ(19, metro.br()[0]);
    EXPECT_EQ(19, metro.br()[1]);
    EXPECT_EQ(18, metro.displaySize()[0]);
    EXPECT_EQ(18, metro.displaySize()[1]);
    EXPECT_EQ(1.0f, metro.pixelRatio);

    EXPECT_EQ(atlas.getPixelSize(), atlas.getAtlasImage().size);

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
    EXPECT_EQ(21, metro2.tl()[0]);
    EXPECT_EQ(1, metro2.tl()[1]);
    EXPECT_EQ(39, metro2.br()[0]);
    EXPECT_EQ(19, metro2.br()[1]);

    test::checkImage("test/fixtures/sprite_atlas/basic", atlas.getAtlasImage());
}

TEST(SpriteAtlas, Size) {
    SpriteAtlas atlas;

    auto images = parseSprite(util::read_file("test/fixtures/annotations/emerald.png"),
                              util::read_file("test/fixtures/annotations/emerald.json"));
    for (auto& image : images) {
        atlas.addImage(image->baseImpl);
    }

    auto metro = *atlas.getIcon("metro");
    EXPECT_EQ(1, metro.tl()[0]);
    EXPECT_EQ(1, metro.tl()[1]);
    EXPECT_EQ(19, metro.br()[0]);
    EXPECT_EQ(19, metro.br()[1]);
    EXPECT_EQ(18, metro.displaySize()[0]);
    EXPECT_EQ(18, metro.displaySize()[1]);
    EXPECT_EQ(1.0f, metro.pixelRatio);

    test::checkImage("test/fixtures/sprite_atlas/size", atlas.getAtlasImage());
}

TEST(SpriteAtlas, Updates) {
    SpriteAtlas atlas;

    atlas.addImage(makeMutable<style::Image::Impl>("one", PremultipliedImage({ 16, 12 }), 1));
    auto one = *atlas.getIcon("one");
    EXPECT_EQ(1, one.tl()[0]);
    EXPECT_EQ(1, one.tl()[1]);
    EXPECT_EQ(17, one.br()[0]);
    EXPECT_EQ(13, one.br()[1]);
    EXPECT_EQ(16, one.displaySize()[0]);
    EXPECT_EQ(12, one.displaySize()[1]);
    EXPECT_EQ(1.0f, one.pixelRatio);

    test::checkImage("test/fixtures/sprite_atlas/updates_before", atlas.getAtlasImage());

    // Update image
    PremultipliedImage image2({ 16, 12 });
    for (size_t i = 0; i < image2.bytes(); i++) {
        image2.data.get()[i] = 255;
    }
    atlas.updateImage(makeMutable<style::Image::Impl>("one", std::move(image2), 1));

    test::checkImage("test/fixtures/sprite_atlas/updates_after", atlas.getAtlasImage());
}

TEST(SpriteAtlas, AddRemove) {
    FixtureLog log;
    SpriteAtlas atlas;

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
    SpriteAtlas atlas;

    atlas.addImage(makeMutable<style::Image::Impl>("big", PremultipliedImage({ 32, 32 }), 1));
    EXPECT_TRUE(atlas.getIcon("big"));

    atlas.removeImage("big");

    atlas.addImage(makeMutable<style::Image::Impl>("big", PremultipliedImage({ 32, 32 }), 1));
    EXPECT_TRUE(atlas.getIcon("big"));
    EXPECT_TRUE(log.empty());
}

class StubIconRequestor : public IconRequestor {
public:
    void onIconsAvailable(IconMap icons) final {
        if (iconsAvailable) iconsAvailable(icons);
    }

    std::function<void (IconMap)> iconsAvailable;
};

TEST(SpriteAtlas, NotifiesRequestorWhenSpriteIsLoaded) {
    SpriteAtlas atlas;
    StubIconRequestor requestor;
    bool notified = false;

    requestor.iconsAvailable = [&] (IconMap) {
        notified = true;
    };

    atlas.getIcons(requestor, {"one"});
    ASSERT_FALSE(notified);

    atlas.onSpriteLoaded();
    ASSERT_TRUE(notified);
}

TEST(SpriteAtlas, NotifiesRequestorImmediatelyIfDependenciesAreSatisfied) {
    SpriteAtlas atlas;
    StubIconRequestor requestor;
    bool notified = false;

    requestor.iconsAvailable = [&] (IconMap) {
        notified = true;
    };

    atlas.addImage(makeMutable<style::Image::Impl>("one", PremultipliedImage({ 16, 16 }), 2));
    atlas.getIcons(requestor, {"one"});

    ASSERT_TRUE(notified);
}
