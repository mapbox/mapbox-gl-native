#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_geometry_tile_feature.hpp>

#include <mbgl/style/function/source_function.hpp>
#include <mbgl/style/expression/dsl.hpp>

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

TEST(SourceFunction, Defaults) {
    using namespace mbgl::style::expression::dsl;

    EXPECT_EQ(1.0f, SourceFunction<float>(number(get("property")), 0.0)
        .evaluate(oneInteger, 2.0f));
    EXPECT_EQ(1.0f, SourceFunction<float>(number(get("property")), 0.0)
        .evaluate(oneDouble, 2.0f));
    EXPECT_EQ(0.0f, SourceFunction<float>(number(get("property")), 0.0)
        .evaluate(oneString, 2.0f));
    EXPECT_EQ(2.0f, SourceFunction<float>(number(get("property")))
        .evaluate(oneString, 2.0f));
}
