#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>

#include <mbgl/text/glyph_manager.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/logging.hpp>

using namespace mbgl;

class StubGlyphManagerObserver : public GlyphManagerObserver {
public:
    void onGlyphsLoaded(const FontStack& fontStack, const GlyphRange& glyphRange) override {
        if (glyphsLoaded) glyphsLoaded(fontStack, glyphRange);
    }

    void onGlyphsError(const FontStack& fontStack, const GlyphRange& glyphRange, std::exception_ptr error) override {
        if (glyphsError) glyphsError(fontStack, glyphRange, error);
    }

    std::function<void (const FontStack&, const GlyphRange&)> glyphsLoaded;
    std::function<void (const FontStack&, const GlyphRange&, std::exception_ptr)> glyphsError;
};

class StubGlyphRequestor : public GlyphRequestor {
public:
    void onGlyphsAvailable(GlyphMap glyphs) override {
        if (glyphsAvailable) glyphsAvailable(std::move(glyphs));
    }

    std::function<void (GlyphMap)> glyphsAvailable;
};

class GlyphManagerTest {
public:
    util::RunLoop loop;
    StubFileSource fileSource;
    StubGlyphManagerObserver observer;
    StubGlyphRequestor requestor;
    GlyphManager glyphManager { fileSource };

    void run(const std::string& url, GlyphDependencies dependencies) {
        // Squelch logging.
        Log::setObserver(std::make_unique<Log::NullObserver>());

        glyphManager.setURL(url);
        glyphManager.setObserver(&observer);
        glyphManager.getGlyphs(requestor, std::move(dependencies));

        loop.run();
    }

    void end() {
        loop.stop();
    }
};

TEST(GlyphManager, LoadingSuccess) {
    GlyphManagerTest test;

    test.fileSource.glyphsResponse = [&] (const Resource& resource) {
        EXPECT_EQ(Resource::Kind::Glyphs, resource.kind);
        Response response;
        response.data = std::make_shared<std::string>(util::read_file("test/fixtures/resources/glyphs.pbf"));
        return response;
    };

    test.observer.glyphsError = [&] (const FontStack&, const GlyphRange&, std::exception_ptr) {
        FAIL();
        test.end();
    };

    test.observer.glyphsLoaded = [&] (const FontStack& fontStack, const GlyphRange& range) {
        ASSERT_EQ(fontStack, FontStack {{"Test Stack"}});
        ASSERT_EQ(range, GlyphRange(0, 255));
    };

    test.requestor.glyphsAvailable = [&] (GlyphMap glyphs) {
        const auto& testPositions = glyphs.at({{"Test Stack"}});

        ASSERT_EQ(testPositions.size(), 3u);
        ASSERT_EQ(testPositions.count(u'a'), 1u);
        ASSERT_EQ(testPositions.count(u'å'), 1u);
        ASSERT_EQ(testPositions.count(u' '), 1u);
        ASSERT_TRUE(bool(testPositions.at(u' ')));

        test.end();
    };

    test.run(
        "test/fixtures/resources/glyphs.pbf",
        GlyphDependencies {
            {{{"Test Stack"}}, {u'a', u'å', u' '}}
        });
}

TEST(GlyphManager, LoadingFail) {
    GlyphManagerTest test;

    test.fileSource.glyphsResponse = [&] (const Resource&) {
        Response response;
        response.error = std::make_unique<Response::Error>(
            Response::Error::Reason::Other,
            "Failed by the test case");
        return response;
    };

    test.observer.glyphsError = [&] (const FontStack& fontStack, const GlyphRange& glyphRange, std::exception_ptr error) {
        EXPECT_EQ(fontStack, FontStack({"Test Stack"}));
        EXPECT_EQ(glyphRange, GlyphRange(0, 255));

        EXPECT_TRUE(error != nullptr);
        EXPECT_EQ(util::toString(error), "Failed by the test case");

        test.end();
    };

    test.requestor.glyphsAvailable = [&] (GlyphMap) {
        FAIL();
        test.end();
    };

    test.run(
        "test/fixtures/resources/glyphs.pbf",
        GlyphDependencies {
            {{{"Test Stack"}}, {u'a', u'å'}}
        });
}

