#include <mbgl/test/util.hpp>
#include <mbgl/test/fixture_log_observer.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/stub_style_observer.hpp>

#include <mbgl/sprite/sprite_store.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/io.hpp>

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
    SpriteStoreTest() = default;

    util::RunLoop loop;
    StubFileSource fileSource;
    StubStyleObserver observer;
    SpriteStore spriteStore = { 1.0 };

    void run() {
        // Squelch logging.
        Log::setObserver(std::make_unique<Log::NullObserver>());

        spriteStore.setObserver(&observer);
        spriteStore.load("test/fixtures/resources/sprite", fileSource);

        loop.run();
    }

    void end() {
        loop.stop();
    }
};

Response successfulSpriteImageResponse(const Resource& resource) {
    EXPECT_EQ("test/fixtures/resources/sprite.png", resource.url);
    Response response;
    response.data = std::make_shared<std::string>(util::read_file(resource.url));
    return response;
}

Response successfulSpriteJSONResponse(const Resource& resource) {
    EXPECT_EQ("test/fixtures/resources/sprite.json", resource.url);
    Response response;
    response.data = std::make_shared<std::string>(util::read_file(resource.url));
    return response;
}

Response failedSpriteResponse(const Resource&) {
    Response response;
    response.error = std::make_unique<Response::Error>(
        Response::Error::Reason::Other,
        "Failed by the test case");
    return response;
}

Response corruptSpriteResponse(const Resource&) {
    Response response;
    response.data = std::make_shared<std::string>("CORRUPT");
    return response;
}

TEST(SpriteStore, LoadingSuccess) {
    SpriteStoreTest test;

    test.fileSource.spriteImageResponse = successfulSpriteImageResponse;
    test.fileSource.spriteJSONResponse = successfulSpriteJSONResponse;

    test.observer.spriteError = [&] (std::exception_ptr error) {
        FAIL() << util::toString(error);
        test.end();
    };

    test.observer.spriteLoaded = [&] () {
        EXPECT_TRUE(!test.spriteStore.getDirty().empty());
        EXPECT_EQ(1.0, test.spriteStore.pixelRatio);
        EXPECT_TRUE(test.spriteStore.isLoaded());
        test.end();
    };

    test.run();
}

TEST(SpriteStore, JSONLoadingFail) {
    SpriteStoreTest test;

    test.fileSource.spriteImageResponse = successfulSpriteImageResponse;
    test.fileSource.spriteJSONResponse = failedSpriteResponse;

    test.observer.spriteError = [&] (std::exception_ptr error) {
        EXPECT_TRUE(error != nullptr);
        EXPECT_EQ("Failed by the test case", util::toString(error));
        EXPECT_FALSE(test.spriteStore.isLoaded());
        test.end();
    };

    test.run();
}

TEST(SpriteStore, ImageLoadingFail) {
    SpriteStoreTest test;

    test.fileSource.spriteImageResponse = failedSpriteResponse;
    test.fileSource.spriteJSONResponse = successfulSpriteJSONResponse;

    test.observer.spriteError = [&] (std::exception_ptr error) {
        EXPECT_TRUE(error != nullptr);
        EXPECT_EQ("Failed by the test case", util::toString(error));
        EXPECT_FALSE(test.spriteStore.isLoaded());
        test.end();
    };

    test.run();
}

TEST(SpriteStore, JSONLoadingCorrupted) {
    SpriteStoreTest test;

    test.fileSource.spriteImageResponse = successfulSpriteImageResponse;
    test.fileSource.spriteJSONResponse = corruptSpriteResponse;

    test.observer.spriteError = [&] (std::exception_ptr error) {
        EXPECT_TRUE(error != nullptr);
        EXPECT_EQ("Failed to parse JSON: Invalid value. at offset 0", util::toString(error));
        EXPECT_FALSE(test.spriteStore.isLoaded());
        test.end();
    };

    test.run();
}

TEST(SpriteStore, ImageLoadingCorrupted) {
    SpriteStoreTest test;

    test.fileSource.spriteImageResponse = corruptSpriteResponse;
    test.fileSource.spriteJSONResponse = successfulSpriteJSONResponse;

    test.observer.spriteError = [&] (std::exception_ptr error) {
        EXPECT_TRUE(error != nullptr);
        // Not asserting on platform-specific error text.
        EXPECT_FALSE(test.spriteStore.isLoaded());
        test.end();
    };

    test.run();
}

TEST(SpriteStore, LoadingCancel) {
    SpriteStoreTest test;

    test.fileSource.spriteImageResponse =
    test.fileSource.spriteJSONResponse = [&] (const Resource&) {
        test.end();
        return optional<Response>();
    };

    test.observer.spriteLoaded = [&] () {
        FAIL() << "Should never be called";
    };

    test.run();
}
