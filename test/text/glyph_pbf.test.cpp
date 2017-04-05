#include <mbgl/test/util.hpp>

#include <mbgl/text/glyph_pbf.hpp>
#include <mbgl/util/io.hpp>

using namespace mbgl;

TEST(GlyphPBF, Parsing) {
    // The fake glyphs contain a number of invalid glyphs, which should be skipped by the parser.
    auto sdfs = parseGlyphPBF(GlyphRange { 0, 255 }, util::read_file("test/fixtures/resources/fake_glyphs-0-255.pbf"));
    EXPECT_TRUE(sdfs.size() == 1);

    auto& sdf = sdfs[0];
    EXPECT_EQ(69u, sdf.id);
    AlphaImage expected({7, 7});
    expected.fill('x');
    EXPECT_EQ(expected, sdf.bitmap);
    EXPECT_EQ(1u, sdf.metrics.width);
    EXPECT_EQ(1u, sdf.metrics.height);
    EXPECT_EQ(20, sdf.metrics.left);
    EXPECT_EQ(2, sdf.metrics.top);
    EXPECT_EQ(8u, sdf.metrics.advance);
}
