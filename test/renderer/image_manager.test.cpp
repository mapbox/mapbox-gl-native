#include <mbgl/test/util.hpp>
#include <mbgl/test/fixture_log_observer.hpp>
#include <mbgl/test/stub_style_observer.hpp>

#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/renderer/image_manager_observer.hpp>
#include <mbgl/sprite/sprite_parser.hpp>
#include <mbgl/style/image_impl.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/run_loop.hpp>
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
        auto* stored = imageManager.getImage(image->getID());
        ASSERT_TRUE(stored);
        EXPECT_EQ(image->getImage().size, stored->image.size);
    }
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

TEST(ImageManager, Update) {
    FixtureLog log;
    ImageManager imageManager;

    imageManager.addImage(makeMutable<style::Image::Impl>("one", PremultipliedImage({ 16, 16 }), 2));
    EXPECT_EQ(0, imageManager.updatedImageVersions.size());
    imageManager.updateImage(makeMutable<style::Image::Impl>("one", PremultipliedImage({ 16, 16 }), 2));
    EXPECT_EQ(1, imageManager.updatedImageVersions.size());
    imageManager.removeImage("one");
    EXPECT_EQ(0, imageManager.updatedImageVersions.size());
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
    StubImageRequestor(ImageManager& imageManager) : ImageRequestor(imageManager) {}

    void onImagesAvailable(ImageMap icons, ImageMap patterns, std::unordered_map<std::string, uint32_t> versionMap, uint64_t imageCorrelationID_) final {
        if (imagesAvailable && imageCorrelationID == imageCorrelationID_) imagesAvailable(icons, patterns, versionMap);
    }

    std::function<void (ImageMap, ImageMap, std::unordered_map<std::string, uint32_t>)> imagesAvailable;
    uint64_t imageCorrelationID = 0;
};

TEST(ImageManager, NotifiesRequestorWhenSpriteIsLoaded) {
    util::RunLoop runLoop;
    ImageManager imageManager;
    StubImageRequestor requestor(imageManager);
    bool notified = false;

    ImageManagerObserver observer;
    imageManager.setObserver(&observer);

    requestor.imagesAvailable = [&] (ImageMap, ImageMap, std::unordered_map<std::string, uint32_t>) {
        notified = true;
    };

    uint64_t imageCorrelationID = 0;
    ImageDependencies dependencies;
    dependencies.emplace("one", ImageType::Icon);
    imageManager.getImages(requestor, std::make_pair(dependencies, imageCorrelationID));
    runLoop.runOnce();

    ASSERT_FALSE(notified);

    imageManager.setLoaded(true);
    runLoop.runOnce();
    ASSERT_FALSE(notified);
    imageManager.notifyIfMissingImageAdded();
    runLoop.runOnce();
    ASSERT_TRUE(notified);
}

TEST(ImageManager, NotifiesRequestorImmediatelyIfDependenciesAreSatisfied) {
    ImageManager imageManager;
    StubImageRequestor requestor(imageManager);
    bool notified = false;

    requestor.imagesAvailable = [&] (ImageMap, ImageMap, std::unordered_map<std::string, uint32_t>) {
        notified = true;
    };

    uint64_t imageCorrelationID = 0;
    ImageDependencies dependencies;
    dependencies.emplace("one", ImageType::Icon);
    imageManager.addImage(makeMutable<style::Image::Impl>("one", PremultipliedImage({ 16, 16 }), 2));
    imageManager.getImages(requestor, std::make_pair(dependencies, imageCorrelationID));

    ASSERT_TRUE(notified);
}


class StubImageManagerObserver : public ImageManagerObserver {
    public:
    int count = 0;
    std::function<void (const std::string&)> imageMissing = [](const std::string&){};
    void onStyleImageMissing(const std::string& id, std::function<void()> done) override {
        count++;
        imageMissing(id);
        done();
    }

