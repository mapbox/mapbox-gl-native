#pragma once

#include <mbgl/style/properties.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/data_driven_property_value.hpp>
#include <mbgl/renderer/property_evaluator.hpp>
#include <mbgl/renderer/cross_faded_property_evaluator.hpp>
#include <mbgl/renderer/data_driven_property_evaluator.hpp>

#include <utility>

namespace mbgl {
namespace style {

template <class T>
class PaintProperty {
public:
    using TransitionableType = Transitionable<PropertyValue<T>>;
    using UnevaluatedType = Transitioning<PropertyValue<T>>;
    using EvaluatorType = PropertyEvaluator<T>;
    using PossiblyEvaluatedType = T;
    using Type = T;
    static constexpr bool IsDataDriven = false;
};

template <class T, class A, class U>
class DataDrivenPaintProperty {
public:
    using TransitionableType = Transitionable<DataDrivenPropertyValue<T>>;
    using UnevaluatedType = Transitioning<DataDrivenPropertyValue<T>>;
    using EvaluatorType = DataDrivenPropertyEvaluator<T>;
    using PossiblyEvaluatedType = PossiblyEvaluatedPropertyValue<T>;
    using Type = T;
    static constexpr bool IsDataDriven = true;

    using Attribute = A;
    using Uniform = U;
};

template <class T>
class CrossFadedPaintProperty {
public:
    using TransitionableType = Transitionable<PropertyValue<T>>;
    using UnevaluatedType = Transitioning<PropertyValue<T>>;
    using EvaluatorType = CrossFadedPropertyEvaluator<T>;
    using PossiblyEvaluatedType = Faded<T>;
    using Type = T;
    static constexpr bool IsDataDriven = false;
};

} // namespace style
} // namespace mbgl
