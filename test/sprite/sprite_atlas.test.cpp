#include <mbgl/test/util.hpp>
#include <mbgl/test/fixture_log_observer.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/stub_style_observer.hpp>

#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/sprite/sprite_parser.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>

#include <utility>

using namespace mbgl;

namespace {

auto readImage(const std::string& name) {
    return decodeImage(util::read_file(name));
}

} // namespace

TEST(SpriteAtlas, Basic) {
    FixtureLog log;

    auto spriteParseResult = parseSprite(util::read_file("test/fixtures/annotations/emerald.png"),
                                         util::read_file("test/fixtures/annotations/emerald.json"));

    SpriteAtlas atlas({ 63, 112 }, 1);
    atlas.setSprites(spriteParseResult.get<Sprites>());

    EXPECT_EQ(1.0f, atlas.getPixelRatio());
    EXPECT_EQ(63u, atlas.getSize().width);
    EXPECT_EQ(112u, atlas.getSize().height);

    auto metro = *atlas.getImage("metro", SpritePatternMode::Single);
    EXPECT_EQ(0, metro.pos.x);
    EXPECT_EQ(0, metro.pos.y);
    EXPECT_EQ(20, metro.pos.w);
    EXPECT_EQ(20, metro.pos.h);
    EXPECT_EQ(18, metro.spriteImage->getWidth());
    EXPECT_EQ(18, metro.spriteImage->getHeight());
    EXPECT_EQ(18u, metro.spriteImage->image.size.width);
    EXPECT_EQ(18u, metro.spriteImage->image.size.height);
    EXPECT_EQ(1.0f, metro.spriteImage->pixelRatio);

    EXPECT_EQ(63u, atlas.getAtlasImage().size.width);
    EXPECT_EQ(112u, atlas.getAtlasImage().size.height);

    auto pos = *atlas.getPosition("metro", SpritePatternMode::Single);
    EXPECT_DOUBLE_EQ(18, pos.size[0]);
    EXPECT_DOUBLE_EQ(18, pos.size[1]);
    EXPECT_DOUBLE_EQ(1.0f / 63, pos.tl[0]);
    EXPECT_DOUBLE_EQ(1.0f / 112, pos.tl[1]);
    EXPECT_DOUBLE_EQ(19.0f / 63, pos.br[0]);
    EXPECT_DOUBLE_EQ(19.0f / 112, pos.br[1]);

    auto missing = atlas.getImage("doesnotexist", SpritePatternMode::Single);
    EXPECT_FALSE(missing);

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Info,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't find sprite named 'doesnotexist'",
                  }));

    // Different wrapping mode produces different image.
    auto metro2 = *atlas.getImage("metro", SpritePatternMode::Repeating);
    EXPECT_EQ(20, metro2.pos.x);
    EXPECT_EQ(0, metro2.pos.y);
    EXPECT_EQ(20, metro2.pos.w);
    EXPECT_EQ(20, metro2.pos.h);

    EXPECT_EQ(readImage("test/fixtures/annotations/result-spriteatlas.png"), atlas.getAtlasImage());
}

TEST(SpriteAtlas, Size) {
    auto spriteParseResult = parseSprite(util::read_file("test/fixtures/annotations/emerald.png"),
                                         util::read_file("test/fixtures/annotations/emerald.json"));

    SpriteAtlas atlas({ 63, 112 }, 1.4);
    atlas.setSprites(spriteParseResult.get<Sprites>());

    EXPECT_DOUBLE_EQ(1.4f, atlas.getPixelRatio());
    EXPECT_EQ(63u, atlas.getSize().width);
    EXPECT_EQ(112u, atlas.getSize().height);

    auto metro = *atlas.getImage("metro", SpritePatternMode::Single);
    EXPECT_EQ(0, metro.pos.x);
    EXPECT_EQ(0, metro.pos.y);
    EXPECT_EQ(16, metro.pos.w);
    EXPECT_EQ(16, metro.pos.h);
    EXPECT_EQ(18, metro.spriteImage->getWidth());
    EXPECT_EQ(18, metro.spriteImage->getHeight());
    EXPECT_EQ(18u, metro.spriteImage->image.size.width);
    EXPECT_EQ(18u, metro.spriteImage->image.size.height);
    EXPECT_EQ(1.0f, metro.spriteImage->pixelRatio);

    // Now the image was created lazily.
    EXPECT_EQ(89u, atlas.getAtlasImage().size.width);
    EXPECT_EQ(157u, atlas.getAtlasImage().size.height);

    EXPECT_EQ(readImage("test/fixtures/annotations/result-spriteatlassize.png"),
              atlas.getAtlasImage());
}

