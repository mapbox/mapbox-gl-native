#include <iostream>
#include <mbgl/test/util.hpp>

#include <mbgl/renderer/property_evaluator.hpp>
#include <mbgl/renderer/property_evaluation_parameters.hpp>
#include <mbgl/style/expression/dsl.hpp>

using namespace mbgl;
using namespace mbgl::style;
using namespace mbgl::style::expression::dsl;

float evaluate(PropertyValue<float> value, float zoom) {
    return value.evaluate(PropertyEvaluator<float>(PropertyEvaluationParameters(zoom), 0));
}

TEST(CameraFunction, Constant) {
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

TEST(CameraFunction, Expression) {
    CameraFunction<float> function(interpolate(linear(), zoom(), 0.0, literal(0.0), 1.0, literal(1.0)));
    EXPECT_EQ(0.0, evaluate(function, 0.0));
    EXPECT_EQ(0.5, evaluate(function, 0.5));
}
