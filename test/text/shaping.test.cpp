
#include <mbgl/test/util.hpp>

#include <mbgl/text/bidi.hpp>
#include <mbgl/text/tagged_string.hpp>
#include <mbgl/text/shaping.hpp>
#include <mbgl/util/constants.hpp>

#include <vector>

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

    BiDi bidi;
    auto immutableGlyph = Immutable<Glyph>(makeMutable<Glyph>(std::move(glyph)));
    const std::vector<std::string> fontStack{{"font-stack"}};
    const SectionOptions sectionOptions(1.0f, fontStack);
    Glyphs glyphData;
    glyphData.glyphs.emplace(u'中', std::move(immutableGlyph));
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
        ASSERT_FLOAT_EQ(-36.0f, shaping.top);
        ASSERT_FLOAT_EQ(36.0f, shaping.bottom);
        ASSERT_FLOAT_EQ(-63.0f, shaping.left);
        ASSERT_FLOAT_EQ(63.0f, shaping.right);
        ASSERT_EQ(shaping.writingMode, WritingModeType::Horizontal);
        ASSERT_EQ(shaping.lineCount, shaping.positionedGlyphs.size());
        ASSERT_FALSE(shaping.hasBaseline);
    }

    // 2 lines
    // 中中
    // 中
    {
        TaggedString string(u"中中\u200b中", sectionOptions);
        auto shaping = testGetShaping(string, 1);
        ASSERT_EQ(shaping.lineCount, 2);
        ASSERT_FLOAT_EQ(-24.0f, shaping.top);
        ASSERT_FLOAT_EQ(24.0f, shaping.bottom);
        ASSERT_FLOAT_EQ(-21.0f, shaping.left);
        ASSERT_FLOAT_EQ(21.0f, shaping.right);
        ASSERT_EQ(shaping.writingMode, WritingModeType::Horizontal);
        ASSERT_EQ(shaping.lineCount, shaping.positionedGlyphs.size());
        ASSERT_FALSE(shaping.hasBaseline);
    }

    // 1 line
    // 中中
    {
        TaggedString string(u"中中\u200b", sectionOptions);
        auto shaping = testGetShaping(string, 2);
        ASSERT_EQ(shaping.lineCount, 1);
        ASSERT_FLOAT_EQ(-12.0f, shaping.top);
        ASSERT_FLOAT_EQ(12.0f, shaping.bottom);
        ASSERT_FLOAT_EQ(-21.0f, shaping.left);
        ASSERT_FLOAT_EQ(21.0f, shaping.right);
        ASSERT_EQ(shaping.writingMode, WritingModeType::Horizontal);
        ASSERT_EQ(shaping.lineCount, shaping.positionedGlyphs.size());
        ASSERT_EQ(1, shaping.positionedGlyphs.count(0));
        ASSERT_EQ(2, shaping.positionedGlyphs[0].size());
        const auto& glyphs = shaping.positionedGlyphs[0];
        ASSERT_FLOAT_EQ(-21.0f, glyphs[0].x);
        ASSERT_FLOAT_EQ(-17.0f, glyphs[0].y);
        ASSERT_FLOAT_EQ(0.0f, glyphs[1].x);
        ASSERT_FLOAT_EQ(-17.0f, glyphs[1].y);
        ASSERT_FALSE(shaping.hasBaseline);
    }

    // 5 'new' lines.
    {
        TaggedString string(u"\u200b\u200b\u200b\u200b\u200b", sectionOptions);
        auto shaping = testGetShaping(string, 1);
        ASSERT_EQ(shaping.lineCount, 5);
        ASSERT_FLOAT_EQ(-60.0f, shaping.top);
        ASSERT_FLOAT_EQ(60.0f, shaping.bottom);
        ASSERT_FLOAT_EQ(-0.0f, shaping.left);
        ASSERT_FLOAT_EQ(0.0f, shaping.right);
        ASSERT_EQ(shaping.writingMode, WritingModeType::Horizontal);
        ASSERT_EQ(shaping.lineCount, shaping.positionedGlyphs.size());
        ASSERT_FALSE(shaping.hasBaseline);
    }
}

