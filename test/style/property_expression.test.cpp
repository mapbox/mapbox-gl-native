#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_geometry_tile_feature.hpp>

#include <mbgl/style/property_expression.hpp>
#include <mbgl/renderer/property_evaluator.hpp>
#include <mbgl/renderer/property_evaluation_parameters.hpp>
#include <mbgl/style/expression/dsl.hpp>
#include <mbgl/style/expression/format_section_override.hpp>

using namespace mbgl;
using namespace mbgl::style;
using namespace mbgl::style::expression;
using namespace mbgl::style::expression::dsl;

using namespace std::string_literals;

static const StubGeometryTileFeature oneInteger{PropertyMap{{"property", uint64_t(1)}}};

static const StubGeometryTileFeature oneDouble{PropertyMap{{"property", 1.0}}};

static const StubGeometryTileFeature oneString{PropertyMap{{"property", "1"s}}};

static const StubGeometryTileFeature oneColor{PropertyMap{{"color", "red"s}}};

static const StubGeometryTileFeature oneImage{PropertyMap{{"image_name", "maki-11"s}}};

static const StubGeometryTileFeature emptyTileFeature{PropertyMap{}};

float evaluate(PropertyValue<float> value, float zoom) {
    return value.evaluate(PropertyEvaluator<float>(PropertyEvaluationParameters(zoom), 0));
}

template<typename T>
auto createOverride(expression::type::Type exprType,
                    PossiblyEvaluatedPropertyValue<T> propValue,
                    std::string propName) {
    return std::make_unique<FormatSectionOverride<T>>(std::move(exprType),
                                                      std::move(propValue),
                                                      std::move(propName));
}

TEST(PropertyExpression, Constant) {
    EXPECT_EQ(2.0f, evaluate(PropertyValue<float>(2.0), 0));
    EXPECT_EQ(3.8f, evaluate(PropertyValue<float>(3.8), 0));
    EXPECT_EQ(22.0f, evaluate(PropertyValue<float>(22.0), 0));
    EXPECT_EQ(2.0f, evaluate(PropertyValue<float>(2.0), 4));
    EXPECT_EQ(3.8f, evaluate(PropertyValue<float>(3.8), 4));
    EXPECT_EQ(22.0f, evaluate(PropertyValue<float>(22.0), 4));
    EXPECT_EQ(2.0f, evaluate(PropertyValue<float>(2.0), 22));
    EXPECT_EQ(3.8f, evaluate(PropertyValue<float>(3.8), 22));
    EXPECT_EQ(22.0f, evaluate(PropertyValue<float>(22.0), 22));
}

TEST(PropertyExpression, Expression) {
    PropertyExpression<float> expression(interpolate(linear(), zoom(), 0.0, literal(0.0), 1.0, literal(1.0)));
    EXPECT_EQ(0.0, evaluate(expression, 0.0));
    EXPECT_EQ(0.5, evaluate(expression, 0.5));
}

TEST(PropertyExpression, Defaults) {
    EXPECT_EQ(1.0f, PropertyExpression<float>(number(get("property")), 0.0)
        .evaluate(oneInteger, 2.0f));
    EXPECT_EQ(1.0f, PropertyExpression<float>(number(get("property")), 0.0)
        .evaluate(oneDouble, 2.0f));
    EXPECT_EQ(0.0f, PropertyExpression<float>(number(get("property")), 0.0)
        .evaluate(oneString, 2.0f));
    EXPECT_EQ(2.0f, PropertyExpression<float>(number(get("property")))
        .evaluate(oneString, 2.0f));
}

