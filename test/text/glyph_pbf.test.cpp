#include <mbgl/test/util.hpp>

#include <mbgl/text/glyph_set.hpp>
#include <mbgl/text/glyph_atlas_observer.hpp>
#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/text/glyph_pbf.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>

#include <future>

using namespace mbgl;

using namespace std::string_literals;

class MockGlyphAtlasObserver : public GlyphAtlasObserver {
public:
    std::function<void(const FontStack&, const GlyphRange&)> glyphsLoaded;
    std::function<void(const FontStack&, const GlyphRange&, std::exception_ptr)> glyphsError;

    void onGlyphsLoaded(const FontStack& fontStack, const GlyphRange& glyphRange) override {
        if (glyphsLoaded) {
            glyphsLoaded(fontStack, glyphRange);
        }
    }
    void onGlyphsError(const FontStack& fontStack, const GlyphRange& glyphRange, std::exception_ptr error) override {
        if (glyphsError) {
            glyphsError(fontStack, glyphRange, error);
        }
    }
};

TEST(GlyphPBF, Parsing) {
    util::RunLoop loop;
    DefaultFileSource fileSource{ ":memory:", "." };
    GlyphAtlas glyphAtlas{ { 1024, 1024 }, fileSource };
    FontStack fontStack{ "fake_glyphs" };
    GlyphRange glyphRange{ 0, 255 };

    glyphAtlas.setURL("asset://test/fixtures/resources/{fontstack}-{range}.pbf");

    MockGlyphAtlasObserver glyphAtlasObserver;
    glyphAtlasObserver.glyphsLoaded = [&](const FontStack&, const GlyphRange&) {
        loop.stop();

        auto sdfs = glyphAtlas.getGlyphSet(fontStack)->getSDFs();

        // The fake glyphs don't contain a glyph that has the ID 0; it only contains glyphs with
        // undefined IDs, but the parser should remove them.

        EXPECT_TRUE(sdfs.size() == 1);
        EXPECT_TRUE(sdfs.find(69) != sdfs.end());
        auto& sdf = sdfs[69];
        EXPECT_EQ("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"s, sdf.bitmap);
        EXPECT_EQ(1u, sdf.metrics.width);
        EXPECT_EQ(1u, sdf.metrics.height);
        EXPECT_EQ(20, sdf.metrics.left);
        EXPECT_EQ(2, sdf.metrics.top);
        EXPECT_EQ(8u, sdf.metrics.advance);
    };

    glyphAtlasObserver.glyphsError = [&](const FontStack&, const GlyphRange&, std::exception_ptr error) {
        loop.stop();
        FAIL() << util::toString(error);
    };

    GlyphPBF pbf(&glyphAtlas, fontStack, glyphRange, &glyphAtlasObserver, fileSource);

    loop.run();
}
