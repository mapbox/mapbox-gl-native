#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/stub_style_observer.hpp>

#include <mbgl/text/glyph_set.hpp>
#include <mbgl/text/glyph_store.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/platform/log.hpp>

using namespace mbgl;

class GlyphStoreTest {
public:
    util::RunLoop loop;
    StubFileSource fileSource;
    StubStyleObserver observer;
    GlyphStore glyphStore{fileSource};

    void run(const std::string& url,
             const FontStack& fontStack,
             const std::set<GlyphRange>& glyphRanges) {
        // Squelch logging.
        Log::setObserver(std::make_unique<Log::NullObserver>());

        glyphStore.setObserver(&observer);
        glyphStore.setURL(url);
        glyphStore.hasGlyphRanges(fontStack, glyphRanges);

        loop.run();
    }

    void end() {
        loop.stop();
    }
};

TEST(GlyphStore, LoadingSuccess) {
    GlyphStoreTest test;

    test.fileSource.glyphsResponse = [&](const Resource& resource) {
        EXPECT_EQ(Resource::Kind::Glyphs, resource.kind);
        Response response;
        response.data =
            std::make_shared<std::string>(util::read_file("test/fixtures/resources/glyphs.pbf"));
        return response;
    };

    test.observer.glyphsError = [&](const FontStack&, const GlyphRange&, std::exception_ptr) {
        FAIL();
        test.end();
    };

    test.observer.glyphsLoaded = [&](const FontStack&, const GlyphRange&) {
        if (!test.glyphStore.hasGlyphRanges({{"Test Stack"}}, {{0, 255}, {256, 511}}))
            return;

        auto glyphSet = test.glyphStore.getGlyphSet({{"Test Stack"}});
        ASSERT_FALSE(glyphSet->getSDFs().empty());

        test.end();
    };

    test.run("test/fixtures/resources/glyphs.pbf", {{"Test Stack"}}, {{0, 255}, {256, 511}});
}

TEST(GlyphStore, LoadingFail) {
    GlyphStoreTest test;

    test.fileSource.glyphsResponse = [&](const Resource&) {
        Response response;
        response.error = std::make_unique<Response::Error>(Response::Error::Reason::Other,
                                                           "Failed by the test case");
        return response;
    };

    test.observer.glyphsError = [&](const FontStack& fontStack, const GlyphRange& glyphRange,
                                    std::exception_ptr error) {
        EXPECT_EQ(fontStack, FontStack({"Test Stack"}));
        EXPECT_EQ(glyphRange, GlyphRange(0, 255));

        EXPECT_TRUE(error != nullptr);
        EXPECT_EQ(util::toString(error), "Failed by the test case");

        auto glyphSet = test.glyphStore.getGlyphSet({{"Test Stack"}});
        ASSERT_TRUE(glyphSet->getSDFs().empty());
        ASSERT_FALSE(test.glyphStore.hasGlyphRanges({{"Test Stack"}}, {{0, 255}}));

        test.end();
    };

    test.run("test/fixtures/resources/glyphs.pbf", {{"Test Stack"}}, {{0, 255}});
}

TEST(GlyphStore, LoadingCorrupted) {
    GlyphStoreTest test;

    test.fileSource.glyphsResponse = [&](const Resource&) {
        Response response;
        response.data = std::make_unique<std::string>("CORRUPTED");
        return response;
    };

    test.observer.glyphsError = [&](const FontStack& fontStack, const GlyphRange& glyphRange,
                                    std::exception_ptr error) {
        EXPECT_EQ(fontStack, FontStack({"Test Stack"}));
        EXPECT_EQ(glyphRange, GlyphRange(0, 255));

        EXPECT_TRUE(error != nullptr);
        EXPECT_EQ(util::toString(error), "unknown pbf field type exception");

        auto glyphSet = test.glyphStore.getGlyphSet({{"Test Stack"}});
        ASSERT_TRUE(glyphSet->getSDFs().empty());
        ASSERT_FALSE(test.glyphStore.hasGlyphRanges({{"Test Stack"}}, {{0, 255}}));

        test.end();
    };

    test.run("test/fixtures/resources/glyphs.pbf", {{"Test Stack"}}, {{0, 255}});
}

TEST(GlyphStore, LoadingCancel) {
    GlyphStoreTest test;

    test.fileSource.glyphsResponse = [&](const Resource&) {
        test.end();
        return optional<Response>();
    };

    test.observer.glyphsLoaded = [&](const FontStack&, const GlyphRange&) {
        FAIL() << "Should never be called";
    };

    test.run("test/fixtures/resources/glyphs.pbf", {{"Test Stack"}}, {{0, 255}});
}
