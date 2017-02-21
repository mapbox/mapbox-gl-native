#pragma once

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
        util::Interpolatable<T>,
        variant<
            ExponentialStops<T>,
            IntervalStops<T>,
            CategoricalStops<T>,
            IdentityStops<T>>,
        variant<
            IntervalStops<T>,
            CategoricalStops<T>,
            IdentityStops<T>>>;

    SourceFunction(std::string property_, Stops stops_, optional<T> defaultValue_ = {})
        : property(std::move(property_)),
          stops(std::move(stops_)),
          defaultValue(std::move(defaultValue_)) {
    }

    template <class Feature>
    T evaluate(const Feature& feature, T finalDefaultValue) const {
        optional<Value> v = feature.getValue(property);
        if (!v) {
            return defaultValue.value_or(finalDefaultValue);
        }
        return stops.match([&] (const auto& s) -> T {
            return s.evaluate(*v).value_or(defaultValue.value_or(finalDefaultValue));
        });
    }

    friend bool operator==(const SourceFunction& lhs,
                           const SourceFunction& rhs) {
        return std::tie(lhs.property, lhs.stops, lhs.defaultValue)
            == std::tie(rhs.property, rhs.stops, rhs.defaultValue);
    }

    std::string property;
    Stops stops;
    optional<T> defaultValue;
};

} // namespace style
} // namespace mbgl
