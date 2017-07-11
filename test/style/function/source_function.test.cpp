#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_geometry_tile_feature.hpp>

#include <mbgl/style/function/source_function.hpp>

using namespace mbgl;
using namespace mbgl::style;

using namespace std::string_literals;

static StubGeometryTileFeature oneInteger {
    PropertyMap {{ "property", uint64_t(1) }}
};

static StubGeometryTileFeature oneDouble {
    PropertyMap {{ "property", 1.0 }}
};

static StubGeometryTileFeature oneString {
    PropertyMap {{ "property", "1"s }}
};

static StubGeometryTileFeature red {
    PropertyMap {{ "property", "red"s }}
};

static StubGeometryTileFeature oneTwoInteger {
    PropertyMap {{ "property", std::vector<Value>({uint64_t(1), uint64_t(2)}) }}
};

static StubGeometryTileFeature oneTwoDouble {
    PropertyMap {{ "property", std::vector<Value>({1.0, 2.0}) }}
};

static StubGeometryTileFeature oneTwoString {
    PropertyMap {{ "property", std::vector<Value>({"1"s, "2"s}) }}
};

static StubGeometryTileFeature trueFeature {
    PropertyMap {{ "property", true }}
};

static StubGeometryTileFeature falseFeature {
    PropertyMap {{ "property", false }}
};

TEST(SourceFunction, Identity) {
    EXPECT_EQ(1.0f, SourceFunction<float>("property", IdentityStops<float>(), 0.0f)
        .evaluate(oneInteger, 2.0f));
    EXPECT_EQ(1.0f, SourceFunction<float>("property", IdentityStops<float>(), 0.0f)
        .evaluate(oneDouble, 2.0f));
    EXPECT_EQ(0.0f, SourceFunction<float>("property", IdentityStops<float>(), 0.0f)
        .evaluate(oneString, 2.0f));
    EXPECT_EQ(2.0f, SourceFunction<float>("property", IdentityStops<float>())
        .evaluate(oneString, 2.0f));

    EXPECT_EQ(Color::red(), SourceFunction<Color>("property", IdentityStops<Color>(), Color::black())
        .evaluate(red, Color::black()));
    EXPECT_EQ(Color::black(), SourceFunction<Color>("property", IdentityStops<Color>(), Color::black())
        .evaluate(oneInteger, Color::black()));

    std::array<float, 2> zeroArray {{ 0, 0 }};
    EXPECT_EQ((std::array<float, 2> {{ 1, 2 }}), (SourceFunction<std::array<float, 2>>("property", IdentityStops<std::array<float, 2>>(), zeroArray)
        .evaluate(oneTwoInteger, zeroArray)));
    EXPECT_EQ((std::array<float, 2> {{ 1, 2 }}), (SourceFunction<std::array<float, 2>>("property", IdentityStops<std::array<float, 2>>(), zeroArray)
        .evaluate(oneTwoDouble, zeroArray)));
    EXPECT_EQ((std::array<float, 2> {{ 0, 0 }}), (SourceFunction<std::array<float, 2>>("property", IdentityStops<std::array<float, 2>>(), zeroArray)
        .evaluate(oneTwoString, zeroArray)));
}

TEST(SourceFunction, Categorical) {
    EXPECT_EQ(1.0f, SourceFunction<float>("property", CategoricalStops<float>({{ int64_t(1), 1.0f }}))
        .evaluate(oneInteger, 0.0f));
    EXPECT_EQ(1.0f, SourceFunction<float>("property", CategoricalStops<float>({{ int64_t(1), 1.0f }}))
        .evaluate(oneDouble, 0.0f));
    EXPECT_EQ(0.0f, SourceFunction<float>("property", CategoricalStops<float>({{ int64_t(1), 1.0f }}))
        .evaluate(oneString, 0.0f));

    CategoricalStops<float>::Stops stops;
    stops["1"s] = 1.0f;

    EXPECT_EQ(0.0f, SourceFunction<float>("property", CategoricalStops<float>(stops))
        .evaluate(oneInteger, 0.0f));
    EXPECT_EQ(0.0f, SourceFunction<float>("property", CategoricalStops<float>(stops))
        .evaluate(oneDouble, 0.0f));
    EXPECT_EQ(1.0f, SourceFunction<float>("property", CategoricalStops<float>(stops))
        .evaluate(oneString, 0.0f));

    EXPECT_EQ(1.0f, SourceFunction<float>("property", CategoricalStops<float>({{ true, 1.0f }}))
        .evaluate(trueFeature, 0.0f));
    EXPECT_EQ(0.0f, SourceFunction<float>("property", CategoricalStops<float>({{ true, 1.0f }}))
        .evaluate(falseFeature, 0.0f));
    EXPECT_EQ(0.0f, SourceFunction<float>("property", CategoricalStops<float>({{ false, 1.0f }}))
        .evaluate(trueFeature, 0.0f));
    EXPECT_EQ(1.0f, SourceFunction<float>("property", CategoricalStops<float>({{ false, 1.0f }}))
        .evaluate(falseFeature, 0.0f));
}
