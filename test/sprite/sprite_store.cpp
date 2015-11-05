#include "../fixtures/util.hpp"
#include "../fixtures/fixture_log_observer.hpp"
#include "../fixtures/mock_file_source.hpp"

#include <mbgl/sprite/sprite_store.hpp>

#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/thread.hpp>

using namespace mbgl;

TEST(Sprite, SpriteStore) {
    FixtureLog log;

    const auto sprite1 = std::make_shared<SpriteImage>(8, 8, 2, std::string(16 * 16 * 4, '\0'));
    const auto sprite2 = std::make_shared<SpriteImage>(8, 8, 2, std::string(16 * 16 * 4, '\0'));
    const auto sprite3 = std::make_shared<SpriteImage>(8, 8, 2, std::string(16 * 16 * 4, '\0'));

    using Sprites = std::map<std::string, std::shared_ptr<const SpriteImage>>;
    SpriteStore store(1);

    // Adding single
    store.setSprite("one", sprite1);
    EXPECT_EQ(Sprites({
                  { "one", sprite1 },
              }),
              store.getDirty());
    EXPECT_EQ(Sprites(), store.getDirty());

    // Adding multiple
    store.setSprite("two", sprite2);
    store.setSprite("three", sprite3);
    EXPECT_EQ(Sprites({
                  { "two", sprite2 }, { "three", sprite3 },
              }),
              store.getDirty());
    EXPECT_EQ(Sprites(), store.getDirty());

    // Removing
    store.removeSprite("one");
    store.removeSprite("two");
    EXPECT_EQ(Sprites({
                  { "one", nullptr }, { "two", nullptr },
              }),
              store.getDirty());
    EXPECT_EQ(Sprites(), store.getDirty());

    // Accessing
    EXPECT_EQ(sprite3, store.getSprite("three"));

    EXPECT_TRUE(log.empty());

    EXPECT_EQ(nullptr, store.getSprite("two"));
    EXPECT_EQ(nullptr, store.getSprite("four"));

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

    // Overwriting
    store.setSprite("three", sprite1);
    EXPECT_EQ(Sprites({
                  { "three", sprite1 },
              }),
              store.getDirty());
    EXPECT_EQ(Sprites(), store.getDirty());
}

TEST(Sprite, SpriteStoreOtherPixelRatio) {
    FixtureLog log;

    const auto sprite1 = std::make_shared<SpriteImage>(8, 8, 1, std::string(8 * 8 * 4, '\0'));

    using Sprites = std::map<std::string, std::shared_ptr<const SpriteImage>>;
    SpriteStore store(1);

    // Adding mismatched sprite image
    store.setSprite("one", sprite1);
    EXPECT_EQ(Sprites({ { "one", sprite1 } }), store.getDirty());
}

TEST(Sprite, SpriteStoreMultiple) {
    const auto sprite1 = std::make_shared<SpriteImage>(8, 8, 2, std::string(16 * 16 * 4, '\0'));
    const auto sprite2 = std::make_shared<SpriteImage>(8, 8, 2, std::string(16 * 16 * 4, '\0'));

    using Sprites = std::map<std::string, std::shared_ptr<const SpriteImage>>;
    SpriteStore store(1);

    store.setSprites({
        { "one", sprite1 }, { "two", sprite2 },
    });
    EXPECT_EQ(Sprites({
                  { "one", sprite1 }, { "two", sprite2 },
              }),
              store.getDirty());
    EXPECT_EQ(Sprites(), store.getDirty());
}

TEST(Sprite, SpriteStoreReplace) {
    FixtureLog log;

    const auto sprite1 = std::make_shared<SpriteImage>(8, 8, 2, std::string(16 * 16 * 4, '\0'));
    const auto sprite2 = std::make_shared<SpriteImage>(8, 8, 2, std::string(16 * 16 * 4, '\0'));

    using Sprites = std::map<std::string, std::shared_ptr<const SpriteImage>>;
    SpriteStore store(1);

    store.setSprite("sprite", sprite1);
    EXPECT_EQ(sprite1, store.getSprite("sprite"));
    store.setSprite("sprite", sprite2);
    EXPECT_EQ(sprite2, store.getSprite("sprite"));

    EXPECT_EQ(Sprites({ { "sprite", sprite2 } }), store.getDirty());
}

TEST(Sprite, SpriteStoreReplaceWithDifferentDimensions) {
    FixtureLog log;

    const auto sprite1 = std::make_shared<SpriteImage>(8, 8, 2, std::string(16 * 16 * 4, '\0'));
    const auto sprite2 = std::make_shared<SpriteImage>(9, 9, 2, std::string(18 * 18 * 4, '\0'));

    using Sprites = std::map<std::string, std::shared_ptr<const SpriteImage>>;
    SpriteStore store(1);

    store.setSprite("sprite", sprite1);
    store.setSprite("sprite", sprite2);

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Warning,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't change sprite dimensions for 'sprite'",
                  }));

    EXPECT_EQ(sprite1, store.getSprite("sprite"));

    EXPECT_EQ(Sprites({ { "sprite", sprite1 } }), store.getDirty());
}

using SpriteTestCallback = std::function<void(SpriteStore*, std::exception_ptr)>;

struct SpriteParams {
    const std::string baseUrl;
    const float pixelRatio;
};

