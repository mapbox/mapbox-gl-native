#include "gtest/gtest.h"

#include <mbgl/util/merge_lines.hpp>


TEST(mergeLines, mergeLines) {

    std::u32string a = U"a";
    std::u32string b = U"b";

    // merges lines with the same text
    std::vector<mbgl::SymbolFeature> input1 = {
        { {{{0, 0}, {1, 0}, {2, 0}}}, a, "" },
        { {{{4, 0}, {5, 0}, {6, 0}}}, b, "" },
        { {{{8, 0}, {9, 0}}}, a, "" },
        { {{{2, 0}, {3, 0}, {4, 0}}}, a, "" },
        { {{{6, 0}, {7, 0}, {8, 0}}}, a, "" },
        { {{{5, 0}, {6, 0}}}, a, "" }
    };

    std::vector<mbgl::SymbolFeature> expected1 = {
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

    // mergeLines handles merge from both ends
    std::vector<mbgl::SymbolFeature> input2 = {
        { {{{0, 0}, {1, 0}, {2, 0}}}, a, "" },
        { {{{4, 0}, {5, 0}, {6, 0}}}, a, "" },
        { {{{2, 0}, {3, 0}, {4, 0}}}, a, "" }
    };

    std::vector<mbgl::SymbolFeature> expected2 = {
        { {{{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}}}, a, "" },
        { {{}}, a, "" },
        { {{}}, a, "" }
    };

    mbgl::util::mergeLines(input2);

    for (int i = 0; i < 3; i++) {
        EXPECT_EQ(input2[i].geometry, expected2[i].geometry);
    }

    // mergeLines handles circular lines
    std::vector<mbgl::SymbolFeature> input3 = {
        { {{{0, 0}, {1, 0}, {2, 0}}}, a, "" },
        { {{{2, 0}, {3, 0}, {4, 0}}}, a, "" },
        { {{{4, 0}, {0, 0}}}, a, "" }
    };

    std::vector<mbgl::SymbolFeature> expected3 = {
        { {{{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {0, 0}}}, a, "" },
        { {{}}, a, "" },
        { {{}}, a, "" }
    };

    mbgl::util::mergeLines(input3);

    for (int i = 0; i < 3; i++) {
        EXPECT_EQ(input3[i].geometry, expected3[i].geometry);
    }
}
