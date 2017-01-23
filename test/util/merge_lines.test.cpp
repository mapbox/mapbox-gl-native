#include <mbgl/test/util.hpp>

#include <mbgl/layout/merge_lines.hpp>
#include <mbgl/layout/symbol_feature.hpp>

const std::u16string aaa = u"a";
const std::u16string bbb = u"b";

using namespace mbgl;

TEST(MergeLines, SameText) {
    // merges lines with the same text
    std::vector<mbgl::SymbolFeature> input1 = {
        { FeatureType::LineString, {{{0, 0}, {1, 0}, {2, 0}}}, aaa, {}, 0 },
        { FeatureType::LineString, {{{4, 0}, {5, 0}, {6, 0}}}, bbb, {}, 0 },
        { FeatureType::LineString, {{{8, 0}, {9, 0}}}, aaa, {}, 0 },
        { FeatureType::LineString, {{{2, 0}, {3, 0}, {4, 0}}}, aaa, {}, 0 },
        { FeatureType::LineString, {{{6, 0}, {7, 0}, {8, 0}}}, aaa, {}, 0 },
        { FeatureType::LineString, {{{5, 0}, {6, 0}}}, aaa, {}, 0 }
    };

    const std::vector<mbgl::SymbolFeature> expected1 = {
        { FeatureType::LineString, {{{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}}}, aaa, {}, 0 },
        { FeatureType::LineString, {{{4, 0}, {5, 0}, {6, 0}}}, bbb, {}, 0 },
        { FeatureType::LineString, {{{5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}}}, aaa, {}, 0 },
        { FeatureType::LineString, {{}}, aaa, {}, 0 },
        { FeatureType::LineString, {{}}, aaa, {}, 0 },
        { FeatureType::LineString, {{}}, aaa, {}, 0 }
    };

    mbgl::util::mergeLines(input1);

    for (int i = 0; i < 6; i++) {
        EXPECT_TRUE(input1[i].geometry == expected1[i].geometry);
    }
}

TEST(MergeLines, BothEnds) {
    // mergeLines handles merge from both ends
    std::vector<mbgl::SymbolFeature> input2 = {
        { FeatureType::LineString, {{{0, 0}, {1, 0}, {2, 0}}}, aaa, {}, 0 },
        { FeatureType::LineString, {{{4, 0}, {5, 0}, {6, 0}}}, aaa, {}, 0 },
        { FeatureType::LineString, {{{2, 0}, {3, 0}, {4, 0}}}, aaa, {}, 0 }
    };

    const std::vector<mbgl::SymbolFeature> expected2 = {
        { FeatureType::LineString, {{{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}}}, aaa, {}, 0 },
        { FeatureType::LineString, {{}}, aaa, {}, 0 },
        { FeatureType::LineString, {{}}, aaa, {}, 0 }
    };

    mbgl::util::mergeLines(input2);

    for (int i = 0; i < 3; i++) {
        EXPECT_TRUE(input2[i].geometry == expected2[i].geometry);
    }
}

TEST(MergeLines, CircularLines) {
    // mergeLines handles circular lines
    std::vector<mbgl::SymbolFeature> input3 = {
        { FeatureType::LineString, {{{0, 0}, {1, 0}, {2, 0}}}, aaa, {}, 0 },
        { FeatureType::LineString, {{{2, 0}, {3, 0}, {4, 0}}}, aaa, {}, 0 },
        { FeatureType::LineString, {{{4, 0}, {0, 0}}}, aaa, {}, 0 }
    };

    const std::vector<mbgl::SymbolFeature> expected3 = {
        { FeatureType::LineString, {{{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {0, 0}}}, aaa, {}, 0 },
        { FeatureType::LineString, {{}}, aaa, {}, 0 },
        { FeatureType::LineString, {{}}, aaa, {}, 0 }
    };

    mbgl::util::mergeLines(input3);

    for (int i = 0; i < 3; i++) {
        EXPECT_TRUE(input3[i].geometry == expected3[i].geometry);
    }
}

TEST(MergeLines, EmptyOuterGeometry) {
    std::vector<mbgl::SymbolFeature> input = {
        { FeatureType::LineString, {}, aaa, {}, 0 },
    };

    const std::vector<mbgl::SymbolFeature> expected = input;

    mbgl::util::mergeLines(input);

    EXPECT_EQ(expected[0].geometry, input[0].geometry);
}

TEST(MergeLines, EmptyInnerGeometry) {
    std::vector<mbgl::SymbolFeature> input = {
        { FeatureType::LineString, {{}}, aaa, {}, 0 },
    };

    const std::vector<mbgl::SymbolFeature> expected = input;

    mbgl::util::mergeLines(input);

    EXPECT_EQ(expected[0].geometry, input[0].geometry);
}
