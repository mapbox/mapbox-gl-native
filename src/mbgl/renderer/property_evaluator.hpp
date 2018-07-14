#pragma once

#include <mbgl/style/property_value.hpp>
#include <mbgl/renderer/property_evaluation_parameters.hpp>

namespace mbgl {

template <typename T>
class PropertyEvaluator {
public:
    using ResultType = T;

    PropertyEvaluator(const PropertyEvaluationParameters& parameters_, T defaultValue_)
        : parameters(parameters_),
          defaultValue(std::move(defaultValue_)) {}

    T operator()(const style::Undefined&) const { return defaultValue; }
    T operator()(const T& constant) const { return constant; }
    T operator()(const style::PropertyExpression<T>& fn) const { return fn.evaluate(parameters.z); }

private:
    const PropertyEvaluationParameters& parameters;
    T defaultValue;
};

} // namespace mbgl
