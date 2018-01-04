#pragma once

#include <mbgl/style/property_value.hpp>
#include <mbgl/renderer/property_evaluation_parameters.hpp>
#include <mbgl/renderer/possibly_evaluated_property_value.hpp>

namespace mbgl {

template <typename T>
class DataDrivenPropertyEvaluator {
public:
    using ResultType = PossiblyEvaluatedPropertyValue<T>;

    DataDrivenPropertyEvaluator(const PropertyEvaluationParameters& parameters_, T defaultValue_)
        : parameters(parameters_),
          defaultValue(std::move(defaultValue_)) {}

    ResultType operator()(const style::Undefined&) const {
        return ResultType(defaultValue);
    }

    ResultType operator()(const T& constant) const {
        return ResultType(constant);
    }

    ResultType operator()(const style::CameraFunction<T>& function) const {
        if (!parameters.useIntegerZoom) {
            return ResultType(function.evaluate(parameters.z));
        } else {
            return ResultType(function.evaluate(floor(parameters.z)));
        }
    }

    template <class Function>
    ResultType operator()(const Function& function) const {
        auto returnFunction = function;
        returnFunction.useIntegerZoom = parameters.useIntegerZoom;
        return ResultType(returnFunction);
    }

private:
    const PropertyEvaluationParameters& parameters;
    T defaultValue;
};

} // namespace mbgl
