#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_geometry_tile_feature.hpp>

#include <mbgl/style/property_expression.hpp>
#include <mbgl/renderer/property_evaluator.hpp>
#include <mbgl/renderer/property_evaluation_parameters.hpp>
#include <mbgl/style/expression/dsl.hpp>
#include <mbgl/style/expression/format_section_override.hpp>

#include <mbgl/util/geojson.hpp>
#include <mbgl/util/io.hpp>

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

        // On the boundary
        pointFeature = getPointFeature(Point<double>(3.076171875, -7.01366792756663));
        evaluatedResult = propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_FALSE(evaluatedResult);

        pointFeature = getPointFeature(Point<double>(-11.689453125, -9.79567758282973));
        evaluatedResult = propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_FALSE(evaluatedResult);

        pointFeature = getPointFeature(Point<double>(2.021484375, -10.012129557908128));
        evaluatedResult = propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_FALSE(evaluatedResult);

        pointFeature = getPointFeature(Point<double>(-15.99609375, -17.392579271057766));
        evaluatedResult = propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_FALSE(evaluatedResult);

        pointFeature = getPointFeature(Point<double>(-5.9765625, -5.659718554577273));
        evaluatedResult = propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_FALSE(evaluatedResult);

        pointFeature = getPointFeature(Point<double>(-16.259765625, -3.7327083213358336));
        evaluatedResult = propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_FALSE(evaluatedResult);

        pointFeature = getPointFeature(Point<double>(-17.75390625, -12.897489183755892));
        evaluatedResult = propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_FALSE(evaluatedResult);

        pointFeature = getPointFeature(Point<double>(-17.138671875, -21.002471054356715));
        evaluatedResult = propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_FALSE(evaluatedResult);

        pointFeature = getPointFeature(Point<double>(4.482421875, -16.8886597873816));
        evaluatedResult = propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_FALSE(evaluatedResult);

        pointFeature = getPointFeature(Point<double>(3.076171875, -7.01366792756663));
        evaluatedResult = propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_FALSE(evaluatedResult);

        pointFeature = getPointFeature(Point<double>(-5.9326171875, 0.6591651462894632));
        evaluatedResult = propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_FALSE(evaluatedResult);

        pointFeature = getPointFeature(Point<double>(-16.1279296875, 1.4939713066293239));
        evaluatedResult = propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_FALSE(evaluatedResult);

        pointFeature = getPointFeature(Point<double>(-11.689453125, -9.79567758282973));
        evaluatedResult = propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_FALSE(evaluatedResult);
    }
}

TEST(PropertyExpression, WithinExpressionAntiMeridian) {
    CanonicalTileID canonicalTileID(0, 0, 0);

    // evaluation test with line geometries
    {
        const std::string polygon1 = R"data(
       {
         "type": "Polygon",
         "coordinates": [[[-190, 0], [-178, 0], [-178, 10], [-190, 10], [-190, 0]]]
       })data";
        // evaluation test with valid geojson source
        std::stringstream ss;
        ss << std::string(R"(["within", )") << polygon1 << std::string(R"( ])");
        auto expression = createExpression(ss.str().c_str());
        ASSERT_TRUE(expression);
        PropertyExpression<bool> propExpr(std::move(expression));

        LineString<double> testLine0{{-183, 5}, {-179, 1}};
        auto geoLine0 = convertGeometry(testLine0, canonicalTileID);
        StubGeometryTileFeature lineFeature0(FeatureType::LineString, geoLine0);

        LineString<double> testLine1{{183, 5}, {181, 1}};
        auto geoLine1 = convertGeometry(testLine1, canonicalTileID);
        StubGeometryTileFeature lineFeature1(FeatureType::LineString, geoLine1);

        auto evaluatedResult =
            propExpr.evaluate(EvaluationContext(&lineFeature0).withCanonicalTileID(&canonicalTileID));
        EXPECT_TRUE(evaluatedResult);
        evaluatedResult = propExpr.evaluate(EvaluationContext(&lineFeature1).withCanonicalTileID(&canonicalTileID));
        EXPECT_FALSE(evaluatedResult);
    }

    // evaluation test with point geometries
    {
        static const std::string polygon2 = R"data(
       {
         "type": "Polygon",
         "coordinates":  [[[-185.0, 60.0], [-175.0, 60.0], [-175.0, 65.0], [-185.0, 65.0], [-185.0, 60.0]]]
       })data";
        // evaluation test with valid geojson source
        std::stringstream ss;
        ss << std::string(R"(["within", )") << polygon2 << std::string(R"( ])");
        auto expression = createExpression(ss.str().c_str());
        ASSERT_TRUE(expression);
        PropertyExpression<bool> propExpr(std::move(expression));

        auto getPointFeature = [&canonicalTileID](const Point<double>& testPoint) -> StubGeometryTileFeature {
            auto geoPoint = convertGeometry(testPoint, canonicalTileID);
            StubGeometryTileFeature pointFeature(FeatureType::Point, geoPoint);
            return pointFeature;
        };

        // check `within` algorithm
        auto pointFeature = getPointFeature(Point<double>(177, 62.5));
        auto evaluatedResult =
            propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_TRUE(evaluatedResult);

        pointFeature = getPointFeature(Point<double>(180, 62.5));
        evaluatedResult = propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_TRUE(evaluatedResult);

        pointFeature = getPointFeature(Point<double>(-180, 62.5));
        evaluatedResult = propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_TRUE(evaluatedResult);

        pointFeature = getPointFeature(Point<double>(-190, 62.5));
        evaluatedResult = propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID));
        EXPECT_FALSE(evaluatedResult);
    }
}

