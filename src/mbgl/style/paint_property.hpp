#pragma once

#include <mbgl/style/color_ramp_property_value.hpp>
#include <mbgl/style/properties.hpp>
#include <mbgl/style/property_value.hpp>
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
    static constexpr bool IsOverridable = false;
};

template <class T, class A, class U, bool isOverridable = false>
class DataDrivenPaintProperty {
public:
    using TransitionableType = Transitionable<PropertyValue<T>>;
    using UnevaluatedType = Transitioning<PropertyValue<T>>;
    using EvaluatorType = DataDrivenPropertyEvaluator<T>;
    using PossiblyEvaluatedType = PossiblyEvaluatedPropertyValue<T>;
    using Type = T;
    static constexpr bool IsDataDriven = true;
    static constexpr bool IsOverridable = isOverridable;

    using Attribute = A;
    using AttributeList = TypeList<A>;
    using Uniform = U;
    using UniformList = TypeList<U>;
};

template <class T, class A1, class U1, class A2, class U2>
class CrossFadedDataDrivenPaintProperty {
public:
    using TransitionableType = Transitionable<PropertyValue<T>>;
    using UnevaluatedType = Transitioning<PropertyValue<T>>;
    using EvaluatorType = DataDrivenPropertyEvaluator<Faded<T>>;
    using PossiblyEvaluatedType = PossiblyEvaluatedPropertyValue<Faded<T>>;
    using Type = T;
    static constexpr bool IsDataDriven = true;
    static constexpr bool IsOverridable = false;

    using Attribute = A1;
    using AttributeList = TypeList<A1, A2>;
    using Uniform = U1;
    using UniformList = TypeList<U1, U2>;
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
    static constexpr bool IsOverridable = false;
};

/*
 * Special-case paint property traits for heatmap-color and line-gradient,
 * needed because these values do not fit into the
 * Undefined | Value | {Camera,Source,Composite}Function taxonomy that applies
 * to all other paint properties.
 *
 * These traits are provided here--despite the fact that color ramps
 * is not used like other paint properties--to allow the parameter-pack-based
 * batch evaluation of paint properties to compile properly.
 */
class ColorRampProperty {
public:
    using TransitionableType = Transitionable<ColorRampPropertyValue>;
    using UnevaluatedType = Transitioning<ColorRampPropertyValue>;
    using EvaluatorType = PropertyEvaluator<Color>;
    using PossiblyEvaluatedType = Color;
    using Type = Color;
    static constexpr bool IsDataDriven = false;
    static constexpr bool IsOverridable = false;

    static Color defaultValue() { return {}; }
};

} // namespace style
} // namespace mbgl
