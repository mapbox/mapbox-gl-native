#include <mbgl/test/util.hpp>
#include <mbgl/test/fixture_log_observer.hpp>
#include <mbgl/test/stub_file_source.hpp>

#include <mbgl/sprite/sprite_loader.hpp>
#include <mbgl/sprite/sprite_loader_observer.hpp>
#include <mbgl/sprite/sprite_parser.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/string.hpp>

#include <utility>

using namespace mbgl;
using namespace mbgl::style;

class StubSpriteLoaderObserver : public SpriteLoaderObserver {
public:
    void onSpriteLoaded(std::vector<std::unique_ptr<style::Image>>&& images) override {
        if (spriteLoaded) spriteLoaded(std::move(images));
    }

    void onSpriteError(std::exception_ptr error) override {
        if (spriteError) spriteError(error);
    }

    std::function<void (std::vector<std::unique_ptr<style::Image>>&&)> spriteLoaded;
    std::function<void (std::exception_ptr)> spriteError;
};

class SpriteLoaderTest {
public:
    SpriteLoaderTest() = default;

    util::RunLoop loop;
    StubFileSource fileSource;
    StubSpriteLoaderObserver observer;
    ThreadPool threadPool { 1 };
    SpriteLoader spriteLoader{ 1 };

    void run() {
        // Squelch logging.
        Log::setObserver(std::make_unique<Log::NullObserver>());

        spriteLoader.setObserver(&observer);
        spriteLoader.load("test/fixtures/resources/sprite", threadPool, fileSource);

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

TEST(SpriteLoader, LoadingSuccess) {
    SpriteLoaderTest test;

    test.fileSource.spriteImageResponse = successfulSpriteImageResponse;
    test.fileSource.spriteJSONResponse = successfulSpriteJSONResponse;

    test.observer.spriteError = [&] (std::exception_ptr error) {
        FAIL() << util::toString(error);
        test.end();
    };

    test.observer.spriteLoaded = [&] (std::vector<std::unique_ptr<style::Image>>&& images) {
        EXPECT_EQ(images.size(), 367u);
        test.end();
    };

    test.run();
}

TEST(SpriteLoader, JSONLoadingFail) {
    SpriteLoaderTest test;

    test.fileSource.spriteImageResponse = successfulSpriteImageResponse;
    test.fileSource.spriteJSONResponse = failedSpriteResponse;

    test.observer.spriteError = [&] (std::exception_ptr error) {
        EXPECT_TRUE(error != nullptr);
        EXPECT_EQ("Failed by the test case", util::toString(error));
        test.end();
    };

    test.run();
}

TEST(SpriteLoader, ImageLoadingFail) {
    SpriteLoaderTest test;

    test.fileSource.spriteImageResponse = failedSpriteResponse;
    test.fileSource.spriteJSONResponse = successfulSpriteJSONResponse;

    test.observer.spriteError = [&] (std::exception_ptr error) {
        EXPECT_TRUE(error != nullptr);
        EXPECT_EQ("Failed by the test case", util::toString(error));
        test.end();
    };

    test.run();
}

TEST(SpriteLoader, JSONLoadingCorrupted) {
    SpriteLoaderTest test;

    test.fileSource.spriteImageResponse = successfulSpriteImageResponse;
    test.fileSource.spriteJSONResponse = corruptSpriteResponse;

    test.observer.spriteError = [&] (std::exception_ptr error) {
        EXPECT_TRUE(error != nullptr);
        EXPECT_EQ("Failed to parse JSON: Invalid value. at offset 0", util::toString(error));
        test.end();
    };

    test.run();
}

TEST(SpriteLoader, ImageLoadingCorrupted) {
    SpriteLoaderTest test;

    test.fileSource.spriteImageResponse = corruptSpriteResponse;
    test.fileSource.spriteJSONResponse = successfulSpriteJSONResponse;

    test.observer.spriteError = [&] (std::exception_ptr error) {
        EXPECT_TRUE(error != nullptr);
        // Not asserting on platform-specific error text.
        test.end();
    };

    test.run();
}

TEST(SpriteLoader, LoadingCancel) {
    SpriteLoaderTest test;

    test.fileSource.spriteImageResponse =
    test.fileSource.spriteJSONResponse = [&] (const Resource&) {
        test.end();
        return optional<Response>();
    };

    test.observer.spriteLoaded = [&] (const std::vector<std::unique_ptr<style::Image>>&) {
        FAIL() << "Should never be called";
    };

    test.run();
}
