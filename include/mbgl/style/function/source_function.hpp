#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/value.hpp>
#include <mbgl/style/expression/is_constant.hpp>

namespace mbgl {
namespace style {

template <class T>
class SourceFunction {
public:
    // The second parameter should be used only for conversions from legacy functions.
    SourceFunction(std::unique_ptr<expression::Expression> expression_, optional<T> defaultValue_ = {})
        : isExpression(defaultValue_),
          expression(std::move(expression_)),
          defaultValue(std::move(defaultValue_)) {
        assert(expression::isZoomConstant(*expression));
        assert(!expression::isFeatureConstant(*expression));
    }
    
    template <class Feature>
    T evaluate(const Feature& feature, T finalDefaultValue) const {
        const expression::EvaluationResult result = expression->evaluate(expression::EvaluationContext(&feature));
        if (result) {
            const optional<T> typed = expression::fromExpressionValue<T>(*result);
            return typed ? *typed : defaultValue ? *defaultValue : finalDefaultValue;
        }
        return defaultValue ? *defaultValue : finalDefaultValue;
    }

    std::vector<optional<T>> possibleOutputs() const {
        return expression::fromExpressionValues<T>(expression->possibleOutputs());
    }

    friend bool operator==(const SourceFunction& lhs,
                           const SourceFunction& rhs) {
        return *lhs.expression == *rhs.expression;
    }

    bool useIntegerZoom = false;
    bool isExpression;

    const expression::Expression& getExpression() const { return *expression; }

private:
    std::shared_ptr<const expression::Expression> expression;
    optional<T> defaultValue;
};

} // namespace style
} // namespace mbgl
