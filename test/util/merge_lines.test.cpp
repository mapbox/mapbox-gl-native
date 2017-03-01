#include <mbgl/test/util.hpp>

#include <mbgl/layout/merge_lines.hpp>
#include <mbgl/layout/symbol_feature.hpp>

const std::u16string aaa = u"a";
const std::u16string bbb = u"b";

using namespace mbgl;

class GeometryTileFeatureStub : public GeometryTileFeature {
public:
    GeometryTileFeatureStub(optional<FeatureIdentifier> id_, FeatureType type_, GeometryCollection geometry_,
                  std::unordered_map<std::string, Value> properties_) :
        id(id_),
        type(type_),
        geometry(geometry_),
        properties(properties_)
    {}
    
    FeatureType getType() const override { return type; }
    optional<Value> getValue(const std::string& key) const override {
        auto it = properties.find(key);
        if (it != properties.end()) {
            return it->second;
        }
        return {};
    };
    std::unordered_map<std::string,Value> getProperties() const override { return properties; };
    optional<FeatureIdentifier> getID() const override { return id; };
    GeometryCollection getGeometries() const override { return geometry; };
    
    optional<FeatureIdentifier> id;
    FeatureType type;
    GeometryCollection geometry;
    std::unordered_map<std::string,Value> properties;
};

class SymbolFeatureStub : public SymbolFeature {
public:
    SymbolFeatureStub(optional<FeatureIdentifier> id_, FeatureType type_, GeometryCollection geometry_,
                            std::unordered_map<std::string, Value> properties_, optional<std::u16string> text_,
                            optional<std::string> icon_, std::size_t index_) :
        SymbolFeature(std::make_unique<GeometryTileFeatureStub>(id_, type_, geometry_, properties_))
    {
        text = text_;
        icon = icon_;
        index = index_;
    }
};

TEST(MergeLines, SameText) {
    // merges lines with the same text
    std::vector<mbgl::SymbolFeature> input1;
    input1.push_back(SymbolFeatureStub({}, FeatureType::LineString, {{{0, 0}, {1, 0}, {2, 0}}}, {}, aaa, {}, 0));
    input1.push_back(SymbolFeatureStub({}, FeatureType::LineString, {{{4, 0}, {5, 0}, {6, 0}}}, {}, bbb, {}, 0));
    input1.push_back(SymbolFeatureStub({}, FeatureType::LineString, {{{8, 0}, {9, 0}}}, {}, aaa, {}, 0));
    input1.push_back(SymbolFeatureStub({}, FeatureType::LineString, {{{2, 0}, {3, 0}, {4, 0}}}, {}, aaa, {}, 0));
    input1.push_back(SymbolFeatureStub({}, FeatureType::LineString, {{{6, 0}, {7, 0}, {8, 0}}}, {}, aaa, {}, 0));
    input1.push_back(SymbolFeatureStub({}, FeatureType::LineString, {{{5, 0}, {6, 0}}}, {}, aaa, {}, 0));

    const std::vector<GeometryTileFeatureStub> expected1 = {
        { {}, FeatureType::LineString, {{{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}}}, {} },
        { {}, FeatureType::LineString, {{{4, 0}, {5, 0}, {6, 0}}}, {} },
        { {}, FeatureType::LineString, {{{5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}}}, {} },
        { {}, FeatureType::LineString, {{}}, {} },
        { {}, FeatureType::LineString, {{}}, {} },
        { {}, FeatureType::LineString, {{}}, {} }
    };
    
    mbgl::util::mergeLines(input1);

    for (int i = 0; i < 6; i++) {
        EXPECT_TRUE(input1[i].geometry == expected1[i].getGeometries());
    }
}

TEST(MergeLines, BothEnds) {
    // mergeLines handles merge from both ends
    std::vector<mbgl::SymbolFeature> input2;
    input2.push_back(SymbolFeatureStub { {}, FeatureType::LineString, {{{0, 0}, {1, 0}, {2, 0}}}, {}, aaa, {}, 0 });
    input2.push_back(SymbolFeatureStub { {}, FeatureType::LineString, {{{4, 0}, {5, 0}, {6, 0}}}, {}, aaa, {}, 0 });
    input2.push_back(SymbolFeatureStub { {}, FeatureType::LineString, {{{2, 0}, {3, 0}, {4, 0}}}, {}, aaa, {}, 0 });

    const std::vector<GeometryTileFeatureStub> expected2 = {
        { {}, FeatureType::LineString, {{{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}}}, {} },
        { {}, FeatureType::LineString, {{}}, {} },
        { {}, FeatureType::LineString, {{}}, {} }
    };

    mbgl::util::mergeLines(input2);

    for (int i = 0; i < 3; i++) {
        EXPECT_TRUE(input2[i].geometry == expected2[i].getGeometries());
    }
}

TEST(MergeLines, CircularLines) {
    // mergeLines handles circular lines
    std::vector<mbgl::SymbolFeature> input3;
    input3.push_back(SymbolFeatureStub { {}, FeatureType::LineString, {{{0, 0}, {1, 0}, {2, 0}}}, {}, aaa, {}, 0 });
    input3.push_back(SymbolFeatureStub { {}, FeatureType::LineString, {{{2, 0}, {3, 0}, {4, 0}}}, {}, aaa, {}, 0 });
    input3.push_back(SymbolFeatureStub { {}, FeatureType::LineString, {{{4, 0}, {0, 0}}}, {}, aaa, {}, 0 });

    const std::vector<GeometryTileFeatureStub> expected3 = {
        { {}, FeatureType::LineString, {{{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {0, 0}}}, {} },
        { {}, FeatureType::LineString, {{}}, {} },
        { {}, FeatureType::LineString, {{}}, {} }
    };

    mbgl::util::mergeLines(input3);

    for (int i = 0; i < 3; i++) {
        EXPECT_TRUE(input3[i].geometry == expected3[i].getGeometries());
    }
}

TEST(MergeLines, EmptyOuterGeometry) {
    std::vector<mbgl::SymbolFeature> input;
    input.push_back(SymbolFeatureStub { {}, FeatureType::LineString, {}, {}, aaa, {}, 0 });

    const std::vector<GeometryTileFeatureStub> expected = { { {}, FeatureType::LineString, {}, {} } };

    mbgl::util::mergeLines(input);

    EXPECT_EQ(input[0].geometry, expected[0].getGeometries());
}

TEST(MergeLines, EmptyInnerGeometry) {
    std::vector<mbgl::SymbolFeature> input;
    input.push_back(SymbolFeatureStub { {}, FeatureType::LineString, {{}}, {}, aaa, {}, 0 });

    const std::vector<GeometryTileFeatureStub> expected = { { {}, FeatureType::LineString, {{}}, {} } };

    mbgl::util::mergeLines(input);

    EXPECT_EQ(input[0].geometry, expected[0].getGeometries());
}