    std::function<void (const std::vector<std::string>&)> removeUnusedStyleImages = [](const std::vector<std::string>&){};
    void onRemoveUnusedStyleImages(const std::vector<std::string>& unusedImageIDs) override {
        removeUnusedStyleImages(unusedImageIDs);
    }
};

TEST(ImageManager, OnStyleImageMissingBeforeSpriteLoaded) {
    util::RunLoop runLoop;
    ImageManager imageManager;
    StubImageRequestor requestor(imageManager);
    StubImageManagerObserver observer;

    imageManager.setObserver(&observer);

    bool notified = false;

    requestor.imagesAvailable = [&] (ImageMap, ImageMap, std::unordered_map<std::string, uint32_t>) {
        notified = true;
    };

    uint64_t imageCorrelationID = 0;
    ImageDependencies dependencies;
    dependencies.emplace("pre", ImageType::Icon);
    imageManager.getImages(requestor, std::make_pair(dependencies, imageCorrelationID));
    runLoop.runOnce();

    EXPECT_EQ(observer.count, 0);
    ASSERT_FALSE(notified);

    imageManager.setLoaded(true);
    runLoop.runOnce();

    EXPECT_EQ(observer.count, 1);
    ASSERT_FALSE(notified);

    imageManager.notifyIfMissingImageAdded();
    runLoop.runOnce();

    EXPECT_EQ(observer.count, 1);
    ASSERT_TRUE(notified);

}

TEST(ImageManager, OnStyleImageMissingAfterSpriteLoaded) {
    util::RunLoop runLoop;
    ImageManager imageManager;
    StubImageRequestor requestor(imageManager);
    StubImageManagerObserver observer;

    imageManager.setObserver(&observer);

    bool notified = false;

    requestor.imagesAvailable = [&] (ImageMap, ImageMap, std::unordered_map<std::string, uint32_t>) {
        notified = true;
    };

    EXPECT_EQ(observer.count, 0);
    ASSERT_FALSE(notified);

    imageManager.setLoaded(true);
    runLoop.runOnce();

    uint64_t imageCorrelationID = 0;
    ImageDependencies dependencies;
    dependencies.emplace("after", ImageType::Icon);
    imageManager.getImages(requestor, std::make_pair(dependencies, imageCorrelationID));
    runLoop.runOnce();

    EXPECT_EQ(observer.count, 1);
    ASSERT_FALSE(notified);

    imageManager.notifyIfMissingImageAdded();
    runLoop.runOnce();

    EXPECT_EQ(observer.count, 1);
    ASSERT_TRUE(notified);
}

