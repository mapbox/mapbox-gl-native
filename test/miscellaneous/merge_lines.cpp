#include "../fixtures/util.hpp"

#include <mbgl/util/merge_lines.hpp>

const std::u32string a = U"a";
const std::u32string b = U"b";

TEST(MergeLines, SameText) {
    // merges lines with the same text
    std::vector<mbgl::SymbolFeature> input1 = {
        { {{{0, 0}, {1, 0}, {2, 0}}}, a, "" },
        { {{{4, 0}, {5, 0}, {6, 0}}}, b, "" },
        { {{{8, 0}, {9, 0}}}, a, "" },
        { {{{2, 0}, {3, 0}, {4, 0}}}, a, "" },
        { {{{6, 0}, {7, 0}, {8, 0}}}, a, "" },
        { {{{5, 0}, {6, 0}}}, a, "" }
    };

    const std::vector<mbgl::SymbolFeature> expected1 = {
        { {{{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}}}, a, "" },
        { {{{4, 0}, {5, 0}, {6, 0}}}, b, "" },
        { {{{5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}}}, a, "" },
        { {{}}, a, "" },
        { {{}}, a, "" },
        { {{}}, a, "" }
    };

    mbgl::util::mergeLines(input1);

    for (int i = 0; i < 6; i++) {
        EXPECT_EQ(input1[i].geometry, expected1[i].geometry);
    }
}

TEST(MergeLines, BothEnds) {
    // mergeLines handles merge from both ends
    std::vector<mbgl::SymbolFeature> input2 = {
        { {{{0, 0}, {1, 0}, {2, 0}}}, a, "" },
        { {{{4, 0}, {5, 0}, {6, 0}}}, a, "" },
        { {{{2, 0}, {3, 0}, {4, 0}}}, a, "" }
    };

    const std::vector<mbgl::SymbolFeature> expected2 = {
        { {{{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}}}, a, "" },
        { {{}}, a, "" },
        { {{}}, a, "" }
    };

    mbgl::util::mergeLines(input2);

    for (int i = 0; i < 3; i++) {
        EXPECT_EQ(input2[i].geometry, expected2[i].geometry);
    }
}

TEST(MergeLines, CircularLines) {
    // mergeLines handles circular lines
    std::vector<mbgl::SymbolFeature> input3 = {
        { {{{0, 0}, {1, 0}, {2, 0}}}, a, "" },
        { {{{2, 0}, {3, 0}, {4, 0}}}, a, "" },
        { {{{4, 0}, {0, 0}}}, a, "" }
    };

    const std::vector<mbgl::SymbolFeature> expected3 = {
        { {{{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {0, 0}}}, a, "" },
        { {{}}, a, "" },
        { {{}}, a, "" }
    };

    mbgl::util::mergeLines(input3);

    for (int i = 0; i < 3; i++) {
        EXPECT_EQ(input3[i].geometry, expected3[i].geometry);
    }
}
