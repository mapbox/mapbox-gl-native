
#include <mbgl/test/util.hpp>

#include <mbgl/text/bidi.hpp>
#include <mbgl/text/tagged_string.hpp>
#include <mbgl/text/shaping.hpp>
#include <mbgl/util/constants.hpp>

using namespace mbgl;
using namespace util;

TEST(Shaping, ZWSP) {
    Glyph glyph;
    glyph.id = u'中';
    glyph.metrics.width = 18;
    glyph.metrics.height = 18;
    glyph.metrics.left = 2;
    glyph.metrics.top = -8;
    glyph.metrics.advance = 21;
    glyph.metrics.ascender = 0;
    glyph.metrics.descender = 0;

    BiDi bidi;
    auto immutableGlyph = Immutable<Glyph>(makeMutable<Glyph>(std::move(glyph)));
    const std::vector<std::string> fontStack{{"font-stack"}};
    const SectionOptions sectionOptions(1.0f, fontStack);
    Glyphs glyphData;
    glyphData.glyphs.emplace(u'中', std::move(immutableGlyph));
    glyphData.hasBaseline = false;
    GlyphMap glyphs = {{FontStackHasher()(fontStack), std::move(glyphData)}};

    const auto testGetShaping = [&](const TaggedString& string, unsigned maxWidthInChars) {
        return getShaping(string,
                          maxWidthInChars * ONE_EM,
                          ONE_EM, // lineHeight
                          style::SymbolAnchorType::Center,
                          style::TextJustifyType::Center,
                          0,              // spacing
                          {{0.0f, 0.0f}}, // translate
                          WritingModeType::Horizontal,
                          bidi,
                          glyphs,
                          /*allowVerticalPlacement*/ false);
    };

    // 3 lines
    // 中中中中中中
    // 中中中中中中
    // 中中
    {
        TaggedString string(u"中中\u200b中中\u200b中中\u200b中中中中中中\u200b中中", sectionOptions);
        auto shaping = testGetShaping(string, 5);
        ASSERT_EQ(shaping.lineCount, 3);
        ASSERT_EQ(shaping.top, -36);
        ASSERT_EQ(shaping.bottom, 36);
        ASSERT_EQ(shaping.left, -63);
        ASSERT_EQ(shaping.right, 63);
        ASSERT_EQ(shaping.writingMode, WritingModeType::Horizontal);
    }

    // 2 lines
    // 中中
    // 中
    {
        TaggedString string(u"中中\u200b中", sectionOptions);
        auto shaping = testGetShaping(string, 1);
        ASSERT_EQ(shaping.lineCount, 2);
        ASSERT_EQ(shaping.top, -24);
        ASSERT_EQ(shaping.bottom, 24);
        ASSERT_EQ(shaping.left, -21);
        ASSERT_EQ(shaping.right, 21);
        ASSERT_EQ(shaping.writingMode, WritingModeType::Horizontal);
    }

    // 1 line
    // 中中
    {
        TaggedString string(u"中中\u200b", sectionOptions);
        auto shaping = testGetShaping(string, 2);
        ASSERT_EQ(shaping.lineCount, 1);
        ASSERT_EQ(shaping.top, -12);
        ASSERT_EQ(shaping.bottom, 12);
        ASSERT_EQ(shaping.left, -21);
        ASSERT_EQ(shaping.right, 21);
        ASSERT_EQ(shaping.writingMode, WritingModeType::Horizontal);
        ASSERT_EQ(2, shaping.positionedGlyphs.size());
        EXPECT_FLOAT_EQ(-21, shaping.positionedGlyphs[0].x);
        EXPECT_FLOAT_EQ(-17, shaping.positionedGlyphs[0].y);
        EXPECT_FLOAT_EQ(0, shaping.positionedGlyphs[1].x);
        EXPECT_FLOAT_EQ(-17, shaping.positionedGlyphs[1].y);
    }

    // 5 'new' lines.
    {
        TaggedString string(u"\u200b\u200b\u200b\u200b\u200b", sectionOptions);
        auto shaping = testGetShaping(string, 1);
        ASSERT_EQ(shaping.lineCount, 5);
        ASSERT_EQ(shaping.top, -60);
        ASSERT_EQ(shaping.bottom, 60);
        ASSERT_EQ(shaping.left, 0);
        ASSERT_EQ(shaping.right, 0);
        ASSERT_EQ(shaping.writingMode, WritingModeType::Horizontal);
    }
}

TEST(Shaping, FontWithBaseline) {
    Glyph glyph1;
    glyph1.id = u'阳';
    glyph1.metrics.width = 18;
    glyph1.metrics.height = 19;
    glyph1.metrics.left = 2;
    glyph1.metrics.top = -8;
    glyph1.metrics.advance = 21;
    glyph1.metrics.ascender = 26;
    glyph1.metrics.descender = -6;

    Glyph glyph2;
    glyph2.id = u'光';
    glyph2.metrics.width = 18;
    glyph2.metrics.height = 18;
    glyph2.metrics.left = 2;
    glyph2.metrics.top = -8;
    glyph2.metrics.advance = 21;
    glyph2.metrics.ascender = 25;
    glyph2.metrics.descender = -5;

    BiDi bidi;
    const std::vector<std::string> fontStack{{"font-stack"}};
    const SectionOptions sectionOptions(1.0f, fontStack);
    Glyphs glyphData;
    glyphData.glyphs.emplace(u'阳', Immutable<Glyph>(makeMutable<Glyph>(std::move(glyph1))));
    glyphData.glyphs.emplace(u'光', Immutable<Glyph>(makeMutable<Glyph>(std::move(glyph2))));
    glyphData.hasBaseline = true;
    GlyphMap glyphs = {{FontStackHasher()(fontStack), std::move(glyphData)}};

    const auto testGetShaping = [&](const TaggedString& string, unsigned maxWidthInChars) {
        return getShaping(string,
                          maxWidthInChars * ONE_EM,
                          ONE_EM, // lineHeight
                          style::SymbolAnchorType::Center,
                          style::TextJustifyType::Center,
                          0,              // spacing
                          {{0.0f, 0.0f}}, // translate
                          WritingModeType::Horizontal,
                          bidi,
                          glyphs,
                          /*allowVerticalPlacement*/ false);
    };

    {
        TaggedString string(u"阳光\u200b", sectionOptions);
        auto shaping = testGetShaping(string, 5);
        ASSERT_EQ(shaping.lineCount, 1);
        ASSERT_EQ(shaping.top, -12);
        ASSERT_EQ(shaping.bottom, 12);
        ASSERT_EQ(shaping.left, -21);
        ASSERT_EQ(shaping.right, 21);
        ASSERT_EQ(shaping.writingMode, WritingModeType::Horizontal);
        ASSERT_EQ(2, shaping.positionedGlyphs.size());
        EXPECT_FLOAT_EQ(-21, shaping.positionedGlyphs[0].x);
        EXPECT_FLOAT_EQ(-16, shaping.positionedGlyphs[0].y);
        EXPECT_FLOAT_EQ(0, shaping.positionedGlyphs[1].x);
        EXPECT_FLOAT_EQ(-15, shaping.positionedGlyphs[1].y);
    }
}