TEST(ImageManager, RemoveUnusedStyleImages) {
    util::RunLoop runLoop;
    ImageManager imageManager;
    StubImageManagerObserver observer;
    imageManager.setObserver(&observer);
    imageManager.setLoaded(true);

    observer.imageMissing = [&imageManager] (const std::string& id) {
        if (id == "1024px") {
            imageManager.addImage(makeMutable<style::Image::Impl>(id, PremultipliedImage({ 1024, 1024 }), 1));
        } else {
            imageManager.addImage(makeMutable<style::Image::Impl>(id, PremultipliedImage({ 16, 16 }), 1));
        }
    };

    observer.removeUnusedStyleImages = [&imageManager](const std::vector<std::string>& ids) {
        for (const auto& id : ids) {
            assert(imageManager.getImage(id));
            imageManager.removeImage(id);
        }
    };

    // Style sprite.
    imageManager.addImage(makeMutable<style::Image::Impl>("sprite", PremultipliedImage({ 16, 16 }), 1));

    // Single requestor
    {
        std::unique_ptr<StubImageRequestor> requestor = std::make_unique<StubImageRequestor>(imageManager);
        imageManager.getImages(*requestor, std::make_pair(ImageDependencies{{"missing", ImageType::Icon}}, 0ull));
        runLoop.runOnce();
        EXPECT_EQ(observer.count, 1);
        ASSERT_FALSE(imageManager.getImage("missing") == nullptr);
    }

    // Within cache limits, no need to notify client.
    imageManager.reduceMemoryUseIfCacheSizeExceedsLimit();
    runLoop.runOnce();
    ASSERT_FALSE(imageManager.getImage("missing") == nullptr);

    // Simulate OOM case, forces client notification to be issued.
    imageManager.reduceMemoryUse();
    runLoop.runOnce();
    ASSERT_TRUE(imageManager.getImage("missing") == nullptr);
    ASSERT_FALSE(imageManager.getImage("sprite") == nullptr);

    // Single requestor, exceed cache size limit.
    {
        std::unique_ptr<StubImageRequestor> requestor = std::make_unique<StubImageRequestor>(imageManager);
        imageManager.getImages(*requestor, std::make_pair(ImageDependencies{{"1024px", ImageType::Icon}}, 0ull));
        runLoop.runOnce();
        EXPECT_EQ(observer.count, 2);
        ASSERT_FALSE(imageManager.getImage("1024px") == nullptr);
    }

    // Over cache limits, need to notify client.
    imageManager.reduceMemoryUseIfCacheSizeExceedsLimit();
    runLoop.runOnce();
    ASSERT_TRUE(imageManager.getImage("1024px") == nullptr);
    ASSERT_FALSE(imageManager.getImage("sprite") == nullptr);

    // Multiple requestors
    {
        std::unique_ptr<StubImageRequestor> requestor1 = std::make_unique<StubImageRequestor>(imageManager);
        std::unique_ptr<StubImageRequestor> requestor2 = std::make_unique<StubImageRequestor>(imageManager);
        imageManager.getImages(*requestor1, std::make_pair(ImageDependencies{{"missing", ImageType::Icon}}, 0ull));
        imageManager.getImages(*requestor2, std::make_pair(ImageDependencies{{"missing", ImageType::Icon}}, 1ull));
        runLoop.runOnce();
        EXPECT_EQ(observer.count, 3);
        ASSERT_FALSE(imageManager.getImage("missing") == nullptr);
    }

    // Reduce memory usage and check that unused image was deleted when all requestors are destructed.
    imageManager.reduceMemoryUseIfCacheSizeExceedsLimit();
    runLoop.runOnce();
    ASSERT_FALSE(imageManager.getImage("missing") == nullptr);
    imageManager.reduceMemoryUse();
    runLoop.runOnce();
    ASSERT_TRUE(imageManager.getImage("missing") == nullptr);

    // Multiple requestors, check that image resource is not destroyed if there is at least 1 requestor that uses it.
    std::unique_ptr<StubImageRequestor> requestor = std::make_unique<StubImageRequestor>(imageManager);
    {
        std::unique_ptr<StubImageRequestor> requestor1 = std::make_unique<StubImageRequestor>(imageManager);
        imageManager.getImages(*requestor, std::make_pair(ImageDependencies{{"missing", ImageType::Icon}, {"1024px", ImageType::Icon}}, 0ull));
        imageManager.getImages(*requestor1, std::make_pair(ImageDependencies{{"missing", ImageType::Icon}}, 1ull));
        runLoop.runOnce();
        EXPECT_EQ(observer.count, 5);
        ASSERT_FALSE(imageManager.getImage("missing") == nullptr);
        ASSERT_FALSE(imageManager.getImage("1024px") == nullptr);
    }

    // Reduce memory usage and check that requested image is not destructed.
    imageManager.reduceMemoryUseIfCacheSizeExceedsLimit();
    runLoop.runOnce();
    ASSERT_FALSE(imageManager.getImage("missing") == nullptr);
    ASSERT_FALSE(imageManager.getImage("1024px") == nullptr);

    // Release last requestor and check if resource was released when cache size is over the limit.
    requestor.reset();
    imageManager.reduceMemoryUseIfCacheSizeExceedsLimit();
    runLoop.runOnce();
    ASSERT_TRUE(imageManager.getImage("missing") == nullptr);
    ASSERT_TRUE(imageManager.getImage("1024px") == nullptr);
    ASSERT_FALSE(imageManager.getImage("sprite") == nullptr);
}