TEST(Shaping, MixedFontsBothWithBaselines) {
    Glyph glyph1;
    glyph1.id = u'阳';
    glyph1.metrics.width = 18;
    glyph1.metrics.height = 19;
    glyph1.metrics.left = 2;
    glyph1.metrics.top = -8;
    glyph1.metrics.advance = 21;

    Glyph glyph2;
    glyph2.id = u'光';
    glyph2.metrics.width = 18;
    glyph2.metrics.height = 18;
    glyph2.metrics.left = 2;
    glyph2.metrics.top = -8;
    glyph2.metrics.advance = 21;

    BiDi bidi;
    std::vector<SectionOptions> sectionOptions;
    const std::vector<std::string> fontStack1{{"font-stack1"}};
    sectionOptions.emplace_back(1.0f, fontStack1);
    Glyphs glyphData1;
    glyphData1.glyphs.emplace(u'阳', Immutable<Glyph>(makeMutable<Glyph>(std::move(glyph1))));
    glyphData1.ascender = 26;
    glyphData1.descender = -6;

    const std::vector<std::string> fontStack2{{"font-stack2"}};
    sectionOptions.emplace_back(1.0f, fontStack2);
    Glyphs glyphData2;
    glyphData2.glyphs.emplace(u'光', Immutable<Glyph>(makeMutable<Glyph>(std::move(glyph2))));
    glyphData2.ascender = 25;
    glyphData2.descender = -5;

    GlyphMap glyphs;
    glyphs.emplace(FontStackHasher()(fontStack1), std::move(glyphData1));
    glyphs.emplace(FontStackHasher()(fontStack2), std::move(glyphData2));

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
        std::u16string text{u"阳光\u200b"};
        StyledText styledText;
        styledText.second = std::vector<uint8_t>{0, 1, 0};
        styledText.first = std::move(text);

        TaggedString string{styledText, sectionOptions};

        auto shaping = testGetShaping(string, 5);
        ASSERT_EQ(shaping.lineCount, 1);
        ASSERT_FLOAT_EQ(-12.0f, shaping.top);
        ASSERT_FLOAT_EQ(12.0f, shaping.bottom);
        ASSERT_FLOAT_EQ(-21.0f, shaping.left);
        ASSERT_FLOAT_EQ(21.0f, shaping.right);
        ASSERT_EQ(shaping.writingMode, WritingModeType::Horizontal);
        ASSERT_EQ(shaping.lineCount, shaping.positionedGlyphs.size());
        ASSERT_EQ(1, shaping.positionedGlyphs.count(0));
        ASSERT_EQ(2, shaping.positionedGlyphs[0].size());
        const auto& glyphs = shaping.positionedGlyphs[0];
        ASSERT_FLOAT_EQ(-21.0f, glyphs[0].x);
        ASSERT_FLOAT_EQ(-16.0f, glyphs[0].y);
        ASSERT_FLOAT_EQ(0.0f, glyphs[1].x);
        ASSERT_FLOAT_EQ(-15.0f, glyphs[1].y);
        ASSERT_TRUE(shaping.hasBaseline);
    }
}

TEST(Shaping, MixedFontsOneWithBaselineOneWithout) {
    Glyph glyph1;
    glyph1.id = u'阳';
    glyph1.metrics.width = 18;
    glyph1.metrics.height = 19;
    glyph1.metrics.left = 2;
    glyph1.metrics.top = -8;
    glyph1.metrics.advance = 21;

    Glyph glyph2;
    glyph2.id = u'光';
    glyph2.metrics.width = 18;
    glyph2.metrics.height = 18;
    glyph2.metrics.left = 2;
    glyph2.metrics.top = -8;
    glyph2.metrics.advance = 21;

    BiDi bidi;
    std::vector<SectionOptions> sectionOptions;
    const std::vector<std::string> fontStack1{{"font-stack1"}};
    sectionOptions.emplace_back(1.0f, fontStack1);
    Glyphs glyphData1;
    glyphData1.glyphs.emplace(u'阳', Immutable<Glyph>(makeMutable<Glyph>(std::move(glyph1))));
    glyphData1.ascender = 26;
    glyphData1.descender = -6;

    const std::vector<std::string> fontStack2{{"font-stack2"}};
    sectionOptions.emplace_back(1.0f, fontStack2);
    Glyphs glyphData2;
    glyphData2.glyphs.emplace(u'光', Immutable<Glyph>(makeMutable<Glyph>(std::move(glyph2))));

    GlyphMap glyphs;
    glyphs.emplace(FontStackHasher()(fontStack1), std::move(glyphData1));
    glyphs.emplace(FontStackHasher()(fontStack2), std::move(glyphData2));

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
        std::u16string text{u"阳光\u200b"};
        StyledText styledText;
        styledText.second = std::vector<uint8_t>{0, 1, 0};
        styledText.first = std::move(text);

        TaggedString string{styledText, sectionOptions};

        auto shaping = testGetShaping(string, 5);
        ASSERT_EQ(shaping.lineCount, 1);
        ASSERT_FLOAT_EQ(-12.0f, shaping.top);
        ASSERT_FLOAT_EQ(12.0f, shaping.bottom);
        ASSERT_FLOAT_EQ(-21.0f, shaping.left);
        ASSERT_FLOAT_EQ(21.0f, shaping.right);
        ASSERT_EQ(shaping.writingMode, WritingModeType::Horizontal);
        ASSERT_EQ(shaping.lineCount, shaping.positionedGlyphs.size());
        ASSERT_EQ(1, shaping.positionedGlyphs.count(0));
        ASSERT_EQ(2, shaping.positionedGlyphs[0].size());
        const auto& glyphs = shaping.positionedGlyphs[0];
        ASSERT_FLOAT_EQ(-21.0f, glyphs[0].x);
        ASSERT_FLOAT_EQ(-17.0f, glyphs[0].y);
        ASSERT_FLOAT_EQ(0.0f, glyphs[1].x);
        ASSERT_FLOAT_EQ(-17.0f, glyphs[1].y);
        ASSERT_FALSE(shaping.hasBaseline);
    }
}

