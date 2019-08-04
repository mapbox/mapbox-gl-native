
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

    BiDi bidi;
    auto immutableGlyph = Immutable<Glyph>(makeMutable<Glyph>(std::move(glyph)));
    const std::vector<std::string> fontStack{{"font-stack"}};
    const SectionOptions sectionOptions(1.0f, fontStack);
    GlyphMap glyphs = {
        { FontStackHasher()(fontStack), {{u'中', std::move(immutableGlyph)}} }
    };

    const auto testGetShaping = [&] (const TaggedString& string, unsigned maxWidthInChars) {
        return getShaping(string,
                          maxWidthInChars * ONE_EM,
                          ONE_EM,       // lineHeight
                          style::SymbolAnchorType::Center,
                          style::TextJustifyType::Center,
                          0,            // spacing
                          {0.0f, 0.0f}, // translate
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
