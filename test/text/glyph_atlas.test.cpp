#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>
#include <mbgl/test/stub_style_observer.hpp>

#include <mbgl/text/glyph_set.hpp>
#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/logging.hpp>

using namespace mbgl;

class GlyphAtlasTest {
public:
    util::RunLoop loop;
    StubFileSource fileSource;
    StubStyleObserver observer;
    GlyphAtlas glyphAtlas{ { 32, 32 }, fileSource };

    void run(const std::string& url, const FontStack& fontStack, const GlyphRangeSet& glyphRanges) {
        // Squelch logging.
        Log::setObserver(std::make_unique<Log::NullObserver>());

        glyphAtlas.setObserver(&observer);
        glyphAtlas.setURL(url);
        glyphAtlas.hasGlyphRanges(fontStack, glyphRanges);

        loop.run();
    }

    void end() {
        loop.stop();
    }
};

TEST(GlyphAtlas, LoadingSuccess) {
    GlyphAtlasTest test;

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

    test.observer.glyphsLoaded = [&] (const FontStack&, const GlyphRange&) {
        if (!test.glyphAtlas.hasGlyphRanges({{"Test Stack"}}, {{0, 255}, {256, 511}}))
            return;

        auto glyphSet = test.glyphAtlas.getGlyphSet({{"Test Stack"}});
        ASSERT_FALSE(glyphSet->getSDFs().empty());

        test.end();
    };

    test.run(
        "test/fixtures/resources/glyphs.pbf",
        {{"Test Stack"}},
        {{0, 255}, {256, 511}});
}

TEST(GlyphAtlas, LoadingFail) {
    GlyphAtlasTest test;

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

        auto glyphSet = test.glyphAtlas.getGlyphSet({{"Test Stack"}});
        ASSERT_TRUE(glyphSet->getSDFs().empty());
        ASSERT_FALSE(test.glyphAtlas.hasGlyphRanges({{"Test Stack"}}, {{0, 255}}));

        test.end();
    };

    test.run(
        "test/fixtures/resources/glyphs.pbf",
        {{"Test Stack"}},
        {{0, 255}});
}

TEST(GlyphAtlas, LoadingCorrupted) {
    GlyphAtlasTest test;

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

        auto glyphSet = test.glyphAtlas.getGlyphSet({{"Test Stack"}});
        ASSERT_TRUE(glyphSet->getSDFs().empty());
        ASSERT_FALSE(test.glyphAtlas.hasGlyphRanges({{"Test Stack"}}, {{0, 255}}));

        test.end();
    };

    test.run(
        "test/fixtures/resources/glyphs.pbf",
        {{"Test Stack"}},
        {{0, 255}});
}

TEST(GlyphAtlas, LoadingCancel) {
    GlyphAtlasTest test;

    test.fileSource.glyphsResponse = [&] (const Resource&) {
        test.end();
        return optional<Response>();
    };

    test.observer.glyphsLoaded = [&] (const FontStack&, const GlyphRange&) {
        FAIL() << "Should never be called";
    };

    test.run(
        "test/fixtures/resources/glyphs.pbf",
        {{"Test Stack"}},
        {{0, 255}});
}

TEST(GlyphAtlas, InvalidSDFGlyph) {
    GlyphSet glyphSet;
    glyphSet.insert(65, SDFGlyph{ 65 /* ASCII 'A' */,
                                  "x" /* bitmap is too short */,
                                  { 1 /* width */, 1 /* height */, 0 /* left */, 0 /* top */,
                                    0 /* advance */ } });
    glyphSet.insert(66, SDFGlyph{ 66 /* ASCII 'B' */,
                                  std::string(7 * 7, 'x'), /* correct */
                                  { 1 /* width */, 1 /* height */, 0 /* left */, 0 /* top */,
                                    0 /* advance */ } });
    glyphSet.insert(67, SDFGlyph{ 67 /* ASCII 'C' */,
                                  std::string(518 * 8, 'x'), /* correct */
                                  { 512 /* width */, 2 /* height */, 0 /* left */, 0 /* top */,
                                    0 /* advance */ } });


    const FontStack fontStack{ "Mock Font" };

    GlyphAtlasTest test;
    GlyphPositions positions;
    test.glyphAtlas.addGlyphs(1, std::u16string{u"ABC"}, fontStack, glyphSet, positions);

    ASSERT_EQ(3u, positions.size());

    // 'A' was not placed because the bitmap size is invalid.
    ASSERT_NE(positions.end(), positions.find(65));
    ASSERT_EQ((Rect<uint16_t>{ 0, 0, 0, 0 }), positions[65].rect);

    // 'B' was placed at the top left.
    ASSERT_NE(positions.end(), positions.find(66));
    // Width is 12 because actual dimensions are 1+6 pixels, with 1px border added, rounded up to
    // the next multiple of 4.
    ASSERT_EQ((Rect<uint16_t>{ 0, 0, 12, 12 }), positions[66].rect);

    // 'C' was not placed because the width is too big.
    ASSERT_NE(positions.end(), positions.find(67));
    ASSERT_EQ((Rect<uint16_t>{ 0, 0, 0, 0 }), positions[67].rect);

}
