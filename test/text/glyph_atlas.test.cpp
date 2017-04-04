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

class GlyphAtlasTest : public GlyphRequestor {
public:
    util::RunLoop loop;
    StubFileSource fileSource;
    StubStyleObserver observer;
    GlyphAtlas glyphAtlas{ { 32, 32 }, fileSource };
    GlyphPositionMap glyphPositions;
    GlyphRangeSet loadedRanges;

    void run(const std::string& url, const FontStack& fontStack, const GlyphRangeSet& glyphRanges) {
        // Squelch logging.
        Log::setObserver(std::make_unique<Log::NullObserver>());

        glyphAtlas.setObserver(&observer);
        glyphAtlas.setURL(url);
        GlyphDependencies glyphDependencies;
        for (const auto& range : glyphRanges) {
            glyphDependencies[fontStack].insert(range.first);
        }
        glyphAtlas.getGlyphs(*this, glyphDependencies);
        glyphAtlas.hasGlyphRanges(fontStack, glyphRanges);

        loop.run();
    }
    void addGlyphs(GlyphRequestor& requestor, const GlyphDependencies& glyphDependencies) {
        glyphAtlas.addGlyphs(requestor, glyphDependencies);
    }
    
    bool hasGlyphRanges(const FontStack& fontStack, const GlyphRangeSet& ranges) const {
        return glyphAtlas.hasGlyphRanges(fontStack, ranges);
    }

    void end() {
        loop.stop();
    }
    
    virtual void onGlyphsAvailable(GlyphPositionMap positions, GlyphRangeSet _loadedRanges) {
        glyphPositions = std::move(positions);
        loadedRanges = std::move(_loadedRanges);
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
        if (!test.hasGlyphRanges({{"Test Stack"}}, {{0, 255}, {256, 511}}))
            return;

        auto& glyphSet = test.glyphAtlas.getGlyphSet({{"Test Stack"}});
        ASSERT_FALSE(glyphSet.getSDFs().empty());

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

        ASSERT_TRUE(test.glyphAtlas.getGlyphSet({{"Test Stack"}}).getSDFs().empty());
        ASSERT_FALSE(test.hasGlyphRanges({{"Test Stack"}}, {{0, 255}}));

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

        ASSERT_TRUE(test.glyphAtlas.getGlyphSet({{"Test Stack"}}).getSDFs().empty());
        ASSERT_FALSE(test.hasGlyphRanges({{"Test Stack"}}, {{0, 255}}));

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
    const FontStack fontStack{ "Mock Font" };

    GlyphAtlasTest test;

    auto& glyphSet = test.glyphAtlas.getGlyphSet(fontStack);
    glyphSet.insert(66, SDFGlyph{ 66 /* ASCII 'B' */,
                                  AlphaImage({7, 7}), /* correct */
                                  { 1 /* width */, 1 /* height */, 0 /* left */, 0 /* top */,
                                    0 /* advance */ } });
    glyphSet.insert(67, SDFGlyph{ 67 /* ASCII 'C' */,
                                  AlphaImage({518, 8}), /* correct */
                                  { 512 /* width */, 2 /* height */, 0 /* left */, 0 /* top */,
                                    0 /* advance */ } });

    GlyphDependencies glyphDependencies = {{fontStack, {'A','B','C'}}};
    test.addGlyphs(test, glyphDependencies);
    GlyphPositions positions = test.glyphPositions[fontStack];

    ASSERT_EQ(2u, positions.size());

    // 'A' was not placed because not in the glyph set.
    ASSERT_EQ(positions.end(), positions.find(65));

    // 'B' was placed at the top left.
    ASSERT_NE(positions.end(), positions.find(66));
    // Width is 12 because actual dimensions are 1+6 pixels, with 1px border added, rounded up to
    // the next multiple of 4.
    ASSERT_EQ((Rect<uint16_t>{ 0, 0, 12, 12 }), positions[66].rect);

    // 'C' was not placed because the width is too big.
    ASSERT_NE(positions.end(), positions.find(67));
    ASSERT_EQ((Rect<uint16_t>{ 0, 0, 0, 0 }), positions[67].rect);

}
