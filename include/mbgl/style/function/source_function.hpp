#pragma once

#include <mbgl/style/expression/is_constant.hpp>
#include <mbgl/style/function/convert.hpp>
#include <mbgl/style/function/exponential_stops.hpp>
#include <mbgl/style/function/interval_stops.hpp>
#include <mbgl/style/function/categorical_stops.hpp>
#include <mbgl/style/function/identity_stops.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/util/variant.hpp>

#include <string>

namespace mbgl {
namespace style {

template <class T>
class SourceFunction {
public:
    using Stops = std::conditional_t<
        util::Interpolatable<T>::value,
        variant<
            ExponentialStops<T>,
            IntervalStops<T>,
            CategoricalStops<T>,
            IdentityStops<T>>,
        variant<
            IntervalStops<T>,
            CategoricalStops<T>,
            IdentityStops<T>>>;

    SourceFunction(std::unique_ptr<expression::Expression> expression_)
        : isExpression(true),
          expression(std::move(expression_))
    {
        assert(expression::isZoomConstant(*expression));
        assert(!expression::isFeatureConstant(*expression));
    }
    
    SourceFunction(const std::string& property, const Stops& stops, optional<T> defaultValue_ = {})
        : isExpression(false),
          defaultValue(std::move(defaultValue_)),
          expression(stops.match([&] (const IdentityStops<T>&) {
              return expression::Convert::fromIdentityFunction(expression::valueTypeToExpressionType<T>(), property);
          }, [&] (const auto& s) {
              return expression::Convert::toExpression(property, s);
          }))
    {}

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
    optional<T> defaultValue;
    std::shared_ptr<expression::Expression> expression;
};

} // namespace style
} // namespace mbgl