TEST(PropertyExpression, DistanceExpression) {
    static const double invalidResult = std::numeric_limits<double>::quiet_NaN();
    static const CanonicalTileID canonicalTileID(15, 18653, 9484);

    // Parsing error with invalid geojson source
    {
        // geoJSON source must be Point or LineString.
        const std::string invalidGeoSource = R"({
            "type": "Unknown",
            "coordinates": [[[0, 0], [0, 5], [5, 5], [5, 0], [0, 0]]]
            })";
        std::stringstream ss;
        ss << std::string(R"(["distance", )") << invalidGeoSource << std::string(R"( ])");
        auto expression = createExpression(ss.str().c_str());
        ASSERT_FALSE(expression);
    }

    // Parsing error with extra arguments
    {
        const std::string invalidGeoSource = R"({
            "type": "Point",
            "coordinates": [0, 0],
            })";
        std::stringstream ss;
        ss << std::string(R"(["distance", )") << invalidGeoSource << std::string(R"(, "ExtraArgument"])");
        auto expression = createExpression(ss.str().c_str());
        ASSERT_FALSE(expression);
    }

    // Evaluation test with Point to Point distance by using different units
    {
        const std::string pointGeoSource = R"({
             "type": "Point",
             "coordinates":
               [
                   24.938492774963375,
                   60.16980226227959
               ]
              })";

        Point<double> featurePoint{24.931159615516663, 60.16733387258923};
        auto geoPoint = convertGeometry(featurePoint, canonicalTileID);
        StubGeometryTileFeature pointFeature(FeatureType::Point, geoPoint);

        std::stringstream ss;

        // Default Unit: meters
        ss << std::string(R"(["distance", )") << pointGeoSource << std::string(R"( ])");
        auto expression = createExpression(ss.str().c_str());
        ASSERT_TRUE(expression);
        PropertyExpression<double> propExpr(std::move(expression));

        auto evaluatedResult =
            propExpr.evaluate(EvaluationContext(&pointFeature).withCanonicalTileID(&canonicalTileID), invalidResult);
        EXPECT_NEAR(491.307, evaluatedResult, 0.01);

    }

    // Evaluation test with Point to MultiPoint distance
    {
        const std::string pointGeoSource = R"({
              "type": "Point",
              "coordinates":
                [
                    24.938492774963375,
                    60.16980226227959
                ]
               })";
        std::stringstream ss;
        ss << std::string(R"(["distance", )") << pointGeoSource << std::string(R"( ])");
        auto expression = createExpression(ss.str().c_str());
        ASSERT_TRUE(expression);
        PropertyExpression<double> propExpr(std::move(expression));

        MultiPoint<double> featurePoint{{24.931159615516663, 60.16733387258923},
                                        {24.936089515686035, 60.169033745694826}};
        auto geoPoint = convertGeometry(featurePoint, canonicalTileID);
        StubGeometryTileFeature feature(FeatureType::Point, geoPoint);

        auto evaluatedResult =
            propExpr.evaluate(EvaluationContext(&feature).withCanonicalTileID(&canonicalTileID), invalidResult);
        EXPECT_NEAR(158.541, evaluatedResult, 0.01);
    }

    // Evaluation test with Point to LineString distance
    {
        const std::string pointGeoSource = R"({
             "type": "Point",
             "coordinates":
               [
                   24.938492774963375,
                   60.16980226227959
               ]
              })";
        std::stringstream ss;
        ss << std::string(R"(["distance", )") << pointGeoSource << std::string(R"( ])");
        auto expression = createExpression(ss.str().c_str());
        ASSERT_TRUE(expression);
        PropertyExpression<double> propExpr(std::move(expression));

        LineString<double> line{{24.93621826171875, 60.17131789507754},
                                {24.941453933715817, 60.170229208170014},
                                {24.941110610961914, 60.16850004304534}};
        auto geoLine = convertGeometry(line, canonicalTileID);
        StubGeometryTileFeature lineFeature(FeatureType::LineString, geoLine);

        auto evaluatedResult =
            propExpr.evaluate(EvaluationContext(&lineFeature).withCanonicalTileID(&canonicalTileID), invalidResult);
        EXPECT_NEAR(107.211, evaluatedResult, 0.01);
    }

    // Evaluation test with Point to MultiLineString distance
    {
        const std::string pointGeoSource = R"({
             "type": "Point",
             "coordinates":
               [
                   24.938492774963375,
                   60.16980226227959
               ]
              })";
        std::stringstream ss;
        ss << std::string(R"(["distance", )") << pointGeoSource << std::string(R"( ])");
        auto expression = createExpression(ss.str().c_str());
        ASSERT_TRUE(expression);
        PropertyExpression<double> propExpr(std::move(expression));

        MultiLineString<double> lines{
            {{24.93621826171875, 60.17131789507754},
             {24.941453933715817, 60.170229208170014},
             {24.941110610961914, 60.16850004304534}},
            {{24.937773942947388, 60.16940199546824}, {24.933900833129883, 60.17007977773949}}};
        auto geoLine = convertGeometry(lines, canonicalTileID);
        StubGeometryTileFeature lineFeature(FeatureType::LineString, geoLine);

        auto evaluatedResult =
            propExpr.evaluate(EvaluationContext(&lineFeature).withCanonicalTileID(&canonicalTileID), invalidResult);
        EXPECT_NEAR(59.845, evaluatedResult, 0.01);
    }

    const auto getFeature = [](const std::string &name, FeatureType type, const CanonicalTileID &canonical) {
        const auto geometryInput = mbgl::util::read_file(std::string("test/fixtures/geometry_data/" + name));
        mapbox::geojson::geojson geojson{mapbox::geojson::parse(geometryInput)};
        const auto &geometry = geojson.match(
            [](const mapbox::geometry::geometry<double> &geometrySet) { return mbgl::Feature(geometrySet).geometry; },
            [](const mapbox::feature::feature<double> &feature) { return mbgl::Feature(feature).geometry; },
            [](const mapbox::feature::feature_collection<double> &features) {
                return mbgl::Feature(features.front()).geometry;
            },
            [](const auto &) { return mapbox::geometry::empty(); });
        return StubGeometryTileFeature(type, convertGeometry(geometry, canonical));
    };

    // Evaluation test with MultiPoint to MultiPoint distance
    {
        const auto multiPoints1 = mbgl::util::read_file("test/fixtures/geometry_data/multi_point_1.geojson");
        const auto multiPointsFeature = getFeature("multi_point_2.geojson", FeatureType::Point, canonicalTileID);
        std::stringstream ss;
        ss << std::string(R"(["distance", )") << multiPoints1 << std::string(R"( ])");
        auto expression = createExpression(ss.str().c_str());
        ASSERT_TRUE(expression);
        PropertyExpression<double> propExpr(std::move(expression));

        auto evaluatedResult = propExpr.evaluate(
            EvaluationContext(&multiPointsFeature).withCanonicalTileID(&canonicalTileID), invalidResult);
        EXPECT_NEAR(283.60, evaluatedResult, 0.1);
    }

    // Evaluation test with MultiPoint to LineString distance
    {
        const auto multiPoints1 = mbgl::util::read_file("test/fixtures/geometry_data/multi_point_1.geojson");
        const auto multiLineFeature = getFeature("line_string_2.geojson", FeatureType::LineString, canonicalTileID);
        std::stringstream ss;
        ss << std::string(R"(["distance", )") << multiPoints1 << std::string(R"( ])");
        auto expression = createExpression(ss.str().c_str());
        ASSERT_TRUE(expression);
        PropertyExpression<double> propExpr(std::move(expression));

        auto evaluatedResult = propExpr.evaluate(
            EvaluationContext(&multiLineFeature).withCanonicalTileID(&canonicalTileID), invalidResult);
        EXPECT_NEAR(258.691, evaluatedResult, 0.01);

        // ----------------- switch feature and argument -----------------
        const auto multiLine2 = mbgl::util::read_file("test/fixtures/geometry_data/line_string_2.geojson");
        const auto multiPointsFeature = getFeature("multi_point_1.geojson", FeatureType::Point, canonicalTileID);
        std::stringstream ss1;
        ss1 << std::string(R"(["distance", )") << multiLine2 << std::string(R"( ])");
        auto expression1 = createExpression(ss1.str().c_str());
        ASSERT_TRUE(expression1);
        PropertyExpression<double> propExpr1(std::move(expression1));

        auto evaluatedResult1 = propExpr1.evaluate(
            EvaluationContext(&multiPointsFeature).withCanonicalTileID(&canonicalTileID), invalidResult);
        EXPECT_NEAR(258.691, evaluatedResult1, 0.01);
    }

    // Evaluation test with LineString to LineString distance
    {
        const auto lineString1 = mbgl::util::read_file("test/fixtures/geometry_data/line_string_1.geojson");
        const auto lineFeature = getFeature("line_string_2.geojson", FeatureType::LineString, canonicalTileID);
        std::stringstream ss;
        ss << std::string(R"(["distance", )") << lineString1 << std::string(R"( ])");
        auto expression = createExpression(ss.str().c_str());
        ASSERT_TRUE(expression);
        PropertyExpression<double> propExpr(std::move(expression));

        auto evaluatedResult =
            propExpr.evaluate(EvaluationContext(&lineFeature).withCanonicalTileID(&canonicalTileID), invalidResult);
        EXPECT_NEAR(180.408, evaluatedResult, 0.01);
    }

    // Evaluation test with MultiPoints to MutltiLineString distance
    {
        const auto multiPoints = mbgl::util::read_file("test/fixtures/geometry_data/multi_point_2.geojson");
        const auto multiLineFeature =
            getFeature("multi_line_string_1.geojson", FeatureType::LineString, canonicalTileID);
        std::stringstream ss;
        ss << std::string(R"(["distance", )") << multiPoints << std::string(R"( ])");
        auto expression = createExpression(ss.str().c_str());
        ASSERT_TRUE(expression);
        PropertyExpression<double> propExpr(std::move(expression));

        auto evaluatedResult = propExpr.evaluate(
            EvaluationContext(&multiLineFeature).withCanonicalTileID(&canonicalTileID), invalidResult);
        EXPECT_NEAR(937.632, evaluatedResult, 0.01);
    }

    // Evaluation test with LineString to MutltiLineString distance
    {
        const auto LineString1 = mbgl::util::read_file("test/fixtures/geometry_data/line_string_1.geojson");
        const auto multiLineFeature =
            getFeature("multi_line_string_2.geojson", FeatureType::LineString, canonicalTileID);
        std::stringstream ss;
        ss << std::string(R"(["distance", )") << LineString1 << std::string(R"( ])");
        auto expression = createExpression(ss.str().c_str());
        ASSERT_TRUE(expression);
        PropertyExpression<double> propExpr(std::move(expression));

        auto evaluatedResult = propExpr.evaluate(
            EvaluationContext(&multiLineFeature).withCanonicalTileID(&canonicalTileID), invalidResult);
        EXPECT_NEAR(17.511, evaluatedResult, 0.01);
    }

    // Evaluation test with MultiLineString to MutltiLineString distance
    {
        const auto multiLineString1 = mbgl::util::read_file("test/fixtures/geometry_data/multi_line_string_1.geojson");
        const auto multiLineFeature =
            getFeature("multi_line_string_2.geojson", FeatureType::LineString, canonicalTileID);
        std::stringstream ss;
        ss << std::string(R"(["distance", )") << multiLineString1 << std::string(R"( ])");
        auto expression = createExpression(ss.str().c_str());
        ASSERT_TRUE(expression);
        PropertyExpression<double> propExpr(std::move(expression));

        auto evaluatedResult = propExpr.evaluate(
            EvaluationContext(&multiLineFeature).withCanonicalTileID(&canonicalTileID), invalidResult);
        EXPECT_NEAR(384.109, evaluatedResult, 0.01);
    }

    // Evaluation test with MultiPoint to MultiPolygon distance
    {
        const auto multiPolygon = mbgl::util::read_file("test/fixtures/geometry_data/multi_polygon_1.geojson");
        std::stringstream ss;
        ss << std::string(R"(["distance", )") << multiPolygon << std::string(R"( ])");
        auto expression = createExpression(ss.str().c_str());
        ASSERT_TRUE(expression);
        PropertyExpression<double> propExpr(std::move(expression));

        const auto multiPointFeature1 = getFeature("multi_point_3.geojson", FeatureType::Point, canonicalTileID);
        auto evaluatedResult = propExpr.evaluate(
            EvaluationContext(&multiPointFeature1).withCanonicalTileID(&canonicalTileID), invalidResult);
        EXPECT_NEAR(27.917, evaluatedResult, 0.01);

        const auto multiPointFeature2 = getFeature("multi_point_2.geojson", FeatureType::Point, canonicalTileID);
        evaluatedResult = propExpr.evaluate(
            EvaluationContext(&multiPointFeature2).withCanonicalTileID(&canonicalTileID), invalidResult);
        EXPECT_NEAR(0.0, evaluatedResult, 0.01);
    }

    // Evaluation test with MultiLine to MultiPolygon distance
    {
        const auto multiPolygon = mbgl::util::read_file("test/fixtures/geometry_data/multi_polygon_1.geojson");
        std::stringstream ss;
        ss << std::string(R"(["distance", )") << multiPolygon << std::string(R"( ])");
        auto expression = createExpression(ss.str().c_str());
        ASSERT_TRUE(expression);
        PropertyExpression<double> propExpr(std::move(expression));

        const auto multiLineFeature1 =
            getFeature("multi_line_string_1.geojson", FeatureType::LineString, canonicalTileID);
        auto evaluatedResult = propExpr.evaluate(
            EvaluationContext(&multiLineFeature1).withCanonicalTileID(&canonicalTileID), invalidResult);
        EXPECT_NEAR(193.450, evaluatedResult, 0.01);

        const auto multiLineFeature2 =
            getFeature("multi_line_string_2.geojson", FeatureType::LineString, canonicalTileID);
        evaluatedResult = propExpr.evaluate(EvaluationContext(&multiLineFeature2).withCanonicalTileID(&canonicalTileID),
                                            invalidResult);
        EXPECT_NEAR(0.0, evaluatedResult, 0.01);
    }

    // Evaluation test with Polygon to MultiPolygon distance
    {
        const auto multiPolygon = mbgl::util::read_file("test/fixtures/geometry_data/multi_polygon_1.geojson");
        std::stringstream ss;
        ss << std::string(R"(["distance", )") << multiPolygon << std::string(R"( ])");
        auto expression = createExpression(ss.str().c_str());
        ASSERT_TRUE(expression);
        PropertyExpression<double> propExpr(std::move(expression));

        Polygon<double> polygon{{{24.94171142578125, 60.16777419352904},
                                 {24.948577880859375, 60.16777419352904},
                                 {24.948577880859375, 60.170122472217564},
                                 {24.94171142578125, 60.170122472217564},
                                 {24.94171142578125, 60.16777419352904}}};
        auto geoPolygon = convertGeometry(polygon, canonicalTileID);
        StubGeometryTileFeature polygonFeature(FeatureType::Polygon, geoPolygon);

        auto evaluatedResult =
            propExpr.evaluate(EvaluationContext(&polygonFeature).withCanonicalTileID(&canonicalTileID), invalidResult);
        EXPECT_NEAR(203.876, evaluatedResult, 0.01);
    }

    // Evaluation test with MultiPolygon to MultiPolygon distance
    {
        const auto multiPolygon = mbgl::util::read_file("test/fixtures/geometry_data/multi_polygon_1.geojson");
        std::stringstream ss;
        ss << std::string(R"(["distance", )") << multiPolygon << std::string(R"( ])");
        auto expression = createExpression(ss.str().c_str());
        ASSERT_TRUE(expression);
        PropertyExpression<double> propExpr(std::move(expression));

        const auto multiPolygonFeature = getFeature("multi_polygon_2.geojson", FeatureType::Polygon, canonicalTileID);
        auto evaluatedResult = propExpr.evaluate(
            EvaluationContext(&multiPolygonFeature).withCanonicalTileID(&canonicalTileID), invalidResult);
        EXPECT_NEAR(41.632, evaluatedResult, 0.01);
    }

    // Evaluation test with LineString/Polygon to Polygon distance with special cases
    {
        const std::string multiPolygon = R"({
                    "type": "Polygon",
                          "coordinates": [[[24.937891960144043, 60.16730451765011],
                                           [24.94892120361328, 60.16730451765011],
                                           [24.94892120361328, 60.17010112498546],
                                           [24.937891960144043, 60.17010112498546],
                                           [24.937891960144043, 60.16730451765011]]]})";
        std::stringstream ss;
        ss << std::string(R"(["distance", )") << multiPolygon << std::string(R"( ])");
        auto expression = createExpression(ss.str().c_str());
        ASSERT_TRUE(expression);
        PropertyExpression<double> propExpr(std::move(expression));

        // polygon1 is fully inside polygon
        Polygon<double> polygon1{{{24.941797256469727, 60.16850004304534},
                                  {24.94235515594482, 60.16850004304534},
                                  {24.94235515594482, 60.169439353910285},
                                  {24.941797256469727, 60.169439353910285},
                                  {24.941797256469727, 60.16850004304534}}};
        auto geoPolygon1 = convertGeometry(polygon1, canonicalTileID);
        StubGeometryTileFeature polygonFeature1(FeatureType::Polygon, geoPolygon1);

        auto evaluatedResult =
            propExpr.evaluate(EvaluationContext(&polygonFeature1).withCanonicalTileID(&canonicalTileID), invalidResult);
        EXPECT_NEAR(0.0, evaluatedResult, 0.01);

        // polygon2's line is intersecting polygon, but no point is inside polygon
        Polygon<double> polygon2{{{24.943385124206543, 60.166066249059554},
                                  {24.94596004486084, 60.166066249059554},
                                  {24.94596004486084, 60.1714246271462},
                                  {24.943385124206543, 60.1714246271462},
                                  {24.943385124206543, 60.166066249059554}}};
        auto geoPolygon2 = convertGeometry(polygon2, canonicalTileID);
        StubGeometryTileFeature polygonFeature2(FeatureType::Polygon, geoPolygon2);

        evaluatedResult =
            propExpr.evaluate(EvaluationContext(&polygonFeature2).withCanonicalTileID(&canonicalTileID), invalidResult);
        EXPECT_NEAR(0.0, evaluatedResult, 0.01);

        // one of polygon3's point is inside polygon
        Polygon<double> polygon3{{{24.94797706604004, 60.1694607015724},
                                  {24.952354431152344, 60.1694607015724},
                                  {24.952354431152344, 60.17082692309542},
                                  {24.94797706604004, 60.17082692309542},
                                  {24.94797706604004, 60.1694607015724}}};
        auto geoPolygon3 = convertGeometry(polygon3, canonicalTileID);
        StubGeometryTileFeature polygonFeature3(FeatureType::Polygon, geoPolygon3);

        evaluatedResult =
            propExpr.evaluate(EvaluationContext(&polygonFeature3).withCanonicalTileID(&canonicalTileID), invalidResult);
        EXPECT_NEAR(0.0, evaluatedResult, 0.01);

        // lineString1 is within polygon
        LineString<double> lineString1{{24.947097301483154, 60.16921520262075}, {24.947311878204346, 60.1680197032483}};
        auto geoLineString1 = convertGeometry(lineString1, canonicalTileID);
        StubGeometryTileFeature lineFeature1(FeatureType::LineString, geoLineString1);

        evaluatedResult =
            propExpr.evaluate(EvaluationContext(&lineFeature1).withCanonicalTileID(&canonicalTileID), invalidResult);
        EXPECT_NEAR(0.0, evaluatedResult, 0.01);

        // lineString2 is intersecting the polygon but its points are outside the polygon
        LineString<double> lineString2{{24.93999481201172, 60.17116846959888}, {24.940037727355957, 60.16542574699484}};
        auto geoLineString2 = convertGeometry(lineString2, canonicalTileID);
        StubGeometryTileFeature lineFeature2(FeatureType::LineString, geoLineString2);

        evaluatedResult =
            propExpr.evaluate(EvaluationContext(&lineFeature2).withCanonicalTileID(&canonicalTileID), invalidResult);
        EXPECT_NEAR(0.0, evaluatedResult, 0.01);
    }
}
