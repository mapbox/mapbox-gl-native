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

    ResultType operator()(const style::PropertyExpression<T>& expression) const {
        if (!expression.isFeatureConstant()) {
            auto returnExpression = expression;
            returnExpression.useIntegerZoom = parameters.useIntegerZoom;
            return ResultType(returnExpression);
        } else if (!parameters.useIntegerZoom) {
            return ResultType(expression.evaluate(parameters.z));
        } else {
            return ResultType(expression.evaluate(floor(parameters.z)));
        }
    }

private:
    const PropertyEvaluationParameters& parameters;
    T defaultValue;
};

} // namespace mbgl