TEST(SpriteAtlas, Updates) {
    SpriteAtlas atlas({ 32, 32 }, 1);

    EXPECT_EQ(1.0f, atlas.getPixelRatio());
    EXPECT_EQ(32u, atlas.getSize().width);
    EXPECT_EQ(32u, atlas.getSize().height);

    atlas.setSprite("one", std::make_shared<SpriteImage>(PremultipliedImage({ 16, 12 }), 1));
    auto one = *atlas.getImage("one", SpritePatternMode::Single);
    EXPECT_EQ(0, one.pos.x);
    EXPECT_EQ(0, one.pos.y);
    EXPECT_EQ(20, one.pos.w);
    EXPECT_EQ(16, one.pos.h);
    EXPECT_EQ(16, one.spriteImage->getWidth());
    EXPECT_EQ(12, one.spriteImage->getHeight());
    EXPECT_EQ(16u, one.spriteImage->image.size.width);
    EXPECT_EQ(12u, one.spriteImage->image.size.height);
    EXPECT_EQ(1.0f, one.spriteImage->pixelRatio);

    // Now the image was created lazily.
    EXPECT_EQ(32u, atlas.getAtlasImage().size.width);
    EXPECT_EQ(32u, atlas.getAtlasImage().size.height);

    EXPECT_EQ(readImage("test/fixtures/annotations/result-spriteatlas-empty.png"),
              atlas.getAtlasImage());

    // Update sprite
    PremultipliedImage image2({ 16, 12 });
    for (size_t i = 0; i < image2.bytes(); i++) {
        image2.data.get()[i] = 255;
    }
    auto newSprite = std::make_shared<SpriteImage>(std::move(image2), 1);
    atlas.setSprite("one", newSprite);
    ASSERT_EQ(newSprite, atlas.getSprite("one"));

    // Atlas texture hasn't changed yet.
    EXPECT_EQ(readImage("test/fixtures/annotations/result-spriteatlas-empty.png"),
              atlas.getAtlasImage());

    atlas.updateDirty();

    // Now the atlas texture has changed.
    EXPECT_EQ(readImage("test/fixtures/annotations/result-spriteatlas-updated.png"),
              atlas.getAtlasImage());
}

TEST(SpriteAtlas, AddRemove) {
    FixtureLog log;

    const auto sprite1 = std::make_shared<SpriteImage>(PremultipliedImage({ 16, 16 }), 2);
    const auto sprite2 = std::make_shared<SpriteImage>(PremultipliedImage({ 16, 16 }), 2);
    const auto sprite3 = std::make_shared<SpriteImage>(PremultipliedImage({ 16, 16 }), 2);

    SpriteAtlas atlas({ 32, 32 }, 1);

    // Adding single
    atlas.setSprite("one", sprite1);

    // Adding multiple
    atlas.setSprite("two", sprite2);
    atlas.setSprite("three", sprite3);

    // Removing
    atlas.removeSprite("one");
    atlas.removeSprite("two");

    // Accessing
    EXPECT_EQ(sprite3, atlas.getSprite("three"));

    EXPECT_TRUE(log.empty());

    EXPECT_EQ(nullptr, atlas.getSprite("two"));
    EXPECT_EQ(nullptr, atlas.getSprite("four"));

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
    atlas.setSprite("three", sprite1);
}

TEST(SpriteAtlas, OtherPixelRatio) {
    FixtureLog log;

    const auto sprite1 = std::make_shared<SpriteImage>(PremultipliedImage({ 8, 8 }), 1);

    SpriteAtlas atlas({ 32, 32 }, 1);

    // Adding mismatched sprite image
    atlas.setSprite("one", sprite1);
}

TEST(SpriteAtlas, Multiple) {
    const auto sprite1 = std::make_shared<SpriteImage>(PremultipliedImage({ 16, 16 }), 2);
    const auto sprite2 = std::make_shared<SpriteImage>(PremultipliedImage({ 16, 16 }), 2);

    SpriteAtlas atlas({ 32, 32 }, 1);

    atlas.setSprites({
        { "one", sprite1 }, { "two", sprite2 },
    });
}

TEST(SpriteAtlas, Replace) {
    FixtureLog log;

    const auto sprite1 = std::make_shared<SpriteImage>(PremultipliedImage({ 16, 16 }), 2);
    const auto sprite2 = std::make_shared<SpriteImage>(PremultipliedImage({ 16, 16 }), 2);

    SpriteAtlas atlas({ 32, 32 }, 1);

    atlas.setSprite("sprite", sprite1);
    EXPECT_EQ(sprite1, atlas.getSprite("sprite"));
    atlas.setSprite("sprite", sprite2);
    EXPECT_EQ(sprite2, atlas.getSprite("sprite"));
}

