#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_geometry_tile_feature.hpp>

#include <mbgl/style/function/composite_function.hpp>

using namespace mbgl;
using namespace mbgl::style;

using namespace std::string_literals;

static StubGeometryTileFeature oneInteger {
    PropertyMap {{ "property", uint64_t(1) }}
};

TEST(CompositeFunction, ZoomInterpolation) {
    EXPECT_EQ(40.0f, CompositeFunction<float>("property", CompositeExponentialStops<float>({
        {0.0f, {{uint64_t(1), 24.0f}}},
        {1.5f, {{uint64_t(1), 36.0f}}},
        {3.0f, {{uint64_t(1), 48.0f}}}
    }), 0.0f)
    .evaluate(2.0f, oneInteger, -1.0f)) << "Should interpolate between stops";
    
    EXPECT_EQ(33.0, CompositeFunction<float>("property", CompositeExponentialStops<float>({
        {5.0f, {{uint64_t(1), 33.0f}}},
        {10.0f, {{uint64_t(1), 66.0f}}}
    }), 0.0f)
    .evaluate(0.0f, oneInteger, -1.0f)) << "Use first stop output for input values from -inf to first stop";
    
    EXPECT_EQ(66.0, CompositeFunction<float>("property", CompositeExponentialStops<float>({
        {0.0f, {{uint64_t(1), 33.0f}}},
        {10.0f, {{uint64_t(1), 66.0f}}}
    }), 0.0f)
    .evaluate(20.0f, oneInteger, -1.0f)) << "Use last stop output for input values from last stop to +inf";

    EXPECT_EQ(66.0f, CompositeFunction<float>("property", CompositeExponentialStops<float>({
        {0.0f, {{uint64_t(1), 33.0f}}},
        {10.0f, {{uint64_t(1), 66.0f}}}
    }), 0.0f)
    .evaluate(10.0f, oneInteger, -1.0f)) << "Should interpolate TO the last stop.";
    
    EXPECT_EQ(33.0f, CompositeFunction<float>("property", CompositeExponentialStops<float>({
        {0.0f, {{uint64_t(1), 33.0f}}},
        {10.0f, {{uint64_t(1), 66.0f}}}
    }), 0.0f)
    .evaluate(0.0f, oneInteger, -1.0f)) << "Should interpolate TO the first stop";
}

TEST(CompositeFunction, Issue8460) {
    CompositeFunction<float> fn1("property", CompositeExponentialStops<float>({
        {15.0f, {{uint64_t(1), 0.0f}}},
        {15.2f, {{uint64_t(1), 600.0f}}},
    }), 0.0f);

    EXPECT_NEAR(  0.0f, fn1.evaluate(15.0f, oneInteger, -1.0f), 0.00);
    EXPECT_NEAR(300.0f, fn1.evaluate(15.1f, oneInteger, -1.0f), 0.01);
    EXPECT_NEAR(600.0f, fn1.evaluate(15.2f, oneInteger, -1.0f), 0.00);
    EXPECT_NEAR(600.0f, fn1.evaluate(16.0f, oneInteger, -1.0f), 0.00);

    CompositeFunction<float> fn2("property", CompositeExponentialStops<float>({
        {15.0f, {{uint64_t(1), 0.0f}}},
        {15.2f, {{uint64_t(1), 300.0f}}},
        {18.0f, {{uint64_t(1), 600.0f}}},
    }), 0.0f);

    EXPECT_NEAR(  0.0f, fn2.evaluate(15.0f, oneInteger, -1.0f), 0.00);
    EXPECT_NEAR(150.0f, fn2.evaluate(15.1f, oneInteger, -1.0f), 0.01);
    EXPECT_NEAR(300.0f, fn2.evaluate(15.2f, oneInteger, -1.0f), 0.00);
    EXPECT_NEAR(385.71f, fn2.evaluate(16.0f, oneInteger, -1.0f), 0.01);
    EXPECT_NEAR(600.0f, fn2.evaluate(18.0f, oneInteger, -1.0f), 0.00);
    EXPECT_NEAR(600.0f, fn2.evaluate(19.0f, oneInteger, -1.0f), 0.00);
}