TEST(PropertyExpression, ZoomInterpolation) {
    EXPECT_EQ(40.0f, PropertyExpression<float>(
        interpolate(linear(), zoom(),
            0.0, interpolate(linear(), number(get("property")), 1.0, literal(24.0)),
            1.5, interpolate(linear(), number(get("property")), 1.0, literal(36.0)),
            3.0, interpolate(linear(), number(get("property")), 1.0, literal(48.0))
        ), 0.0f)
    .evaluate(2.0f, oneInteger, -1.0f)) << "Should interpolate between stops";
    
    EXPECT_EQ(33.0, PropertyExpression<float>(
        interpolate(linear(), zoom(),
            5.0, interpolate(linear(), number(get("property")), 1.0, literal(33.0)),
            10.0, interpolate(linear(), number(get("property")), 1.0, literal(66.0))
        ), 0.0f)
    .evaluate(0.0f, oneInteger, -1.0f)) << "Use first stop output for input values from -inf to first stop";
    
    EXPECT_EQ(66.0, PropertyExpression<float>(
        interpolate(linear(), zoom(),
            0.0, interpolate(linear(), number(get("property")), 1.0, literal(33.0)),
            10.0, interpolate(linear(), number(get("property")), 1.0, literal(66.0))
        ), 0.0f)
    .evaluate(20.0f, oneInteger, -1.0f)) << "Use last stop output for input values from last stop to +inf";

    EXPECT_EQ(66.0f, PropertyExpression<float>(
        interpolate(linear(), zoom(),
            0.0, interpolate(linear(), number(get("property")), 1.0, literal(33.0)),
            10.0, interpolate(linear(), number(get("property")), 1.0, literal(66.0))
        ), 0.0f)
    .evaluate(10.0f, oneInteger, -1.0f)) << "Should interpolate TO the last stop.";
    
    EXPECT_EQ(33.0f, PropertyExpression<float>(
        interpolate(linear(), zoom(),
            0.0, interpolate(linear(), number(get("property")), 1.0, literal(33.0)),
            10.0, interpolate(linear(), number(get("property")), 1.0, literal(66.0))
        ), 0.0f)
    .evaluate(0.0f, oneInteger, -1.0f)) << "Should interpolate TO the first stop";
}

TEST(PropertyExpression, Issue8460) {
    PropertyExpression<float> fn1(
        interpolate(linear(), zoom(),
            15.0, interpolate(linear(), number(get("property")), 1.0, literal(0.0)),
            15.2, interpolate(linear(), number(get("property")), 1.0, literal(600.0))
        ), 0.0f);

    EXPECT_NEAR(  0.0f, fn1.evaluate(15.0f, oneInteger, -1.0f), 0.00);
    EXPECT_NEAR(300.0f, fn1.evaluate(15.1f, oneInteger, -1.0f), 0.01);
    EXPECT_NEAR(600.0f, fn1.evaluate(15.2f, oneInteger, -1.0f), 0.00);
    EXPECT_NEAR(600.0f, fn1.evaluate(16.0f, oneInteger, -1.0f), 0.00);

    PropertyExpression<float> fn2(
        interpolate(linear(), zoom(),
            15.0, interpolate(linear(), number(get("property")), 1.0, literal(0.0)),
            15.2, interpolate(linear(), number(get("property")), 1.0, literal(300.0)),
            18.0, interpolate(linear(), number(get("property")), 1.0, literal(600.0))
        ), 0.0f);

    EXPECT_NEAR(  0.0f, fn2.evaluate(15.0f, oneInteger, -1.0f), 0.00);
    EXPECT_NEAR(150.0f, fn2.evaluate(15.1f, oneInteger, -1.0f), 0.01);
    EXPECT_NEAR(300.0f, fn2.evaluate(15.2f, oneInteger, -1.0f), 0.00);
    EXPECT_NEAR(385.71f, fn2.evaluate(16.0f, oneInteger, -1.0f), 0.01);
    EXPECT_NEAR(600.0f, fn2.evaluate(18.0f, oneInteger, -1.0f), 0.00);
    EXPECT_NEAR(600.0f, fn2.evaluate(19.0f, oneInteger, -1.0f), 0.00);
}

