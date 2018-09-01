#include <mbgl/test/util.hpp>
#include <mbgl/test/fixture_log_observer.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/stub_style_observer.hpp>

#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/sprite/sprite_parser.hpp>
#include <mbgl/style/image_impl.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/string.hpp>

#include <utility>

using namespace mbgl;

TEST(ImageManager, Missing) {
    ImageManager imageManager;
    EXPECT_FALSE(imageManager.getImage("doesnotexist"));
}

TEST(ImageManager, Basic) {
    FixtureLog log;
    ImageManager imageManager;

    auto images = parseSprite(util::read_file("test/fixtures/annotations/emerald.png"),
                              util::read_file("test/fixtures/annotations/emerald.json"));
    for (auto& image : images) {
        imageManager.addImage(image->baseImpl);
    }

    auto metro = *imageManager.getPattern("metro");
    EXPECT_EQ(1, metro.tl()[0]);
    EXPECT_EQ(1, metro.tl()[1]);
    EXPECT_EQ(19, metro.br()[0]);
    EXPECT_EQ(19, metro.br()[1]);
    EXPECT_EQ(18, metro.displaySize()[0]);
    EXPECT_EQ(18, metro.displaySize()[1]);
    EXPECT_EQ(1.0f, metro.pixelRatio);
    EXPECT_EQ(imageManager.getPixelSize(), imageManager.getAtlasImage().size);

    test::checkImage("test/fixtures/image_manager/basic", imageManager.getAtlasImage());
}

TEST(ImageManager, Updates) {
    ImageManager imageManager;

    PremultipliedImage imageA({ 16, 12 });
    imageA.fill(255);
    imageManager.addImage(makeMutable<style::Image::Impl>("one", std::move(imageA), 1));

    auto a = *imageManager.getPattern("one");
    EXPECT_EQ(1, a.tl()[0]);
    EXPECT_EQ(1, a.tl()[1]);
    EXPECT_EQ(17, a.br()[0]);
    EXPECT_EQ(13, a.br()[1]);
    EXPECT_EQ(16, a.displaySize()[0]);
    EXPECT_EQ(12, a.displaySize()[1]);
    EXPECT_EQ(1.0f, a.pixelRatio);
    test::checkImage("test/fixtures/image_manager/updates_before", imageManager.getAtlasImage());

    PremultipliedImage imageB({ 5, 5 });
    imageA.fill(200);
    imageManager.updateImage(makeMutable<style::Image::Impl>("one", std::move(imageB), 1));

    auto b = *imageManager.getPattern("one");
    EXPECT_EQ(1, b.tl()[0]);
    EXPECT_EQ(1, b.tl()[1]);
    EXPECT_EQ(6, b.br()[0]);
    EXPECT_EQ(6, b.br()[1]);
    EXPECT_EQ(5, b.displaySize()[0]);
    EXPECT_EQ(5, b.displaySize()[1]);
    EXPECT_EQ(1.0f, b.pixelRatio);
    test::checkImage("test/fixtures/image_manager/updates_after", imageManager.getAtlasImage());
}

TEST(ImageManager, AddRemove) {
    FixtureLog log;
    ImageManager imageManager;

    imageManager.addImage(makeMutable<style::Image::Impl>("one", PremultipliedImage({ 16, 16 }), 2));
    imageManager.addImage(makeMutable<style::Image::Impl>("two", PremultipliedImage({ 16, 16 }), 2));
    imageManager.addImage(makeMutable<style::Image::Impl>("three", PremultipliedImage({ 16, 16 }), 2));

    imageManager.removeImage("one");
    imageManager.removeImage("two");

    EXPECT_NE(nullptr, imageManager.getImage("three"));
    EXPECT_EQ(nullptr, imageManager.getImage("two"));
    EXPECT_EQ(nullptr, imageManager.getImage("four"));
}

TEST(ImageManager, RemoveReleasesBinPackRect) {
    FixtureLog log;
    ImageManager imageManager;

    imageManager.addImage(makeMutable<style::Image::Impl>("big", PremultipliedImage({ 32, 32 }), 1));
    EXPECT_TRUE(imageManager.getImage("big"));

    imageManager.removeImage("big");

    imageManager.addImage(makeMutable<style::Image::Impl>("big", PremultipliedImage({ 32, 32 }), 1));
    EXPECT_TRUE(imageManager.getImage("big"));
    EXPECT_TRUE(log.empty());
}

class StubImageRequestor : public ImageRequestor {
public:
    void onImagesAvailable(ImageMap icons, ImageMap patterns, uint64_t imageCorrelationID_) final {
        if (imagesAvailable && imageCorrelationID == imageCorrelationID_) imagesAvailable(icons, patterns);
    }

    std::function<void (ImageMap, ImageMap)> imagesAvailable;
    uint64_t imageCorrelationID = 0;
};

TEST(ImageManager, NotifiesRequestorWhenSpriteIsLoaded) {
    ImageManager imageManager;
    StubImageRequestor requestor;
    bool notified = false;

    requestor.imagesAvailable = [&] (ImageMap, ImageMap) {
        notified = true;
    };

    uint64_t imageCorrelationID = 0;
    ImageDependencies dependencies;
    dependencies.emplace("one", ImageType::Icon);
    imageManager.getImages(requestor, std::make_pair(dependencies, imageCorrelationID));
    ASSERT_FALSE(notified);

    imageManager.setLoaded(true);
    ASSERT_TRUE(notified);
}

TEST(ImageManager, NotifiesRequestorImmediatelyIfDependenciesAreSatisfied) {
    ImageManager imageManager;
    StubImageRequestor requestor;
    bool notified = false;

    requestor.imagesAvailable = [&] (ImageMap, ImageMap) {
        notified = true;
    };

    uint64_t imageCorrelationID = 0;
    ImageDependencies dependencies;
    dependencies.emplace("one", ImageType::Icon);
    imageManager.addImage(makeMutable<style::Image::Impl>("one", PremultipliedImage({ 16, 16 }), 2));
    imageManager.getImages(requestor, std::make_pair(dependencies, imageCorrelationID));

    ASSERT_TRUE(notified);
}
