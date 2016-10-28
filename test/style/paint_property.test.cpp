#include <mbgl/test/util.hpp>

#include <mbgl/style/paint_property.hpp>

using namespace mbgl;
using namespace mbgl::style;
using namespace std::literals::chrono_literals;

float evaluate(UnevaluatedPaintProperty<PropertyValue<float>>& property, Duration delta = Duration::zero()) {
    PropertyEvaluationParameters parameters {
        0,
        TimePoint::min() + delta,
        ZoomHistory(),
        Duration::zero()
    };

    PropertyEvaluator<float> evaluator {
        parameters,
        0.0f
    };

    return property.evaluate(evaluator, parameters.now);
}

TEST(UnevaluatedPaintProperty, EvaluateDefaultValue) {
    UnevaluatedPaintProperty<PropertyValue<float>> property;
    ASSERT_EQ(0.0f, evaluate(property));
}

TEST(UnevaluatedPaintProperty, EvaluateUntransitionedConstant) {
    UnevaluatedPaintProperty<PropertyValue<float>> property {
        PropertyValue<float>(1.0f),
        UnevaluatedPaintProperty<PropertyValue<float>>(),
        TransitionOptions(),
        TimePoint::min()
    };

    ASSERT_EQ(1.0f, evaluate(property));
}

TEST(UnevaluatedPaintProperty, EvaluateTransitionedConstantWithoutDelay) {
    TransitionOptions transition;
    transition.duration = { 1000ms };

    UnevaluatedPaintProperty<PropertyValue<float>> t0 {
        PropertyValue<float>(0.0f),
        UnevaluatedPaintProperty<PropertyValue<float>>(),
        TransitionOptions(),
        TimePoint::min()
    };

    UnevaluatedPaintProperty<PropertyValue<float>> t1 {
        PropertyValue<float>(1.0f),
        t0,
        transition,
        TimePoint::min()
    };

    ASSERT_FLOAT_EQ(0.0f, evaluate(t1, 0ms));
    ASSERT_FLOAT_EQ(0.823099f, evaluate(t1, 500ms));
    ASSERT_FLOAT_EQ(1.0f, evaluate(t1, 1500ms));
}

TEST(UnevaluatedPaintProperty, EvaluateTransitionedConstantWithDelay) {
    TransitionOptions transition;
    transition.delay = { 1000ms };
    transition.duration = { 1000ms };

    UnevaluatedPaintProperty<PropertyValue<float>> t0 {
        PropertyValue<float>(0.0f),
        UnevaluatedPaintProperty<PropertyValue<float>>(),
        TransitionOptions(),
        TimePoint::min()
    };

    UnevaluatedPaintProperty<PropertyValue<float>> t1 {
        PropertyValue<float>(1.0f),
        t0,
        transition,
        TimePoint::min()
    };

    ASSERT_FLOAT_EQ(0.0f, evaluate(t1, 0ms));
    ASSERT_FLOAT_EQ(0.0f, evaluate(t1, 500ms));
    ASSERT_FLOAT_EQ(0.0f, evaluate(t1, 612ms));
    ASSERT_FLOAT_EQ(0.823099f, evaluate(t1, 1500ms));
    ASSERT_FLOAT_EQ(1.0f, evaluate(t1, 2500ms));
}