TEST(PropertyExpression, FormatSectionOverride) {
    using Value = expression::Value;
    Value formattedSection =
            std::unordered_map<std::string, Value>{ {"text-color", Value{Color::blue()}} };
    auto ctx = expression::EvaluationContext(&oneDouble).withFormattedSection(&formattedSection);
    PossiblyEvaluatedPropertyValue<Color> constantValueRed(Color::red());
    PossiblyEvaluatedPropertyValue<Color> constantValueGreen(Color::green());
    PossiblyEvaluatedPropertyValue<Color> ddsValueRed(toColor(string(get("color"))));

    // Evaluation test
    {
        auto override1 = createOverride(expression::type::Color, constantValueGreen, "text-color");
        PropertyExpression<Color> propExpr(std::move(override1));
        EXPECT_EQ(Color::green(), propExpr.evaluate(15.0f, oneDouble, Color()));
        EXPECT_EQ(Color::green(), propExpr.evaluate(oneDouble, Color()));
        EXPECT_EQ(Color::blue(), propExpr.evaluate(ctx));

        auto override2 = createOverride(expression::type::Color, ddsValueRed, "text-color");
        PropertyExpression<Color> propExprDDS(std::move(override2));
        EXPECT_EQ(Color::red(), propExprDDS.evaluate(oneColor, Color()));
        EXPECT_EQ(Color::blue(), propExprDDS.evaluate(ctx));
    }

    // Equality test
    {
        auto override1 = createOverride(expression::type::Color, constantValueRed, "text-color");
        auto override2 = createOverride(expression::type::Color, constantValueGreen, "text-color");
        auto override3 = createOverride(expression::type::Color, constantValueGreen, "text-halo-color");
        auto override4 = createOverride(expression::type::Color, ddsValueRed, "text-color");
        EXPECT_TRUE(*override1 != *override2);
        EXPECT_TRUE(*override2 != *override3);
        EXPECT_TRUE(*override1 != *override4);
    }
}

TEST(PropertyExpression, ImageExpression) {
    const std::set<std::string> emptySet;
    const std::set<std::string> availableImages = {"maki-11", "airport-11", "bicycle-15"};

    // evaluation test without available images
    {
        PropertyExpression<expression::Image> propExpr(image(literal("airport-11")));
        auto evaluatedImage = propExpr.evaluate(emptyTileFeature, emptySet, expression::Image());
        EXPECT_FALSE(evaluatedImage.isAvailable());
        EXPECT_EQ(evaluatedImage.id(), "airport-11"s);

        PropertyExpression<expression::Image> ddPropExpr(image(get(literal("image_name"s))));
        evaluatedImage = ddPropExpr.evaluate(oneImage, emptySet, expression::Image());
        EXPECT_FALSE(evaluatedImage.isAvailable());
        EXPECT_EQ(evaluatedImage.id(), "maki-11"s);

        evaluatedImage = ddPropExpr.evaluate(emptyTileFeature, emptySet, expression::Image());
        EXPECT_FALSE(evaluatedImage.isAvailable());
        EXPECT_EQ(evaluatedImage.id(), ""s);
    }

    // evaluation test with available images
    {
        PropertyExpression<expression::Image> propExpr(image(literal("airport-11")));
        auto evaluatedImage = propExpr.evaluate(emptyTileFeature, availableImages, expression::Image());
        EXPECT_TRUE(evaluatedImage.isAvailable());
        EXPECT_EQ(evaluatedImage.id(), "airport-11"s);

        PropertyExpression<expression::Image> ddPropExpr(image(get(literal("image_name"s))));
        evaluatedImage = ddPropExpr.evaluate(oneImage, availableImages, expression::Image());
        EXPECT_TRUE(evaluatedImage.isAvailable());
        EXPECT_EQ(evaluatedImage.id(), "maki-11"s);

        evaluatedImage = ddPropExpr.evaluate(emptyTileFeature, availableImages, expression::Image());
        EXPECT_FALSE(evaluatedImage.isAvailable());
        EXPECT_EQ(evaluatedImage.id(), ""s);
    }

    // evaluation with zoom
    {
        auto expr = step(zoom(), image(literal("airport-11")), 18.0, image(literal("bicycle-15")));
        PropertyExpression<expression::Image> propExpr(std::move(expr));
        auto evaluatedImage = propExpr.evaluate(0.0, emptyTileFeature, availableImages, expression::Image());
        EXPECT_TRUE(evaluatedImage.isAvailable());
        EXPECT_EQ(evaluatedImage.id(), "airport-11"s);
        evaluatedImage = propExpr.evaluate(18.0, emptyTileFeature, availableImages, expression::Image());
        EXPECT_TRUE(evaluatedImage.isAvailable());
        EXPECT_EQ(evaluatedImage.id(), "bicycle-15"s);
    }
}
