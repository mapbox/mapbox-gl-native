#pragma once

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
        assert(expression->isZoomConstant());
        assert(!expression->isFeatureConstant());
    }
    
    SourceFunction(std::string property_, Stops stops_, optional<T> defaultValue_ = {})
        : property(std::move(property_)),
          stops(std::move(stops_)),
          defaultValue(std::move(defaultValue_)),
          expression(stops.match([&] (const auto& s) {
            return expression::Convert::toExpression(property, s, defaultValue);
          }))
    {}

    template <class Feature>
    T evaluate(const Feature& feature, T finalDefaultValue) const {
        const expression::EvaluationResult result = expression->evaluate(expression::EvaluationParameters(&feature));
        if (result) {
            const optional<T> typed = expression::fromExpressionValue<T>(*result);
            return typed ? *typed : finalDefaultValue;
        }
        return finalDefaultValue;
    }

    friend bool operator==(const SourceFunction& lhs,
                           const SourceFunction& rhs) {
        return std::tie(lhs.property, lhs.stops, lhs.defaultValue)
            == std::tie(rhs.property, rhs.stops, rhs.defaultValue);
    }

    bool useIntegerZoom = false;

    // retained for compatibility with pre-expression function API
    std::string property;
    Stops stops;
    optional<T> defaultValue;

private:
    std::shared_ptr<expression::Expression> expression;
};

} // namespace style
} // namespace mbgl
