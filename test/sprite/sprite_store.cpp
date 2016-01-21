#include "../fixtures/util.hpp"
#include "../fixtures/fixture_log_observer.hpp"
#include "../fixtures/mock_file_source.hpp"
#include "../fixtures/stub_style_observer.hpp"

#include <mbgl/sprite/sprite_store.hpp>
#include <mbgl/util/run_loop.hpp>

#include <utility>

using namespace mbgl;

TEST(SpriteStore, SpriteStore) {
    FixtureLog log;

    const auto sprite1 = std::make_shared<SpriteImage>(PremultipliedImage(16, 16), 2);
    const auto sprite2 = std::make_shared<SpriteImage>(PremultipliedImage(16, 16), 2);
    const auto sprite3 = std::make_shared<SpriteImage>(PremultipliedImage(16, 16), 2);

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

TEST(SpriteStore, OtherPixelRatio) {
    FixtureLog log;

    const auto sprite1 = std::make_shared<SpriteImage>(PremultipliedImage(8, 8), 1);

    using Sprites = std::map<std::string, std::shared_ptr<const SpriteImage>>;
    SpriteStore store(1);

    // Adding mismatched sprite image
    store.setSprite("one", sprite1);
    EXPECT_EQ(Sprites({ { "one", sprite1 } }), store.getDirty());
}

TEST(SpriteStore, Multiple) {
    const auto sprite1 = std::make_shared<SpriteImage>(PremultipliedImage(16, 16), 2);
    const auto sprite2 = std::make_shared<SpriteImage>(PremultipliedImage(16, 16), 2);

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

TEST(SpriteStore, Replace) {
    FixtureLog log;

    const auto sprite1 = std::make_shared<SpriteImage>(PremultipliedImage(16, 16), 2);
    const auto sprite2 = std::make_shared<SpriteImage>(PremultipliedImage(16, 16), 2);

    using Sprites = std::map<std::string, std::shared_ptr<const SpriteImage>>;
    SpriteStore store(1);

    store.setSprite("sprite", sprite1);
    EXPECT_EQ(sprite1, store.getSprite("sprite"));
    store.setSprite("sprite", sprite2);
    EXPECT_EQ(sprite2, store.getSprite("sprite"));

    EXPECT_EQ(Sprites({ { "sprite", sprite2 } }), store.getDirty());
}

TEST(SpriteStore, ReplaceWithDifferentDimensions) {
    FixtureLog log;

    PremultipliedImage image(16, 16);
    PremultipliedImage image2(18, 18);
    const auto sprite1 = std::make_shared<SpriteImage>(PremultipliedImage(16, 16), 2);
    const auto sprite2 = std::make_shared<SpriteImage>(PremultipliedImage(18, 18), 2);

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

class SpriteStoreTest {
public:
    SpriteStoreTest(MockFileSource::Type type, const std::string& resource)
        : fileSource(type, resource),
          spriteStore(1.0) {}

    util::ThreadContext context { "Map", util::ThreadType::Map, util::ThreadPriority::Regular };
    util::RunLoop loop;
    MockFileSource fileSource;
    StubStyleObserver observer;
    SpriteStore spriteStore;

    void run(const std::string& url) {
        // Squelch logging.
        Log::setObserver(std::make_unique<Log::NullObserver>());

        util::ThreadContext::Set(&context);
        util::ThreadContext::setFileSource(&fileSource);

        spriteStore.setObserver(&observer);
        spriteStore.setURL(url);

        loop.run();
    }

    void end() {
        loop.stop();
    }
};

TEST(SpriteStore, LoadingSuccess) {
    SpriteStoreTest test(MockFileSource::Success, "");

    test.observer.spriteError = [&] (std::exception_ptr) {
        FAIL();
        test.end();
    };

    test.observer.spriteLoaded = [&] () {
        ASSERT_TRUE(!test.spriteStore.getDirty().empty());
        ASSERT_EQ(test.spriteStore.pixelRatio, 1.0);
        ASSERT_TRUE(test.spriteStore.isLoaded());
        test.end();
    };

    test.run("test/fixtures/resources/sprite");
}

TEST(SpriteStore, LoadingFail) {
    SpriteStoreTest test(MockFileSource::RequestFail, "sprite.json");

    test.observer.spriteError = [&] (std::exception_ptr error) {
        ASSERT_TRUE(error != nullptr);
        ASSERT_FALSE(test.spriteStore.isLoaded());
        test.end();
    };

    test.run("test/fixtures/resources/sprite");
}

TEST(SpriteStore, LoadingCorrupted) {
    SpriteStoreTest test(MockFileSource::RequestWithCorruptedData, "sprite.json");

    test.observer.spriteError = [&] (std::exception_ptr error) {
        ASSERT_TRUE(error != nullptr);
        ASSERT_FALSE(test.spriteStore.isLoaded());
        test.end();
    };

    test.run("test/fixtures/resources/sprite");
}

TEST(SpriteStore, LoadingCancel) {
    SpriteStoreTest test(MockFileSource::Success, "sprite.json");

    test.observer.spriteLoaded = [&] () {
        FAIL() << "Should never be called";
    };

    test.fileSource.requestEnqueuedCallback = [&]{
        test.end();
    };

    test.run("test/fixtures/resources/sprite");
}

TEST(SpriteStore, InvalidURL) {
    SpriteStoreTest test(MockFileSource::Success, "");

    test.observer.spriteError = [&] (std::exception_ptr error) {
        ASSERT_TRUE(error != nullptr);
        ASSERT_EQ(test.spriteStore.isLoaded(), false);
        test.end();
    };

    test.run("foo bar");
}