class SpriteThread : public SpriteStore::Observer {
public:
    SpriteThread(FileSource* fileSource, SpriteTestCallback callback) : callback_(callback) {
        util::ThreadContext::setFileSource(fileSource);
    }

    void loadSprite(const SpriteParams& params) {
        spriteStore_.reset(new SpriteStore(params.pixelRatio));
        spriteStore_->setObserver(this);
        spriteStore_->setURL(params.baseUrl);
    }

    void unloadSprite() {
        spriteStore_->setObserver(nullptr);
        spriteStore_.reset();
    }

    void onSpriteLoaded() override {
        callback_(spriteStore_.get(), nullptr);
    }

    void onSpriteLoadingFailed(std::exception_ptr error) override {
        callback_(spriteStore_.get(), error);
    }

private:
    std::unique_ptr<SpriteStore> spriteStore_;
    SpriteTestCallback callback_;
};

class SpriteTest : public testing::Test {
protected:
    void runTest(const SpriteParams& params, FileSource* fileSource, SpriteTestCallback callback) {
        util::RunLoop loop(uv_default_loop());

        async_ = std::make_unique<uv::async>(loop.get(), [&] { loop.stop(); });
        async_->unref();

        const util::ThreadContext context = {"Map", util::ThreadType::Map, util::ThreadPriority::Regular};

        util::Thread<SpriteThread> tester(context, fileSource, callback);
        tester.invoke(&SpriteThread::loadSprite, params);

        uv_run(loop.get(), UV_RUN_DEFAULT);

        tester.invoke(&SpriteThread::unloadSprite);
    }

    void stopTest() {
        async_->send();
    }

private:
    std::unique_ptr<uv::async> async_;
};

TEST_F(SpriteTest, LoadingSuccess) {
    SpriteParams params = {
        "test/fixtures/resources/sprite",
        1.0,
    };

    auto callback = [this, &params](SpriteStore* spriteStore, std::exception_ptr error) {
        ASSERT_TRUE(util::ThreadContext::currentlyOn(util::ThreadType::Map));

        ASSERT_TRUE(error == nullptr);

        ASSERT_TRUE(!spriteStore->getDirty().empty());

        ASSERT_EQ(spriteStore->pixelRatio, params.pixelRatio);
        ASSERT_NE(spriteStore->pixelRatio, 1.5);
        ASSERT_NE(spriteStore->pixelRatio, 2.0);

        ASSERT_TRUE(spriteStore->isLoaded());

        stopTest();
    };

    MockFileSource fileSource(MockFileSource::Success, "");
    runTest(params, &fileSource, callback);
}

TEST_F(SpriteTest, LoadingFail) {
    SpriteParams params = {
        "test/fixtures/resources/sprite",
        1.0,
    };

    auto callback = [this, &params](SpriteStore* spriteStore, std::exception_ptr error) {
        ASSERT_TRUE(util::ThreadContext::currentlyOn(util::ThreadType::Map));

        ASSERT_TRUE(error != nullptr);

        ASSERT_EQ(spriteStore->pixelRatio, params.pixelRatio);
        ASSERT_NE(spriteStore->pixelRatio, 1.5);
        ASSERT_NE(spriteStore->pixelRatio, 2.0);

        ASSERT_FALSE(spriteStore->isLoaded());

        stopTest();
    };

    MockFileSource fileSourceFailSpriteJSON(MockFileSource::RequestFail, "sprite.json");
    runTest(params, &fileSourceFailSpriteJSON, callback);

    MockFileSource fileSourceFailSpriteImage(MockFileSource::RequestFail, "sprite.png");
    runTest(params, &fileSourceFailSpriteImage, callback);

    MockFileSource fileSourceCorruptedSpriteJSON(MockFileSource::RequestWithCorruptedData, "sprite.json");
    runTest(params, &fileSourceCorruptedSpriteJSON, callback);

    MockFileSource fileSourceCorruptedSpriteImage(MockFileSource::RequestWithCorruptedData, "sprite.png");
    runTest(params, &fileSourceCorruptedSpriteImage, callback);
}

TEST_F(SpriteTest, LoadingCancel) {
    SpriteParams params = {
        "test/fixtures/resources/sprite",
        1.0,
    };

    auto callback = [this](SpriteStore*, std::exception_ptr) {
        FAIL() << "Should never be called";
    };

    MockFileSource fileSourceDelaySpriteJSON(MockFileSource::SuccessWithDelay, "sprite.json");
    fileSourceDelaySpriteJSON.setOnRequestDelayedCallback([this]{
        stopTest();
    });
    runTest(params, &fileSourceDelaySpriteJSON, callback);

    MockFileSource fileSourceDelaySpriteImage(MockFileSource::SuccessWithDelay, "sprite.png");
    fileSourceDelaySpriteImage.setOnRequestDelayedCallback([this]{
        stopTest();
    });
    runTest(params, &fileSourceDelaySpriteImage, callback);
}

TEST_F(SpriteTest, InvalidURL) {
    SpriteParams params = {
        "foo bar",
        1.0,
    };

    auto callback = [this](SpriteStore* spriteStore, std::exception_ptr error) {
        ASSERT_TRUE(util::ThreadContext::currentlyOn(util::ThreadType::Map));

        ASSERT_TRUE(error != nullptr);

        ASSERT_EQ(spriteStore->isLoaded(), false);

        stopTest();
    };

    MockFileSource fileSource(MockFileSource::Success, "");
    runTest(params, &fileSource, callback);
}
