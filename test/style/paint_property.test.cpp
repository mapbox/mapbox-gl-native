#include <mbgl/test/util.hpp>

#include <mbgl/style/paint_property.hpp>
#include <mbgl/renderer/transitioning_property.hpp>

using namespace mbgl;
using namespace mbgl::style;
using namespace std::literals::chrono_literals;

float evaluate(TransitioningProperty<PropertyValue<float>>& property, Duration delta = Duration::zero()) {
    ZoomHistory zoomHistory;
    zoomHistory.update(0, TimePoint::min() + delta);

    PropertyEvaluationParameters parameters {
        zoomHistory,
        TimePoint::min() + delta,
        Duration::zero()
    };

    PropertyEvaluator<float> evaluator {
        parameters,
        0.0f
    };

    return property.evaluate(evaluator, parameters.now);
}

PossiblyEvaluatedPropertyValue<float> evaluate(TransitioningProperty<DataDrivenPropertyValue<float>>& property, Duration delta = Duration::zero()) {
    ZoomHistory zoomHistory;
    zoomHistory.update(0, TimePoint::min() + delta);

    PropertyEvaluationParameters parameters {
        zoomHistory,
        TimePoint::min() + delta,
        Duration::zero()
    };
    
    DataDrivenPropertyEvaluator<float> evaluator {
        parameters,
        0.0f
    };
    
    return property.evaluate(evaluator, parameters.now);
}

TEST(TransitioningProperty, EvaluateDefaultValue) {
    TransitioningProperty<PropertyValue<float>> property;
    ASSERT_EQ(0.0f, evaluate(property));
}

TEST(TransitioningProperty, EvaluateUntransitionedConstant) {
    TransitioningProperty<PropertyValue<float>> property {
        PropertyValue<float>(1.0f),
        TransitioningProperty<PropertyValue<float>>(),
        TransitionOptions(),
        TimePoint::min()
    };

    ASSERT_EQ(1.0f, evaluate(property));
}

TEST(TransitioningProperty, EvaluateTransitionedConstantWithoutDelay) {
    TransitionOptions transition;
    transition.duration = { 1000ms };

    TransitioningProperty<PropertyValue<float>> t0 {
        PropertyValue<float>(0.0f),
        TransitioningProperty<PropertyValue<float>>(),
        TransitionOptions(),
        TimePoint::min()
    };

    TransitioningProperty<PropertyValue<float>> t1 {
        PropertyValue<float>(1.0f),
        t0,
        transition,
        TimePoint::min()
    };

    ASSERT_FLOAT_EQ(0.0f, evaluate(t1, 0ms));
    ASSERT_FLOAT_EQ(0.823099f, evaluate(t1, 500ms));
    ASSERT_FLOAT_EQ(1.0f, evaluate(t1, 1500ms));
}

TEST(TransitioningProperty, EvaluateTransitionedConstantWithDelay) {
    TransitionOptions transition;
    transition.delay = { 1000ms };
    transition.duration = { 1000ms };

    TransitioningProperty<PropertyValue<float>> t0 {
        PropertyValue<float>(0.0f),
        TransitioningProperty<PropertyValue<float>>(),
        TransitionOptions(),
        TimePoint::min()
    };

    TransitioningProperty<PropertyValue<float>> t1 {
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

TEST(TransitioningProperty, EvaluateDataDrivenValue) {
    TransitionOptions transition;
    transition.delay = { 1000ms };
    transition.duration = { 1000ms };
    
    TransitioningProperty<DataDrivenPropertyValue<float>> t0 {
        DataDrivenPropertyValue<float>(0.0f),
        TransitioningProperty<DataDrivenPropertyValue<float>>(),
        TransitionOptions(),
        TimePoint::min()
    };
    
    SourceFunction<float> sourceFunction = {
        "property_name",
        IdentityStops<float>()
    };
    
    TransitioningProperty<DataDrivenPropertyValue<float>> t1 {
        DataDrivenPropertyValue<float>(sourceFunction),
        t0,
        transition,
        TimePoint::min()
    };
    
    ASSERT_TRUE(evaluate(t0, 0ms).isConstant());
    ASSERT_FALSE(evaluate(t1, 0ms).isConstant()) <<
        "A paint property transition to a data-driven evaluates immediately to the final value (see https://github.com/mapbox/mapbox-gl-native/issues/8237).";
}
