#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_geometry_tile_feature.hpp>

#include <mbgl/style/property_expression.hpp>
#include <mbgl/renderer/property_evaluator.hpp>
#include <mbgl/renderer/property_evaluation_parameters.hpp>
#include <mbgl/style/expression/dsl.hpp>
#include <mbgl/style/expression/format_section_override.hpp>

#include <sstream>

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

TEST(PropertyExpression, WithinExpression) {
    CanonicalTileID canonicalTileID(3, 3, 3);

    // geoJSON source must be Polygon.(Currently only supports Polygon)
    static const std::string invalidGeoSource = R"({
      "type": "LineString",
      "coordinates": [[0, 0], [0, 5], [5, 5], [5, 0]]
    })";
    static const std::string validGeoSource = R"data(
    {
      "type": "Polygon",
      "coordinates": [
        [
          [-11.689453125, -9.79567758282973],
          [2.021484375, -10.012129557908128],
          [-15.99609375, -17.392579271057766],
          [-5.9765625, -5.659718554577273],
          [-16.259765625, -3.7327083213358336],
          [-17.75390625, -12.897489183755892],
          [-17.138671875, -21.002471054356715],
          [4.482421875, -16.8886597873816],
          [3.076171875, -7.01366792756663],
          [ -5.9326171875, 0.6591651462894632],
          [-16.1279296875, 1.4939713066293239],
          [-11.689453125, -9.79567758282973]
        ]
      ]
        })data";

    // evaluation test with invalid geojson source
    {
        std::stringstream ss;
        ss << std::string(R"(["within", )") << invalidGeoSource << std::string(R"( ])");
        auto expression = createExpression(ss.str().c_str());
        ASSERT_FALSE(expression);
    }

    // evaluation test with valid geojson source
    std::stringstream ss;
    ss << std::string(R"(["within", )") << validGeoSource << std::string(R"( ])");
    auto expression = createExpression(ss.str().c_str());
    ASSERT_TRUE(expression);
    PropertyExpression<bool> propExpr(std::move(expression));

    // evaluation test with valid geojson source but FeatureType is not Point/LineString (currently only support
    // FeatureType::Point and FeatureType::LineString)
    {
        // testPoly is inside polygon, but will return false
        Polygon<double> testRing{{{-9.228515625, -17.560246503294888},
                                  {-2.4609375, -16.04581345375217},
                                  {-9.228515625, -17.560246503294888}}};
        auto geoPoly = convertGeometry(testRing, canonicalTileID);
        StubGeometryTileFeature polyFeature(FeatureType::Polygon, geoPoly);

        auto evaluatedResult = propExpr.evaluate(EvaluationContext(&polyFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_FALSE(evaluatedResult);
    }
    // evaluation test with valid geojson source and valid linestring features
    {
        // testLine is inside polygon
        LineString<double> testLine0{{-9.228515625, -17.560246503294888}, {-2.4609375, -16.04581345375217}};
        auto geoLine0 = convertGeometry(testLine0, canonicalTileID);
        StubGeometryTileFeature lineFeature0(FeatureType::LineString, geoLine0);

        // testLine is intersecting polygon even though end points are all inside polygon, will return false
        LineString<double> testLine1{{-10.4150390625, -10.082445532162465}, {-8.8275146484375, -9.194292714912638}};
        auto geoLine1 = convertGeometry(testLine1, canonicalTileID);
        StubGeometryTileFeature lineFeature1(FeatureType::LineString, geoLine1);

        auto evaluatedResult =
            propExpr.evaluate(EvaluationContext(&lineFeature0).withCanonicalTileID(&canonicalTileID));
        EXPECT_TRUE(evaluatedResult);
        evaluatedResult = propExpr.evaluate(EvaluationContext(&lineFeature1).withCanonicalTileID(&canonicalTileID));
        EXPECT_FALSE(evaluatedResult);
    }

    // evaluation test with valid geojson source and valid point features
    {
        auto getPointFeature = [&canonicalTileID](const Point<double>& testPoint) -> StubGeometryTileFeature {
            auto geoPoint = convertGeometry(testPoint, canonicalTileID);
            StubGeometryTileFeature pointFeature(FeatureType::Point, geoPoint);
            return pointFeature;
        };

        // check `within` algorithm
        auto pointFeature = getPointFeature(Point<double>(-10.72265625, -7.27529233637217));
        auto evaluatedResult =
            propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_FALSE(evaluatedResult);

        pointFeature = getPointFeature(Point<double>(-7.646484374999999, -12.382928338487396));
        evaluatedResult = propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_FALSE(evaluatedResult);

        pointFeature = getPointFeature(Point<double>(-15.2490234375, -2.986927393334863));
        evaluatedResult = propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_FALSE(evaluatedResult);

        pointFeature = getPointFeature(Point<double>(-10.590820312499998, 2.4601811810210052));
        evaluatedResult = propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_FALSE(evaluatedResult);

        pointFeature = getPointFeature(Point<double>(-3.9990234375, -4.915832801313164));
        evaluatedResult = propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_TRUE(evaluatedResult);

        pointFeature = getPointFeature(Point<double>(-1.1865234375, -16.63619187839765));
        evaluatedResult = propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_TRUE(evaluatedResult);

        pointFeature = getPointFeature(Point<double>(-15.5126953125, -11.73830237143684));
        evaluatedResult = propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_TRUE(evaluatedResult);

        pointFeature = getPointFeature(Point<double>(-5.9765625, -5.659718554577273));
        evaluatedResult = propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_FALSE(evaluatedResult);
    }
}
