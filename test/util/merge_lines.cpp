#include <mbgl/test/util.hpp>

#include <mbgl/util/merge_lines.hpp>

const std::u32string aaa = U"a";
const std::u32string bbb = U"b";

TEST(MergeLines, SameText) {
    // merges lines with the same text
    std::vector<mbgl::SymbolFeature> input1 = {
        {{{{0, 0}, {1, 0}, {2, 0}}}, aaa, "", 0}, {{{{4, 0}, {5, 0}, {6, 0}}}, bbb, "", 0},
        {{{{8, 0}, {9, 0}}}, aaa, "", 0},         {{{{2, 0}, {3, 0}, {4, 0}}}, aaa, "", 0},
        {{{{6, 0}, {7, 0}, {8, 0}}}, aaa, "", 0}, {{{{5, 0}, {6, 0}}}, aaa, "", 0}};

    const std::vector<mbgl::SymbolFeature> expected1 = {
        {{{{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}}}, aaa, "", 0},
        {{{{4, 0}, {5, 0}, {6, 0}}}, bbb, "", 0},
        {{{{5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}}}, aaa, "", 0},
        {{{}}, aaa, "", 0},
        {{{}}, aaa, "", 0},
        {{{}}, aaa, "", 0}};

    mbgl::util::mergeLines(input1);

    for (int i = 0; i < 6; i++) {
        EXPECT_TRUE(input1[i].geometry == expected1[i].geometry);
    }
}

TEST(MergeLines, BothEnds) {
    // mergeLines handles merge from both ends
    std::vector<mbgl::SymbolFeature> input2 = {{{{{0, 0}, {1, 0}, {2, 0}}}, aaa, "", 0},
                                               {{{{4, 0}, {5, 0}, {6, 0}}}, aaa, "", 0},
                                               {{{{2, 0}, {3, 0}, {4, 0}}}, aaa, "", 0}};

    const std::vector<mbgl::SymbolFeature> expected2 = {
        {{{{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}}}, aaa, "", 0},
        {{{}}, aaa, "", 0},
        {{{}}, aaa, "", 0}};

    mbgl::util::mergeLines(input2);

    for (int i = 0; i < 3; i++) {
        EXPECT_TRUE(input2[i].geometry == expected2[i].geometry);
    }
}

TEST(MergeLines, CircularLines) {
    // mergeLines handles circular lines
    std::vector<mbgl::SymbolFeature> input3 = {{{{{0, 0}, {1, 0}, {2, 0}}}, aaa, "", 0},
                                               {{{{2, 0}, {3, 0}, {4, 0}}}, aaa, "", 0},
                                               {{{{4, 0}, {0, 0}}}, aaa, "", 0}};

    const std::vector<mbgl::SymbolFeature> expected3 = {
        {{{{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {0, 0}}}, aaa, "", 0},
        {{{}}, aaa, "", 0},
        {{{}}, aaa, "", 0}};

    mbgl::util::mergeLines(input3);

    for (int i = 0; i < 3; i++) {
        EXPECT_TRUE(input3[i].geometry == expected3[i].geometry);
    }
}