TEST(GlyphManager, LoadingCorrupted) {
    GlyphManagerTest test;

    test.fileSource.glyphsResponse = [&] (const Resource&) {
        Response response;
        response.data = std::make_unique<std::string>("CORRUPTED");
        return response;
    };

    test.observer.glyphsError = [&] (const FontStack& fontStack, const GlyphRange& glyphRange, std::exception_ptr error) {
        EXPECT_EQ(fontStack, FontStack({"Test Stack"}));
        EXPECT_EQ(glyphRange, GlyphRange(0, 255));

        EXPECT_TRUE(error != nullptr);
        EXPECT_EQ(util::toString(error), "unknown pbf field type exception");

        test.end();
    };

    test.requestor.glyphsAvailable = [&] (GlyphMap) {
        FAIL();
        test.end();
    };

    test.run(
        "test/fixtures/resources/glyphs.pbf",
        GlyphDependencies {
            {{{"Test Stack"}}, {u'a', u'å'}}
        });
}

TEST(GlyphManager, LoadingCancel) {
    GlyphManagerTest test;

    test.fileSource.glyphsResponse = [&] (const Resource&) {
        test.end();
        return optional<Response>();
    };

    test.observer.glyphsLoaded = [&] (const FontStack&, const GlyphRange&) {
        FAIL() << "Should never be called";
    };

    test.run(
        "test/fixtures/resources/glyphs.pbf",
        GlyphDependencies {
            {{{"Test Stack"}}, {u'a', u'å'}}
        });
}

TEST(GlyphManager, LoadingInvalid) {
    GlyphManagerTest test;

    test.fileSource.glyphsResponse = [&] (const Resource& resource) {
        EXPECT_EQ(Resource::Kind::Glyphs, resource.kind);
        Response response;
        response.data = std::make_shared<std::string>(util::read_file("test/fixtures/resources/fake_glyphs-0-255.pbf"));
        return response;
    };

    test.observer.glyphsError = [&] (const FontStack&, const GlyphRange&, std::exception_ptr) {
        FAIL();
        test.end();
    };

    test.observer.glyphsLoaded = [&] (const FontStack& fontStack, const GlyphRange& range) {
        ASSERT_EQ(fontStack, FontStack {{"Test Stack"}});
        ASSERT_EQ(range, GlyphRange(0, 255));
    };

    test.requestor.glyphsAvailable = [&] (GlyphMap glyphs) {
        const auto& testPositions = glyphs.at({{"Test Stack"}});

        ASSERT_EQ(testPositions.size(), 2u);
        ASSERT_FALSE(bool(testPositions.at(u'A')));
        ASSERT_TRUE(bool(testPositions.at(u'E')));

        test.end();
    };

    test.run(
        "test/fixtures/resources/glyphs.pbf",
        GlyphDependencies {
            {{{"Test Stack"}}, {u'A', u'E'}}
        });
}

TEST(GlyphManager, ImmediateFileSource) {
    class GlyphManagerTestSynchronous {
    public:
        util::RunLoop loop;
        StubFileSource fileSource = { StubFileSource::ResponseType::Synchronous };
        StubGlyphManagerObserver observer;
        StubGlyphRequestor requestor;
        GlyphManager glyphManager { fileSource };

        void run(const std::string& url, GlyphDependencies dependencies) {
            // Squelch logging.
            Log::setObserver(std::make_unique<Log::NullObserver>());

            glyphManager.setURL(url);
            glyphManager.setObserver(&observer);
            glyphManager.getGlyphs(requestor, std::move(dependencies));

            loop.run();
        }

        void end() {
            loop.stop();
        }
    };

    GlyphManagerTestSynchronous test;

    test.fileSource.glyphsResponse = [&] (const Resource&) {
        Response response;
        response.data = std::make_shared<std::string>(util::read_file("test/fixtures/resources/glyphs.pbf"));
        return response;
    };

    test.observer.glyphsError = [&] (const FontStack&, const GlyphRange&, std::exception_ptr) {
        FAIL();
        test.end();
    };

    test.requestor.glyphsAvailable = [&] (GlyphMap) {
        test.end();
    };

    test.run(
        "test/fixtures/resources/glyphs.pbf",
        GlyphDependencies {
            {{{"Test Stack"}}, {u'a', u'å', u' '}}
        });
}
