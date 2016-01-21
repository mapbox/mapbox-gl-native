#include "../fixtures/util.hpp"
#include "../fixtures/mock_file_source.hpp"
#include "../fixtures/stub_style_observer.hpp"

#include <mbgl/text/font_stack.hpp>
#include <mbgl/text/glyph_store.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/platform/log.hpp>

using namespace mbgl;

class GlyphStoreTest {
public:
    GlyphStoreTest(MockFileSource::Type type, const std::string& resource)
        : fileSource(type, resource) {}

    util::ThreadContext context { "Map", util::ThreadType::Map, util::ThreadPriority::Regular };
    util::RunLoop loop;
    MockFileSource fileSource;
    StubStyleObserver observer;
    GlyphStore glyphStore;

    void run(const std::string& url, const std::string& fontStack, const std::set<GlyphRange>& glyphRanges) {
        // Squelch logging.
        Log::setObserver(std::make_unique<Log::NullObserver>());

        util::ThreadContext::Set(&context);
        util::ThreadContext::setFileSource(&fileSource);

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
    GlyphStoreTest test(MockFileSource::Success, "");

    test.observer.glyphsError = [&] (const std::string&, const GlyphRange&, std::exception_ptr) {
        FAIL();
        test.end();
    };

    test.observer.glyphsLoaded = [&] (const std::string&, const GlyphRange&) {
        if (!test.glyphStore.hasGlyphRanges("Test Stack", {{0, 255}, {256, 511}}))
            return;

        auto fontStack = test.glyphStore.getFontStack("Test Stack");
        ASSERT_FALSE(fontStack->getSDFs().empty());

        test.end();
    };

    test.run(
        "test/fixtures/resources/glyphs.pbf",
        "Test Stack",
        {{0, 255}, {256, 511}});
}

TEST(GlyphStore, LoadingFail) {
    GlyphStoreTest test(MockFileSource::RequestFail, "glyphs.pbf");

    test.observer.glyphsError = [&] (const std::string& fontStack, const GlyphRange& glyphRange, std::exception_ptr error) {
        ASSERT_TRUE(error != nullptr);
        ASSERT_EQ(fontStack, "Test Stack");
        ASSERT_EQ(glyphRange, GlyphRange(0, 255));

        auto stack = test.glyphStore.getFontStack("Test Stack");
        ASSERT_TRUE(stack->getSDFs().empty());
        ASSERT_FALSE(test.glyphStore.hasGlyphRanges("Test Stack", {{0, 255}}));

        test.end();
    };

    test.run(
        "test/fixtures/resources/glyphs.pbf",
        "Test Stack",
        {{0, 255}});
}

TEST(GlyphStore, LoadingCorrupted) {
    GlyphStoreTest test(MockFileSource::RequestWithCorruptedData, "glyphs.pbf");

    test.observer.glyphsError = [&] (const std::string& fontStack, const GlyphRange& glyphRange, std::exception_ptr error) {
        ASSERT_TRUE(error != nullptr);
        ASSERT_EQ(fontStack, "Test Stack");
        ASSERT_EQ(glyphRange, GlyphRange(0, 255));

        auto stack = test.glyphStore.getFontStack("Test Stack");
        ASSERT_TRUE(stack->getSDFs().empty());
        ASSERT_FALSE(test.glyphStore.hasGlyphRanges("Test Stack", {{0, 255}}));

        test.end();
    };

    test.run(
        "test/fixtures/resources/glyphs.pbf",
        "Test Stack",
        {{0, 255}});
}

TEST(GlyphStore, LoadingCancel) {
    GlyphStoreTest test(MockFileSource::Success, "glyphs.pbf");

    test.observer.glyphsLoaded = [&] (const std::string&, const GlyphRange&) {
        FAIL() << "Should never be called";
    };

    test.fileSource.requestEnqueuedCallback = [&]{
        test.end();
    };

    test.run(
        "test/fixtures/resources/glyphs.pbf",
        "Test Stack",
        {{0, 255}});
}

TEST(GlyphStore, InvalidURL) {
    GlyphStoreTest test(MockFileSource::Success, "");

    test.observer.glyphsError = [&] (const std::string&, const GlyphRange&, std::exception_ptr error) {
        ASSERT_TRUE(error != nullptr);

        auto stack = test.glyphStore.getFontStack("Test Stack");
        ASSERT_TRUE(stack->getSDFs().empty());

        test.end();
    };

    test.run(
        "foo bar",
        "Test Stack",
        {{0, 255}});
}
