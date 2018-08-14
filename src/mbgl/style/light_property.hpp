#pragma once

#include <mbgl/style/properties.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/renderer/property_evaluator.hpp>

namespace mbgl {
namespace style {

template <class T>
class LightProperty {
public:
    using EvaluatorType = PropertyEvaluator<T>;
    using Type = T;
    static constexpr bool IsDataDriven = false;
};

} // namespace style
} // namespace mbgl
