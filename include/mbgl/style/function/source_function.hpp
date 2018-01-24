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
        : expression(std::move(expression_))
    {
        assert(expression::isZoomConstant(*expression));
        assert(!expression::isFeatureConstant(*expression));
    }
    
    SourceFunction(std::string property_, Stops stops_, optional<T> defaultValue_ = {})
        : property(std::move(property_)),
          stops(std::move(stops_)),
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

    const expression::Expression& getExpression() const { return *expression; }

    // retained for compatibility with pre-expression function API
    std::string property;
    Stops stops;
    optional<T> defaultValue;

private:
    std::shared_ptr<expression::Expression> expression;
};

} // namespace style
} // namespace mbgl
