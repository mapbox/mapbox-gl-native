#include <mbgl/test/util.hpp>

#include <mbgl/programs/symbol_program.hpp>

using namespace mbgl;

TEST(SymbolProgram, SymbolSizeBinder) {
    auto binder = SymbolSizeBinder::create(5.0f, 12.0f, 0.0f);
    auto uniformValues = binder->uniformValues(5.5f);
    EXPECT_EQ(uniformValues.get<uniforms::u_is_size_zoom_constant>().t, true);
    EXPECT_EQ(uniformValues.get<uniforms::u_is_size_feature_constant>().t, true);
    EXPECT_EQ(uniformValues.get<uniforms::u_size>().t, 12.0f);

    binder = SymbolSizeBinder::create(1.0f, style::CameraFunction<float>(style::ExponentialStops<float>({
        {0.0f, 8.0f},
        {10.0f, 18.0f}
    }, 1.0f)), 0.0f);
    uniformValues = binder->uniformValues(1.5f);
    EXPECT_EQ(uniformValues.get<uniforms::u_is_size_zoom_constant>().t, false);
    EXPECT_EQ(uniformValues.get<uniforms::u_is_size_feature_constant>().t, true);
    EXPECT_EQ(uniformValues.get<uniforms::u_size>().t, 9.5f);
    
    binder = SymbolSizeBinder::create(0.0f, style::CameraFunction<float>(style::ExponentialStops<float>({
        {1.0f, 8.0f},
        {11.0f, 18.0f}
    }, 1.0f)), 0.0f);
    uniformValues = binder->uniformValues(0.5f);
    EXPECT_EQ(uniformValues.get<uniforms::u_is_size_zoom_constant>().t, false);
    EXPECT_EQ(uniformValues.get<uniforms::u_is_size_feature_constant>().t, true);
    EXPECT_EQ(uniformValues.get<uniforms::u_size>().t, 8.0f);
    
    binder = SymbolSizeBinder::create(12.0f, style::CameraFunction<float>(style::ExponentialStops<float>({
        {1.0f, 8.0f},
        {11.0f, 18.0f}
    }, 1.0f)), 0.0f);
    uniformValues = binder->uniformValues(12.5f);
    EXPECT_EQ(uniformValues.get<uniforms::u_is_size_zoom_constant>().t, false);
    EXPECT_EQ(uniformValues.get<uniforms::u_is_size_feature_constant>().t, true);
    EXPECT_EQ(uniformValues.get<uniforms::u_size>().t, 18.0f);
    
    binder = SymbolSizeBinder::create(0.0f, style::SourceFunction<float>("x", style::ExponentialStops<float>({
        {1.0f, 8.0f},
        {11.0f, 18.0f}
    }, 1.0f)), 0.0f);
    uniformValues = binder->uniformValues(12.5f);
    EXPECT_EQ(uniformValues.get<uniforms::u_is_size_zoom_constant>().t, true);
    EXPECT_EQ(uniformValues.get<uniforms::u_is_size_feature_constant>().t, false);

    binder = SymbolSizeBinder::create(5.0f, style::CompositeFunction<float>("x", style::CompositeExponentialStops<float>({
        {1.0f, {{0.0f, 8.0f}, {100.0f, 18.0f}}},
        {11.0f, {{0.0f, 12.0f}, {100.0f, 24.9f}}}
    }, 1.0f)), 0.0f);
    uniformValues = binder->uniformValues(5.5f);
    EXPECT_EQ(uniformValues.get<uniforms::u_is_size_zoom_constant>().t, false);
    EXPECT_EQ(uniformValues.get<uniforms::u_is_size_feature_constant>().t, false);
    EXPECT_EQ(uniformValues.get<uniforms::u_size_t>().t, 0.45f);
}
