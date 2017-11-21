#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>

#include <mbgl/text/glyph_manager.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/i18n.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/logging.hpp>

using namespace mbgl;

// Alpha channel rendering of '中'
static constexpr const uint8_t stubBitmap[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 95, 82, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45, 255, 233, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 37, 255, 233, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 36, 255, 227, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 22, 18, 18, 18, 18, 18, 15, 55, 255, 255, 42, 14, 18, 18, 18, 18, 18, 22, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 145, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 167, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 149, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 178, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 90, 255, 120, 8, 15, 15, 15, 11, 51, 255, 233, 32, 11, 15, 15, 15, 8, 93, 255, 178, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 84, 255, 100, 0, 0, 0, 0, 0, 37, 255, 233, 17, 0, 0, 0, 0, 0, 84, 255, 178, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 84, 255, 107, 0, 0, 0, 0, 0, 37, 255, 233, 17, 0, 0, 0, 0, 0, 84, 255, 178, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 84, 255, 107, 0, 0, 0, 0, 0, 37, 255, 233, 17, 0, 0, 0, 0, 0, 84, 255, 178, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 84, 255, 100, 0, 0, 0, 0, 0, 32, 255, 227, 10, 0, 0, 0, 0, 0, 78, 255, 178, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 84, 255, 170, 101, 101, 101, 101, 101, 129, 255, 255, 133, 100, 101, 101, 101, 96, 191, 255, 178, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 105, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 178, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1, 156, 255, 172, 110, 116, 116, 116, 115, 140, 255, 242, 127, 116, 116, 116, 116, 110, 163, 255, 169, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 145, 255, 108, 0, 0, 0, 0, 0, 32, 255, 227, 10, 0, 0, 0, 0, 0, 89, 255, 194, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 27, 16, 0, 0, 0, 0, 0, 37, 255, 233, 17, 0, 0, 0, 0, 0, 14, 27, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 37, 255, 233, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 37, 255, 233, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 37, 255, 233, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 37, 255, 233, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 37, 255, 233, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 73, 205, 167, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
// SDF transformation of '中'
static constexpr const uint8_t sdfBitmap[] = {0, 0, 0, 0, 0, 0, 0, 0, 19, 48, 76, 101, 119, 127, 127, 127, 126, 118, 100, 75, 48, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 19, 28, 31, 31, 31, 31, 59, 90, 119, 145, 158, 159, 159, 156, 144, 118, 89, 59, 31, 31, 31, 31, 28, 19, 5, 0, 0, 0, 10, 31, 48, 59, 63, 63, 63, 63, 63, 95, 127, 158, 182, 187, 186, 176, 156, 126, 94, 63, 63, 63, 63, 63, 59, 48, 31, 10, 0, 5, 31, 55, 75, 89, 94, 95, 95, 95, 95, 95, 127, 158, 181, 223, 204, 177, 157, 126, 95, 95, 95, 95, 95, 95, 89, 75, 55, 31, 10, 19, 48, 75, 100, 118, 126, 126, 126, 126, 126, 126, 127, 157, 180, 223, 204, 177, 157, 126, 126, 126, 126, 126, 126, 126, 119, 100, 75, 55, 31, 28, 59, 89, 118, 144, 156, 157, 157, 157, 157, 157, 157, 157, 180, 223, 204, 177, 156, 157, 157, 157, 157, 157, 157, 156, 144, 119, 100, 75, 48, 31, 63, 94, 126, 156, 176, 178, 178, 178, 178, 178, 178, 177, 182, 223, 223, 181, 177, 178, 178, 178, 178, 178, 178, 177, 156, 144, 118, 89, 59, 32, 64, 96, 128, 159, 193, 223, 223, 223, 223, 223, 223, 223, 223, 236, 236, 223, 223, 223, 223, 223, 223, 223, 223, 196, 176, 156, 126, 94, 63, 32, 64, 96, 128, 159, 194, 223, 223, 223, 223, 223, 223, 223, 223, 236, 226, 223, 223, 223, 223, 223, 223, 223, 224, 198, 176, 156, 126, 94, 63, 32, 64, 96, 127, 159, 187, 223, 190, 176, 177, 177, 177, 177, 182, 223, 204, 179, 177, 177, 177, 177, 176, 187, 223, 198, 176, 156, 126, 94, 63, 32, 64, 95, 127, 159, 186, 223, 188, 159, 156, 156, 156, 157, 180, 223, 204, 177, 157, 156, 156, 156, 159, 186, 223, 198, 176, 156, 126, 94, 63, 32, 64, 95, 127, 159, 186, 223, 189, 159, 127, 126, 127, 157, 180, 223, 204, 177, 157, 126, 126, 127, 159, 186, 223, 198, 176, 156, 126, 94, 63, 32, 64, 95, 127, 159, 186, 223, 189, 159, 127, 127, 127, 157, 180, 223, 204, 177, 157, 127, 127, 127, 159, 186, 223, 198, 176, 156, 126, 94, 63, 32, 64, 95, 127, 159, 186, 223, 188, 159, 159, 159, 159, 159, 179, 223, 204, 177, 159, 159, 159, 159, 159, 185, 223, 198, 176, 156, 126, 94, 63, 48, 75, 100, 127, 159, 186, 223, 197, 188, 188, 188, 188, 188, 191, 223, 223, 192, 188, 188, 188, 188, 187, 199, 224, 198, 176, 156, 126, 94, 63, 59, 89, 118, 143, 159, 188, 223, 224, 223, 223, 223, 223, 223, 223, 236, 226, 223, 223, 223, 223, 223, 223, 223, 224, 198, 176, 156, 126, 94, 63, 63, 94, 126, 156, 175, 195, 223, 197, 189, 190, 190, 190, 190, 193, 223, 206, 191, 190, 190, 190, 190, 189, 196, 223, 196, 176, 156, 126, 94, 63, 59, 89, 118, 143, 159, 193, 223, 189, 159, 159, 159, 159, 159, 179, 223, 204, 177, 159, 159, 159, 159, 159, 186, 223, 200, 176, 156, 126, 94, 63, 48, 75, 100, 126, 156, 176, 179, 177, 156, 127, 127, 127, 157, 180, 223, 204, 177, 157, 127, 127, 127, 156, 177, 179, 178, 157, 144, 118, 89, 59, 31, 59, 89, 118, 144, 156, 157, 156, 144, 119, 96, 127, 157, 180, 223, 204, 177, 157, 126, 96, 119, 144, 156, 157, 157, 144, 119, 100, 75, 48, 19, 48, 75, 100, 118, 126, 126, 126, 119, 100, 95, 127, 157, 180, 223, 204, 177, 157, 126, 95, 100, 119, 126, 126, 126, 119, 100, 76, 55, 31, 5, 31, 55, 75, 89, 94, 95, 95, 89, 75, 95, 127, 157, 180, 223, 204, 177, 157, 126, 95, 75, 89, 95, 95, 95, 90, 76, 55, 31, 10, 0, 10, 31, 48, 59, 63, 63, 63, 59, 63, 95, 127, 157, 180, 223, 204, 177, 157, 126, 95, 63, 59, 63, 63, 63, 59, 48, 31, 10, 0, 0, 0, 5, 19, 28, 31, 31, 31, 31, 63, 95, 127, 157, 180, 223, 204, 177, 157, 126, 95, 63, 31, 31, 31, 31, 28, 19, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 64, 95, 127, 159, 184, 201, 196, 176, 156, 126, 94, 63, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 60, 90, 120, 146, 159, 159, 159, 156, 144, 118, 89, 59, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 49, 76, 101, 120, 127, 128, 128, 126, 118, 100, 75, 48, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 32, 56, 76, 90, 95, 96, 96, 94, 89, 75, 55, 31, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 32, 49, 60, 64, 64, 64, 63, 59, 48, 31, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 19, 29, 32, 32, 32, 31, 28, 19, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static constexpr const size_t stubBitmapLength = 900;

class StubLocalGlyphRasterizer : public LocalGlyphRasterizer {
public:
    bool canRasterizeGlyph(const FontStack&, GlyphID glyphID) {
        return util::i18n::allowsIdeographicBreaking(glyphID);
    }

    Glyph rasterizeGlyph(const FontStack&, GlyphID glyphID) {
        Glyph stub;
        stub.id = glyphID;
        
        stub.metrics.width = 24;
        stub.metrics.height = 24;
        stub.metrics.left = 0;
        stub.metrics.top = -8;
        stub.metrics.advance = 24;

        stub.bitmap = AlphaImage(Size(30, 30), stubBitmap, stubBitmapLength);

        return stub;
    }
};

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
    GlyphManager glyphManager{ fileSource, std::make_unique<StubLocalGlyphRasterizer>() };

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

TEST(GlyphManager, LoadLocalCJKGlyph) {
    GlyphManagerTest test;
    int glyphResponses = 0;

    test.fileSource.glyphsResponse = [&] (const Resource&) {
        glyphResponses++;
        return optional<Response>();
    };

    test.observer.glyphsLoaded = [&] (const FontStack&, const GlyphRange&) {
        glyphResponses++;
    };
    
    test.requestor.glyphsAvailable = [&] (GlyphMap glyphs) {
        EXPECT_EQ(glyphResponses, 0); // Local generation should prevent requesting any glyphs
        
        const auto& testPositions = glyphs.at({{"Test Stack"}});

        ASSERT_EQ(testPositions.size(), 1u);
        ASSERT_EQ(testPositions.count(u'中'), 1u);

        Immutable<Glyph> glyph = *testPositions.at(u'中');
        EXPECT_EQ(glyph->id, u'中');
        EXPECT_EQ(glyph->metrics.width, 24ul);
        EXPECT_EQ(glyph->metrics.height, 24ul);
        EXPECT_EQ(glyph->metrics.left, 0);
        EXPECT_EQ(glyph->metrics.top, -8);
        EXPECT_EQ(glyph->metrics.advance, 24ul);
        EXPECT_EQ(glyph->bitmap.size, Size(30, 30));

        size_t pixelCount = glyph->bitmap.size.width * glyph->bitmap.size.height;
        for (size_t i = 0; i < pixelCount; i++) {
            EXPECT_EQ(glyph->bitmap.data[i], sdfBitmap[i]);
        }

        test.end();
    };

    test.run(
        "test/fixtures/resources/glyphs.pbf",
        GlyphDependencies {
            {{{"Test Stack"}}, {u'中'}}
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
