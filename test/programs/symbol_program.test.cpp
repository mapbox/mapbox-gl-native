#include <mbgl/test/util.hpp>

#include <mbgl/programs/symbol_program.hpp>
#include <mbgl/style/expression/dsl.hpp>

using namespace mbgl;
using namespace mbgl::style::expression::dsl;

TEST(SymbolProgram, SymbolSizeBinder) {
    auto binder = SymbolSizeBinder::create(5.0f, 12.0f, 0.0f);
    auto uniformValues = binder->uniformValues(5.5f);
    EXPECT_EQ(uniformValues.get<uniforms::u_is_size_zoom_constant>(), true);
    EXPECT_EQ(uniformValues.get<uniforms::u_is_size_feature_constant>(), true);
    EXPECT_EQ(uniformValues.get<uniforms::u_size>(), 12.0f);

    binder = SymbolSizeBinder::create(1.0f, style::PropertyExpression<float>(
        interpolate(
            linear(),
            zoom(),
            0., literal(8.),
            10., literal(18.))), 0.0f);
    uniformValues = binder->uniformValues(1.5f);
    EXPECT_EQ(uniformValues.get<uniforms::u_is_size_zoom_constant>(), false);
    EXPECT_EQ(uniformValues.get<uniforms::u_is_size_feature_constant>(), true);
    EXPECT_EQ(uniformValues.get<uniforms::u_size>(), 9.5f);
    
    binder = SymbolSizeBinder::create(0.0f, style::PropertyExpression<float>(
        interpolate(
            linear(),
            zoom(),
            1., literal(8.),
            11., literal(18.))), 0.0f);
    uniformValues = binder->uniformValues(0.5f);
    EXPECT_EQ(uniformValues.get<uniforms::u_is_size_zoom_constant>(), false);
    EXPECT_EQ(uniformValues.get<uniforms::u_is_size_feature_constant>(), true);
    EXPECT_EQ(uniformValues.get<uniforms::u_size>(), 8.0f);
    
    binder = SymbolSizeBinder::create(12.0f, style::PropertyExpression<float>(
        interpolate(
            linear(),
            zoom(),
            1., literal(8.),
            11., literal(18.))), 0.0f);
    uniformValues = binder->uniformValues(12.5f);
    EXPECT_EQ(uniformValues.get<uniforms::u_is_size_zoom_constant>(), false);
    EXPECT_EQ(uniformValues.get<uniforms::u_is_size_feature_constant>(), true);
    EXPECT_EQ(uniformValues.get<uniforms::u_size>(), 18.0f);
    
    binder = SymbolSizeBinder::create(0.0f, style::PropertyExpression<float>(
        interpolate(
            linear(),
            number(get("x")),
            1., literal(8.),
            11., literal(18.))), 0.0f);
    uniformValues = binder->uniformValues(12.5f);
    EXPECT_EQ(uniformValues.get<uniforms::u_is_size_zoom_constant>(), true);
    EXPECT_EQ(uniformValues.get<uniforms::u_is_size_feature_constant>(), false);

    binder = SymbolSizeBinder::create(5.0f, style::PropertyExpression<float>(
        interpolate(
            linear(),
            zoom(),
            1., interpolate(linear(), number(get("x")), 0., literal(8.), 100., literal(18.)),
            11., interpolate(linear(), number(get("x")), 0., literal(12.), 100., literal(24.9)))), 0.0f);
    uniformValues = binder->uniformValues(5.5f);
    EXPECT_EQ(uniformValues.get<uniforms::u_is_size_zoom_constant>(), false);
    EXPECT_EQ(uniformValues.get<uniforms::u_is_size_feature_constant>(), false);
    EXPECT_EQ(uniformValues.get<uniforms::u_size_t>(), 0.45f);
}