TEST(SpriteAtlas, ReplaceWithDifferentDimensions) {
    FixtureLog log;

    PremultipliedImage image({ 16, 16 });
    PremultipliedImage image2({ 18, 18 });
    const auto sprite1 = std::make_shared<SpriteImage>(PremultipliedImage({ 16, 16 }), 2);
    const auto sprite2 = std::make_shared<SpriteImage>(PremultipliedImage({ 18, 18 }), 2);

    SpriteAtlas atlas({ 32, 32 }, 1);

    atlas.setSprite("sprite", sprite1);
    atlas.setSprite("sprite", sprite2);

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Warning,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't change sprite dimensions for 'sprite'",
                  }));

    EXPECT_EQ(sprite1, atlas.getSprite("sprite"));
}

class SpriteAtlasTest {
public:
    SpriteAtlasTest() = default;

    util::RunLoop loop;
    StubFileSource fileSource;
    StubStyleObserver observer;
    SpriteAtlas spriteAtlas{ { 32, 32 }, 1 };

    void run() {
        // Squelch logging.
        Log::setObserver(std::make_unique<Log::NullObserver>());

        spriteAtlas.setObserver(&observer);
        spriteAtlas.load("test/fixtures/resources/sprite", fileSource);

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

TEST(SpriteAtlas, LoadingSuccess) {
    SpriteAtlasTest test;

    test.fileSource.spriteImageResponse = successfulSpriteImageResponse;
    test.fileSource.spriteJSONResponse = successfulSpriteJSONResponse;

    test.observer.spriteError = [&] (std::exception_ptr error) {
        FAIL() << util::toString(error);
        test.end();
    };

    test.observer.spriteLoaded = [&] () {
        EXPECT_EQ(1.0, test.spriteAtlas.getPixelRatio());
        EXPECT_TRUE(test.spriteAtlas.isLoaded());
        test.end();
    };

    test.run();
}

TEST(SpriteAtlas, JSONLoadingFail) {
    SpriteAtlasTest test;

    test.fileSource.spriteImageResponse = successfulSpriteImageResponse;
    test.fileSource.spriteJSONResponse = failedSpriteResponse;

    test.observer.spriteError = [&] (std::exception_ptr error) {
        EXPECT_TRUE(error != nullptr);
        EXPECT_EQ("Failed by the test case", util::toString(error));
        EXPECT_FALSE(test.spriteAtlas.isLoaded());
        test.end();
    };

    test.run();
}

TEST(SpriteAtlas, ImageLoadingFail) {
    SpriteAtlasTest test;

    test.fileSource.spriteImageResponse = failedSpriteResponse;
    test.fileSource.spriteJSONResponse = successfulSpriteJSONResponse;

    test.observer.spriteError = [&] (std::exception_ptr error) {
        EXPECT_TRUE(error != nullptr);
        EXPECT_EQ("Failed by the test case", util::toString(error));
        EXPECT_FALSE(test.spriteAtlas.isLoaded());
        test.end();
    };

    test.run();
}

TEST(SpriteAtlas, JSONLoadingCorrupted) {
    SpriteAtlasTest test;

    test.fileSource.spriteImageResponse = successfulSpriteImageResponse;
    test.fileSource.spriteJSONResponse = corruptSpriteResponse;

    test.observer.spriteError = [&] (std::exception_ptr error) {
        EXPECT_TRUE(error != nullptr);
        EXPECT_EQ("Failed to parse JSON: Invalid value. at offset 0", util::toString(error));
        EXPECT_FALSE(test.spriteAtlas.isLoaded());
        test.end();
    };

    test.run();
}

TEST(SpriteAtlas, ImageLoadingCorrupted) {
    SpriteAtlasTest test;

    test.fileSource.spriteImageResponse = corruptSpriteResponse;
    test.fileSource.spriteJSONResponse = successfulSpriteJSONResponse;

    test.observer.spriteError = [&] (std::exception_ptr error) {
        EXPECT_TRUE(error != nullptr);
        // Not asserting on platform-specific error text.
        EXPECT_FALSE(test.spriteAtlas.isLoaded());
        test.end();
    };

    test.run();
}

TEST(SpriteAtlas, LoadingCancel) {
    SpriteAtlasTest test;

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
