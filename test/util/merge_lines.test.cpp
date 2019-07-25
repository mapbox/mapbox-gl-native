#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_geometry_tile_feature.hpp>

#include <mbgl/layout/merge_lines.hpp>
#include <mbgl/layout/symbol_feature.hpp>
#include <utility>

const std::u16string aaa = u"a";
const std::u16string bbb = u"b";

using namespace mbgl;

namespace {

PropertyMap properties;
LineString<int16_t> emptyLine;

} // namespace

class SymbolFeatureStub : public SymbolFeature {
public:
    SymbolFeatureStub(FeatureIdentifier id_, FeatureType type_, GeometryCollection geometry_,
                      PropertyMap properties_, optional<std::u16string> text_,
                      optional<std::string> icon_, std::size_t index_) :
        SymbolFeature(std::make_unique<StubGeometryTileFeature>(std::move(id_), type_, std::move(geometry_), std::move(properties_)))
    {
        if (text_) {
            formattedText = TaggedString(*text_, SectionOptions(1.0, {}));
        }
        icon = std::move(icon_);
        index = index_;
    }
};

TEST(MergeLines, SameText) {
    // merges lines with the same text
    std::vector<mbgl::SymbolFeature> input1;
    input1.push_back(SymbolFeatureStub({}, FeatureType::LineString, {{{0, 0}, {1, 0}, {2, 0}}}, properties, aaa, {}, 0));
    input1.push_back(SymbolFeatureStub({}, FeatureType::LineString, {{{4, 0}, {5, 0}, {6, 0}}}, properties, bbb, {}, 0));
    input1.push_back(SymbolFeatureStub({}, FeatureType::LineString, {{{8, 0}, {9, 0}}}, properties, aaa, {}, 0));
    input1.push_back(SymbolFeatureStub({}, FeatureType::LineString, {{{2, 0}, {3, 0}, {4, 0}}}, properties, aaa, {}, 0));
    input1.push_back(SymbolFeatureStub({}, FeatureType::LineString, {{{6, 0}, {7, 0}, {8, 0}}}, properties, aaa, {}, 0));
    input1.push_back(SymbolFeatureStub({}, FeatureType::LineString, {{{5, 0}, {6, 0}}}, properties, aaa, {}, 0));

    std::vector<StubGeometryTileFeature> expected1;
    expected1.emplace_back(StubGeometryTileFeature({}, FeatureType::LineString, {{{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}}}, properties));
    expected1.emplace_back(StubGeometryTileFeature({}, FeatureType::LineString, {{{4, 0}, {5, 0}, {6, 0}}}, properties));
    expected1.emplace_back(StubGeometryTileFeature({}, FeatureType::LineString, {{{5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}}}, properties));
    expected1.emplace_back(StubGeometryTileFeature({}, FeatureType::LineString, { emptyLine }, properties));
    expected1.emplace_back(StubGeometryTileFeature({}, FeatureType::LineString, { emptyLine }, properties));
    expected1.emplace_back(StubGeometryTileFeature({}, FeatureType::LineString, { emptyLine }, properties));
    
    
    mbgl::util::mergeLines(input1);

    for (int i = 0; i < 6; i++) {
        EXPECT_TRUE(input1[i].geometry == expected1[i].getGeometries());
    }
}

TEST(MergeLines, BothEnds) {
    // mergeLines handles merge from both ends
    std::vector<mbgl::SymbolFeature> input2;
    input2.push_back(SymbolFeatureStub { {}, FeatureType::LineString, {{{0, 0}, {1, 0}, {2, 0}}}, properties, aaa, {}, 0 });
    input2.push_back(SymbolFeatureStub { {}, FeatureType::LineString, {{{4, 0}, {5, 0}, {6, 0}}}, properties, aaa, {}, 0 });
    input2.push_back(SymbolFeatureStub { {}, FeatureType::LineString, {{{2, 0}, {3, 0}, {4, 0}}}, properties, aaa, {}, 0 });

    std::vector<StubGeometryTileFeature> expected2;
    expected2.emplace_back(StubGeometryTileFeature({}, FeatureType::LineString, {{{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}}}, properties));
    expected2.emplace_back(StubGeometryTileFeature({}, FeatureType::LineString, { emptyLine }, properties));
    expected2.emplace_back(StubGeometryTileFeature({}, FeatureType::LineString, { emptyLine }, properties));


    mbgl::util::mergeLines(input2);

    for (int i = 0; i < 3; i++) {
        EXPECT_TRUE(input2[i].geometry == expected2[i].getGeometries());
    }
}

TEST(MergeLines, CircularLines) {
    // mergeLines handles circular lines
    std::vector<mbgl::SymbolFeature> input3;
    input3.push_back(SymbolFeatureStub { {}, FeatureType::LineString, {{{0, 0}, {1, 0}, {2, 0}}}, properties, aaa, {}, 0 });
    input3.push_back(SymbolFeatureStub { {}, FeatureType::LineString, {{{2, 0}, {3, 0}, {4, 0}}}, properties, aaa, {}, 0 });
    input3.push_back(SymbolFeatureStub { {}, FeatureType::LineString, {{{4, 0}, {0, 0}}}, properties, aaa, {}, 0 });

    std::vector<StubGeometryTileFeature> expected3;
    expected3.emplace_back(StubGeometryTileFeature({}, FeatureType::LineString, {{{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {0, 0}}}, properties));
    expected3.emplace_back(StubGeometryTileFeature({}, FeatureType::LineString, { emptyLine }, properties));
    expected3.emplace_back(StubGeometryTileFeature({}, FeatureType::LineString, { emptyLine }, properties));


    mbgl::util::mergeLines(input3);

    for (int i = 0; i < 3; i++) {
        EXPECT_TRUE(input3[i].geometry == expected3[i].getGeometries());
    }
}

TEST(MergeLines, EmptyOuterGeometry) {
    std::vector<mbgl::SymbolFeature> input;
    input.push_back(SymbolFeatureStub { {}, FeatureType::LineString, {}, properties, aaa, {}, 0 });

    const StubGeometryTileFeature expected{ {}, FeatureType::LineString, {}, properties };

    mbgl::util::mergeLines(input);

    EXPECT_EQ(input[0].geometry, expected.getGeometries());
}

TEST(MergeLines, EmptyInnerGeometry) {
    std::vector<mbgl::SymbolFeature> input;
    input.push_back(SymbolFeatureStub { {}, FeatureType::LineString, {}, properties, aaa, {}, 0 });

    const StubGeometryTileFeature expected{ {}, FeatureType::LineString, {}, properties };

    mbgl::util::mergeLines(input);

    EXPECT_EQ(input[0].geometry, expected.getGeometries());
}
