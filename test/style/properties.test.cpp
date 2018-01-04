#include <mbgl/test/util.hpp>

#include <mbgl/style/properties.hpp>
#include <mbgl/renderer/property_evaluator.hpp>
#include <mbgl/renderer/data_driven_property_evaluator.hpp>

using namespace mbgl;
using namespace mbgl::style;
using namespace std::literals::chrono_literals;

float evaluate(Transitioning<PropertyValue<float>>& property, Duration delta = Duration::zero()) {
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

PossiblyEvaluatedPropertyValue<float> evaluate(Transitioning<DataDrivenPropertyValue<float>>& property, Duration delta = Duration::zero()) {
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

TEST(TransitioningPropertyValue, EvaluateDefaultValue) {
    Transitioning<PropertyValue<float>> property;
    ASSERT_EQ(0.0f, evaluate(property));
}

TEST(TransitioningPropertyValue, EvaluateUntransitionedConstant) {
    Transitioning<PropertyValue<float>> property {
        PropertyValue<float>(1.0f),
        Transitioning<PropertyValue<float>>(),
        TransitionOptions(),
        TimePoint::min()
    };

    ASSERT_EQ(1.0f, evaluate(property));
}

TEST(TransitioningPropertyValue, EvaluateTransitionedConstantWithoutDelay) {
    TransitionOptions transition;
    transition.duration = { 1000ms };

    Transitioning<PropertyValue<float>> t0 {
        PropertyValue<float>(0.0f),
        Transitioning<PropertyValue<float>>(),
        TransitionOptions(),
        TimePoint::min()
    };

    Transitioning<PropertyValue<float>> t1 {
        PropertyValue<float>(1.0f),
        t0,
        transition,
        TimePoint::min()
    };

    ASSERT_FLOAT_EQ(0.0f, evaluate(t1, 0ms));
    ASSERT_FLOAT_EQ(0.823099f, evaluate(t1, 500ms));
    ASSERT_FLOAT_EQ(1.0f, evaluate(t1, 1500ms));
}

TEST(TransitioningPropertyValue, EvaluateTransitionedConstantWithDelay) {
    TransitionOptions transition;
    transition.delay = { 1000ms };
    transition.duration = { 1000ms };

    Transitioning<PropertyValue<float>> t0 {
        PropertyValue<float>(0.0f),
        Transitioning<PropertyValue<float>>(),
        TransitionOptions(),
        TimePoint::min()
    };

    Transitioning<PropertyValue<float>> t1 {
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

TEST(TransitioningDataDrivenPropertyValue, Evaluate) {
    TransitionOptions transition;
    transition.delay = { 1000ms };
    transition.duration = { 1000ms };
    
    Transitioning<DataDrivenPropertyValue<float>> t0 {
        DataDrivenPropertyValue<float>(0.0f),
        Transitioning<DataDrivenPropertyValue<float>>(),
        TransitionOptions(),
        TimePoint::min()
    };
    
    SourceFunction<float> sourceFunction = {
        "property_name",
        IdentityStops<float>()
    };
    
    Transitioning<DataDrivenPropertyValue<float>> t1 {
        DataDrivenPropertyValue<float>(sourceFunction),
        t0,
        transition,
        TimePoint::min()
    };
    
    ASSERT_TRUE(evaluate(t0, 0ms).isConstant());
    ASSERT_FALSE(evaluate(t1, 0ms).isConstant()) <<
        "A paint property transition to a data-driven evaluates immediately to the final value (see https://github.com/mapbox/mapbox-gl-native/issues/8237).";
}
