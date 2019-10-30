#pragma once

#include <mbgl/style/property_value.hpp>
#include <mbgl/renderer/property_evaluation_parameters.hpp>
#include <mbgl/renderer/possibly_evaluated_property_value.hpp>
#include <mbgl/renderer/cross_faded_property_evaluator.hpp>

namespace mbgl {

template <typename T, bool useIntegerZoom = false>
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
        if (useIntegerZoom) {  // Compiler will optimize out the unused branch.
            if (!expression.isFeatureConstant() || !expression.isRuntimeConstant()) {
                auto returnExpression = expression;
                returnExpression.useIntegerZoom = true;
                return ResultType(returnExpression);
            }
            return ResultType(expression.evaluate(floor(parameters.z)));
        } else {
            if (!expression.isFeatureConstant() || !expression.isRuntimeConstant()) {
                return ResultType(expression);
            }
            return ResultType(expression.evaluate(parameters.z));
        }
    }

private:
    const PropertyEvaluationParameters& parameters;
    T defaultValue;
};

template <typename T>
class DataDrivenPropertyEvaluator<Faded<T>> {
public:
    using ResultType = PossiblyEvaluatedPropertyValue<Faded<T>>;

    DataDrivenPropertyEvaluator(const PropertyEvaluationParameters& parameters_, T defaultValue_)
    : parameters(parameters_),
      defaultValue(std::move(defaultValue_)) {}

    ResultType operator()(const T& constant) const {
        return ResultType(calculate(constant, constant, constant));
    }

    ResultType operator()(const style::Undefined& ) const {
        return ResultType(calculate(defaultValue, defaultValue, defaultValue));
    }

    ResultType operator()(const style::PropertyExpression<T>& expression) const {
        if (!expression.isFeatureConstant() || !expression.isRuntimeConstant()) {
            return ResultType(expression);
        } else {
            const T evaluated = expression.evaluate(floor(parameters.z));
            return ResultType(calculate(evaluated, evaluated, evaluated));
        }
    }


private:
    Faded<T> calculate(const T& min, const T& mid, const T& max) const {
        const float z = parameters.z;
        return z > parameters.zoomHistory.lastIntegerZoom
            ? Faded<T> { min, mid }
            : Faded<T> { max, mid };
    };

    const PropertyEvaluationParameters& parameters;
    T defaultValue;
};

} // namespace mbgl
