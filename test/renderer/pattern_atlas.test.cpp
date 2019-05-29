#include <mbgl/test/util.hpp>
#include <mbgl/test/fixture_log_observer.hpp>
#include <mbgl/test/stub_style_observer.hpp>

#include <mbgl/renderer/pattern_atlas.hpp>
#include <mbgl/sprite/sprite_parser.hpp>
#include <mbgl/style/image_impl.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>

#include <utility>

using namespace mbgl;

TEST(PatternAtlas, Basic) {
    FixtureLog log;
    PatternAtlas patternAtlas;

    auto images = parseSprite(util::read_file("test/fixtures/annotations/emerald.png"),
                              util::read_file("test/fixtures/annotations/emerald.json"));
    for (auto& image : images) {
        if (image->getID() == "metro") {
            ASSERT_TRUE(patternAtlas.addPattern(*image->baseImpl));
        }
    }
    auto found = patternAtlas.getPattern("metro");
    ASSERT_TRUE(found);

    auto metro = *found;
    EXPECT_EQ(1, metro.tl()[0]);
    EXPECT_EQ(1, metro.tl()[1]);
    EXPECT_EQ(19, metro.br()[0]);
    EXPECT_EQ(19, metro.br()[1]);
    EXPECT_EQ(18, metro.displaySize()[0]);
    EXPECT_EQ(18, metro.displaySize()[1]);
    EXPECT_EQ(1.0f, metro.pixelRatio);
    EXPECT_EQ(patternAtlas.getPixelSize(), patternAtlas.getAtlasImageForTests().size);

    test::checkImage("test/fixtures/image_manager/basic", patternAtlas.getAtlasImageForTests());
}

TEST(PatternAtlas, Updates) {
    PatternAtlas patternAtlas;

    PremultipliedImage imageA({ 16, 12 });
    imageA.fill(255);

    auto added = patternAtlas.addPattern(*makeMutable<style::Image::Impl>("one", std::move(imageA), 1));
    ASSERT_TRUE(added);
    auto found = patternAtlas.getPattern("one");
    ASSERT_TRUE(found);
    EXPECT_EQ(added->textureRect, found->textureRect);

    auto a = *found;
    EXPECT_EQ(1, a.tl()[0]);
    EXPECT_EQ(1, a.tl()[1]);
    EXPECT_EQ(17, a.br()[0]);
    EXPECT_EQ(13, a.br()[1]);
    EXPECT_EQ(16, a.displaySize()[0]);
    EXPECT_EQ(12, a.displaySize()[1]);
    EXPECT_EQ(1.0f, a.pixelRatio);
    test::checkImage("test/fixtures/image_manager/updates_before", patternAtlas.getAtlasImageForTests());

    auto imageB = makeMutable<style::Image::Impl>("one", PremultipliedImage({ 5, 5 }), 1);
    EXPECT_FALSE(patternAtlas.addPattern(*imageB)); // Already added.

    patternAtlas.removePattern("one");
    ASSERT_FALSE(patternAtlas.getPattern("one"));
    EXPECT_TRUE(patternAtlas.addPattern(*imageB)); 

    auto b = *patternAtlas.getPattern("one");
    EXPECT_EQ(1, b.tl()[0]);
    EXPECT_EQ(1, b.tl()[1]);
    EXPECT_EQ(6, b.br()[0]);
    EXPECT_EQ(6, b.br()[1]);
    EXPECT_EQ(5, b.displaySize()[0]);
    EXPECT_EQ(5, b.displaySize()[1]);
    EXPECT_EQ(1.0f, b.pixelRatio);
    test::checkImage("test/fixtures/image_manager/updates_after", patternAtlas.getAtlasImageForTests());
}
