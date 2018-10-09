
#include <mbgl/test/util.hpp>

#include <mbgl/text/bidi.hpp>

using namespace mbgl;

/*
 These tests mirror the unit tests in mapbox-gl-rtl-text's arabic.test.js

 The arabic text in results may appear to be backwards
 This is because whatever you're viewing the text with is
 applying the bidirectional algorithm a second time.

 Although they may look the same as input in your viewer, the
 characters in the test results are "presentation forms" of
 the characters.
 
 To closely inspect the inputs and outputs, use a binary/hex editor.
*/

TEST(BiDi, ArabicShaping) {
    EXPECT_EQ(applyArabicShaping(u"اليَمَن‎‎"), u"ﺍﻟﻴﹷﻤﹷﻦ‎‎");
}

TEST(BiDi, Tashkeel) {
    EXPECT_EQ(applyArabicShaping(u"سلام۳۹"), u"ﺳﻼﻡ۳۹");
}

TEST(BiDi, MixedShaping) {
    EXPECT_EQ(applyArabicShaping(u"مكتبة الإسكندرية‎‎ Maktabat al-Iskandarīyah"),
              u"ﻣﻜﺘﺒﺔ ﺍﻹﺳﻜﻨﺪﺭﻳﺔ‎‎ Maktabat al-Iskandarīyah");
}

TEST(BiDi, ReverseArabic) {
    BiDi bidi;
    EXPECT_EQ(bidi.processText(applyArabicShaping(u"سلام۳۹"), {}),
              std::vector<std::u16string>{ u"۳۹ﻡﻼﺳ" });
}

TEST(BiDi, ReverseMixed) {
    BiDi bidi;
    EXPECT_EQ(bidi.processText(applyArabicShaping(u"مكتبة الإسكندرية‎‎ Maktabat al-Iskandarīyah"), {}),
              std::vector<std::u16string>{ u" Maktabat al-Iskandarīyahﺔﻳﺭﺪﻨﻜﺳﻹﺍ ﺔﺒﺘﻜﻣ" });
}

TEST(BiDi, WithLineBreaks) {
    BiDi bidi;
    std::vector<std::u16string> expected;
    expected.emplace_back(u" ﺔﻳﺭﺪﻨﻜﺳﻹﺍ ﺔﺒﺘﻜﻣ");
    expected.emplace_back(u"Maktabat al-");
    expected.emplace_back(u"Iskandarīyah");
    EXPECT_EQ(bidi.processText(applyArabicShaping(u"مكتبة الإسكندرية‎‎ Maktabat al-Iskandarīyah"), { 18, 30 }),
              expected);
}

TEST(BiDi, StyledText) {
    // This test uses line breaks that intentionally split/reorder/interleave styled blocks
    // that are contiguous in the input
    BiDi bidi;
    std::vector<StyledText> expected;
    StyledText input(
        applyArabicShaping(u"مكتبة الإسكندرية‎‎ Maktabat al-Iskandarīyah"),
        std::vector<uint8_t>{ 0,0,0,0,0,1,1,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,4,5,5,5,5,6,6,6,6,6,6,6,6,6,6,7,7,7 }
    );

    expected.emplace_back(StyledText(
        u"ﺔﺒﺘﻜﻣ",
        std::vector<uint8_t>{ 0,0,0,0,0 }
    ));
    EXPECT_EQ(expected.rbegin()->first.size(), expected.rbegin()->second.size());
    expected.emplace_back(StyledText(
        u" ‎‎ﺔﻳﺭﺪﻨﻜﺳﻹﺍ ",
        std::vector<uint8_t>{ 2,2,2,2,2,2,2,1,1,1,1,1,1 }
    ));
    EXPECT_EQ(expected.rbegin()->first.size(), expected.rbegin()->second.size());
    expected.emplace_back(StyledText(
        u"Maktabat al-",
        std::vector<uint8_t>{ 2,3,3,3,3,3,4,5,5,5,5,6 }
    ));
    EXPECT_EQ(expected.rbegin()->first.size(), expected.rbegin()->second.size());
    expected.emplace_back(StyledText(
        u"Iskandarīyah",
        std::vector<uint8_t>{ 6,6,6,6,6,6,6,6,6,7,7,7 }
    ));
    EXPECT_EQ(expected.rbegin()->first.size(), expected.rbegin()->second.size());

    EXPECT_EQ(bidi.processStyledText(input, { 5, 18, 30 }), expected);
}

