#include <mbgl/test/util.hpp>

#include <mbgl/style/paint_property.hpp>

using namespace mbgl;
using namespace mbgl::style;
using namespace std::literals::chrono_literals;

TEST(UnevaluatedPaintProperty, EvaluateDefaultValue) {
    UnevaluatedPaintProperty<float, PropertyEvaluator<float>> property;
    ASSERT_EQ(0.0f, property.evaluate(PropertyEvaluationParameters(0), 0.0f));
}

TEST(UnevaluatedPaintProperty, EvaluateUntransitionedConstant) {
    UnevaluatedPaintProperty<float, PropertyEvaluator<float>> property {
        PropertyValue<float>(1.0f),
        UnevaluatedPaintProperty<float, PropertyEvaluator<float>>(),
        TransitionOptions(),
        TimePoint::min()
    };

    ASSERT_EQ(1.0f, property.evaluate(PropertyEvaluationParameters(0), 0.0f));
}

TEST(UnevaluatedPaintProperty, EvaluateTransitionedConstantWithoutDelay) {
    TransitionOptions transition;
    transition.duration = { 1000ms };

    UnevaluatedPaintProperty<float, PropertyEvaluator<float>> t0 {
        PropertyValue<float>(0.0f),
        UnevaluatedPaintProperty<float, PropertyEvaluator<float>>(),
        TransitionOptions(),
        TimePoint::min()
    };

    UnevaluatedPaintProperty<float, PropertyEvaluator<float>> t1 {
        PropertyValue<float>(1.0f),
        t0,
        transition,
        TimePoint::min()
    };

    auto evaluate = [&] (Duration delta) {
        PropertyEvaluationParameters parameters {
            0,
            TimePoint::min() + delta,
            ZoomHistory(),
            Duration::zero()
        };

        return t1.evaluate(parameters, 0.0f);
    };

    ASSERT_FLOAT_EQ(0.0f, evaluate(0ms));
    ASSERT_FLOAT_EQ(0.823099f, evaluate(500ms));
    ASSERT_FLOAT_EQ(1.0f, evaluate(1500ms));
}

TEST(UnevaluatedPaintProperty, EvaluateTransitionedConstantWithDelay) {
    TransitionOptions transition;
    transition.delay = { 1000ms };
    transition.duration = { 1000ms };

    UnevaluatedPaintProperty<float, PropertyEvaluator<float>> t0 {
        PropertyValue<float>(0.0f),
        UnevaluatedPaintProperty<float, PropertyEvaluator<float>>(),
        TransitionOptions(),
        TimePoint::min()
    };

    UnevaluatedPaintProperty<float, PropertyEvaluator<float>> t1 {
        PropertyValue<float>(1.0f),
        t0,
        transition,
        TimePoint::min()
    };

    auto evaluate = [&] (Duration delta) {
        PropertyEvaluationParameters parameters {
            0,
            TimePoint::min() + delta,
            ZoomHistory(),
            Duration::zero()
        };

        return t1.evaluate(parameters, 0.0f);
    };

    ASSERT_FLOAT_EQ(0.0f, evaluate(0ms));
    ASSERT_FLOAT_EQ(0.0f, evaluate(500ms));
    ASSERT_FLOAT_EQ(0.0f, evaluate(612ms));
    ASSERT_FLOAT_EQ(0.823099f, evaluate(1500ms));
    ASSERT_FLOAT_EQ(1.0f, evaluate(2500ms));
}
