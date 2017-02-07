#pragma once

#include <mbgl/style/function/composite_exponential_stops.hpp>
#include <mbgl/style/function/composite_interval_stops.hpp>
#include <mbgl/style/function/composite_categorical_stops.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/util/range.hpp>
#include <mbgl/util/variant.hpp>

#include <string>
#include <tuple>

namespace mbgl {

class GeometryTileFeature;

namespace style {

// A CompositeFunction consists of an outer zoom function whose stop range values are
// "inner" source functions. It provides the GL Native implementation of
// "zoom-and-property" functions from the style spec.

template <class T>
class CompositeFunction {
public:
    using InnerStops = std::conditional_t<
        util::Interpolatable<T>,
        variant<
            ExponentialStops<T>,
            IntervalStops<T>,
            CategoricalStops<T>>,
        variant<
            IntervalStops<T>,
            CategoricalStops<T>>>;

    using Stops = std::conditional_t<
        util::Interpolatable<T>,
        variant<
            CompositeExponentialStops<T>,
            CompositeIntervalStops<T>,
            CompositeCategoricalStops<T>>,
        variant<
            CompositeIntervalStops<T>,
            CompositeCategoricalStops<T>>>;

    CompositeFunction(std::string property_, Stops stops_, optional<T> defaultValue_ = {})
        : property(std::move(property_)),
          stops(std::move(stops_)),
          defaultValue(std::move(defaultValue_)) {
    }

    std::tuple<Range<float>, Range<InnerStops>>
    coveringRanges(float zoom) const {
        return stops.match(
            [&] (const auto& s) {
                assert(!s.stops.empty());
                auto minIt = s.stops.lower_bound(zoom);
                auto maxIt = s.stops.upper_bound(zoom);
                if (minIt != s.stops.begin()) {
                    minIt--;
                }
                return std::make_tuple(
                    Range<float> {
                        minIt == s.stops.end() ? s.stops.rbegin()->first : minIt->first,
                        maxIt == s.stops.end() ? s.stops.rbegin()->first : maxIt->first
                    },
                    Range<InnerStops> {
                        s.innerStops(minIt == s.stops.end() ? s.stops.rbegin()->second : minIt->second),
                        s.innerStops(maxIt == s.stops.end() ? s.stops.rbegin()->second : maxIt->second)
                    }
                );
            }
        );
    }

    Range<T> evaluate(Range<InnerStops> coveringStops,
                      const GeometryTileFeature& feature,
                      T finalDefaultValue) const {
        optional<Value> v = feature.getValue(property);
        if (!v) {
            return {
                defaultValue.value_or(finalDefaultValue),
                defaultValue.value_or(finalDefaultValue)
            };
        }
        auto eval = [&] (const auto& s) {
            return s.evaluate(*v).value_or(defaultValue.value_or(finalDefaultValue));
        };
        return Range<T> {
            coveringStops.min.match(eval),
            coveringStops.max.match(eval)
        };
    }

    T evaluate(float zoom, const GeometryTileFeature& feature, T finalDefaultValue) const {
        std::tuple<Range<float>, Range<InnerStops>> ranges = coveringRanges(zoom);
        Range<T> resultRange = evaluate(std::get<1>(ranges), feature, finalDefaultValue);
        return util::interpolate(
            resultRange.min,
            resultRange.max,
            util::interpolationFactor(1.0f, std::get<0>(ranges), zoom));
    }

    friend bool operator==(const CompositeFunction& lhs,
                           const CompositeFunction& rhs) {
        return std::tie(lhs.property, lhs.stops, lhs.defaultValue)
            == std::tie(rhs.property, rhs.stops, rhs.defaultValue);
    }

    std::string property;
    Stops stops;
    optional<T> defaultValue;
};

} // namespace style
} // namespace mbgl
