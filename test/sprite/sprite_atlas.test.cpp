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
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/string.hpp>

#include <utility>

using namespace mbgl;

TEST(SpriteAtlas, Basic) {
    FixtureLog log;
    SpriteAtlas atlas({ 63, 112 }, 1);

    auto images = parseSprite(util::read_file("test/fixtures/annotations/emerald.png"),
                              util::read_file("test/fixtures/annotations/emerald.json"));
    for (auto& pair : images) {
        atlas.addImage(pair.first, std::move(pair.second));
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
    for (auto& pair : images) {
        atlas.addImage(pair.first, std::move(pair.second));
    }

    EXPECT_DOUBLE_EQ(1.4f, atlas.getPixelRatio());
    EXPECT_EQ(63u, atlas.getSize().width);
    EXPECT_EQ(112u, atlas.getSize().height);

    auto metro = *atlas.getIcon("metro");
    float imagePixelRatio = metro.relativePixelRatio * atlas.getPixelRatio();
    EXPECT_EQ(0, metro.pos.x);
    EXPECT_EQ(0, metro.pos.y);
    EXPECT_EQ(16, metro.pos.w);
    EXPECT_EQ(16, metro.pos.h);
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

    atlas.addImage("one", std::make_unique<style::Image>(PremultipliedImage({ 16, 12 }), 1));
    auto one = *atlas.getIcon("one");
    float imagePixelRatio = one.relativePixelRatio * atlas.getPixelRatio();
    EXPECT_EQ(0, one.pos.x);
    EXPECT_EQ(0, one.pos.y);
    EXPECT_EQ(20, one.pos.w);
    EXPECT_EQ(16, one.pos.h);
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
    atlas.addImage("one", std::make_unique<style::Image>(std::move(image2), 1));

    test::checkImage("test/fixtures/sprite_atlas/updates_after", atlas.getAtlasImage());
}

TEST(SpriteAtlas, AddRemove) {
    FixtureLog log;
    SpriteAtlas atlas({ 32, 32 }, 1);

    atlas.addImage("one", std::make_unique<style::Image>(PremultipliedImage({ 16, 16 }), 2));
    atlas.addImage("two", std::make_unique<style::Image>(PremultipliedImage({ 16, 16 }), 2));
    atlas.addImage("three", std::make_unique<style::Image>(PremultipliedImage({ 16, 16 }), 2));

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

    atlas.addImage("big", std::make_unique<style::Image>(PremultipliedImage({ 32, 32 }), 1));
    EXPECT_TRUE(atlas.getIcon("big"));

    atlas.removeImage("big");

    atlas.addImage("big", std::make_unique<style::Image>(PremultipliedImage({ 32, 32 }), 1));
    EXPECT_TRUE(atlas.getIcon("big"));
    EXPECT_TRUE(log.empty());
}

TEST(SpriteAtlas, OtherPixelRatio) {
    FixtureLog log;
    SpriteAtlas atlas({ 32, 32 }, 1);

    // Adding mismatched sprite image
    atlas.addImage("one", std::make_unique<style::Image>(PremultipliedImage({ 8, 8 }), 2));
}

TEST(SpriteAtlas, Replace) {
    FixtureLog log;
    SpriteAtlas atlas({ 32, 32 }, 1);

    atlas.addImage("sprite", std::make_unique<style::Image>(PremultipliedImage({ 16, 16 }), 2));
    auto image = atlas.getImage("sprite");
    atlas.addImage("sprite", std::make_unique<style::Image>(PremultipliedImage({ 16, 16 }), 2));
    EXPECT_NE(image, atlas.getImage("sprite"));
}

TEST(SpriteAtlas, ReplaceWithDifferentDimensions) {
    FixtureLog log;
    SpriteAtlas atlas({ 32, 32 }, 1);

    atlas.addImage("sprite", std::make_unique<style::Image>(PremultipliedImage({ 16, 16 }), 2));
    atlas.addImage("sprite", std::make_unique<style::Image>(PremultipliedImage({ 18, 18 }), 2));

    EXPECT_EQ(1u, log.count({
                      EventSeverity::Warning,
                      Event::Sprite,
                      int64_t(-1),
                      "Can't change sprite dimensions for 'sprite'",
                  }));
}

class SpriteAtlasTest {
public:
    SpriteAtlasTest() = default;

    util::RunLoop loop;
    StubFileSource fileSource;
    StubStyleObserver observer;
    ThreadPool threadPool { 1 };
    SpriteAtlas spriteAtlas{ { 32, 32 }, 1 };

    void run() {
        // Squelch logging.
        Log::setObserver(std::make_unique<Log::NullObserver>());

        spriteAtlas.setObserver(&observer);
        spriteAtlas.load("test/fixtures/resources/sprite", threadPool, fileSource);

        loop.run();
    }

    void end() {
        loop.stop();
    }
};

Response successfulSpriteImageResponse(const Resource& resource) {
    EXPECT_EQ("test/fixtures/resources/sprite.png", resource.url);
    Response response;
    response.data = std::make_unique<std::string>(util::read_file(resource.url));
    return response;
}

Response successfulSpriteJSONResponse(const Resource& resource) {
    EXPECT_EQ("test/fixtures/resources/sprite.json", resource.url);
    Response response;
    response.data = std::make_unique<std::string>(util::read_file(resource.url));
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
    response.data = std::make_unique<std::string>("CORRUPT");
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