TEST(Shaping, MixedFontsWithBaselineWithFontScale) {
    Glyph glyph1;
    glyph1.id = u'阳';
    glyph1.metrics.width = 18;
    glyph1.metrics.height = 19;
    glyph1.metrics.left = 2;
    glyph1.metrics.top = -8;
    glyph1.metrics.advance = 21;

    Glyph glyph2;
    glyph2.id = u'光';
    glyph2.metrics.width = 18;
    glyph2.metrics.height = 18;
    glyph2.metrics.left = 2;
    glyph2.metrics.top = -8;
    glyph2.metrics.advance = 21;

    BiDi bidi;
    std::vector<SectionOptions> sectionOptions;
    const std::vector<std::string> fontStack1{{"font-stack1"}};
    sectionOptions.emplace_back(1.0f, fontStack1);
    sectionOptions.back().scale = 1.5;
    Glyphs glyphData1;
    glyphData1.glyphs.emplace(u'阳', Immutable<Glyph>(makeMutable<Glyph>(std::move(glyph1))));
    glyphData1.ascender = 26;
    glyphData1.descender = -6;

    const std::vector<std::string> fontStack2{{"font-stack2"}};
    sectionOptions.emplace_back(1.0f, fontStack2);
    Glyphs glyphData2;
    glyphData2.glyphs.emplace(u'光', Immutable<Glyph>(makeMutable<Glyph>(std::move(glyph2))));
    glyphData2.ascender = 25;
    glyphData2.descender = -5;

    GlyphMap glyphs;
    glyphs.emplace(FontStackHasher()(fontStack1), std::move(glyphData1));
    glyphs.emplace(FontStackHasher()(fontStack2), std::move(glyphData2));

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
        std::u16string text{u"阳光\u200b"};
        StyledText styledText;
        styledText.second = std::vector<uint8_t>{0, 1, 0};
        styledText.first = std::move(text);

        TaggedString string{styledText, sectionOptions};

        auto shaping = testGetShaping(string, 5);
        ASSERT_EQ(shaping.lineCount, 1);
        ASSERT_FLOAT_EQ(-18.0f, shaping.top);
        ASSERT_FLOAT_EQ(18.0f, shaping.bottom);
        ASSERT_FLOAT_EQ(-26.25f, shaping.left);
        ASSERT_FLOAT_EQ(26.25f, shaping.right);
        ASSERT_EQ(shaping.writingMode, WritingModeType::Horizontal);
        ASSERT_EQ(shaping.lineCount, shaping.positionedGlyphs.size());
        ASSERT_EQ(1, shaping.positionedGlyphs.count(0));
        ASSERT_EQ(2, shaping.positionedGlyphs[0].size());
        const auto& glyphs = shaping.positionedGlyphs[0];
        ASSERT_FLOAT_EQ(-26.25f, glyphs[0].x);
        ASSERT_FLOAT_EQ(-24.0f, glyphs[0].y);
        ASSERT_FLOAT_EQ(5.25f, glyphs[1].x);
        ASSERT_FLOAT_EQ(-10.0f, glyphs[1].y);
        ASSERT_TRUE(shaping.hasBaseline);
    }
}
