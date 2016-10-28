#pragma once

#include <mbgl/style/property_value.hpp>
#include <mbgl/style/property_evaluation_parameters.hpp>
#include <mbgl/style/possibly_evaluated_property_value.hpp>

namespace mbgl {
namespace style {

template <typename T>
class DataDrivenPropertyEvaluator {
public:
    using ResultType = PossiblyEvaluatedPropertyValue<T>;

    DataDrivenPropertyEvaluator(const PropertyEvaluationParameters& parameters_, T defaultValue_)
        : parameters(parameters_),
          defaultValue(std::move(defaultValue_)) {}

    ResultType operator()(const Undefined&) const {
        return ResultType(defaultValue);
    }

    ResultType operator()(const T& constant) const {
        return ResultType(constant);
    }

    ResultType operator()(const CameraFunction<T>& function) const {
        return ResultType(function.evaluate(parameters.z));
    }

    template <class Function>
    ResultType operator()(const Function& function) const {
        return ResultType(function);
    }

private:
    const PropertyEvaluationParameters& parameters;
    T defaultValue;
};

} // namespace style
} // namespace mbgl
