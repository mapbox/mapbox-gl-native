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
        util::Interpolatable<T>::value,
        variant<
            ExponentialStops<T>,
            IntervalStops<T>,
            CategoricalStops<T>>,
        variant<
            IntervalStops<T>,
            CategoricalStops<T>>>;

    using Stops = std::conditional_t<
        util::Interpolatable<T>::value,
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

    struct CoveringRanges {
        float zoom;
        Range<float> coveringZoomRange;
        Range<InnerStops> coveringStopsRange;
    };

    // Return the relevant stop zoom values and inner stops that bracket a given zoom level. This
    // is the first step toward evaluating the function, and is used for in the course of both partial
    // evaluation of data-driven paint properties, and full evaluation of data-driven layout properties.
    CoveringRanges coveringRanges(float zoom) const {
        return stops.match(
            [&] (const auto& s) {
                assert(!s.stops.empty());
                auto minIt = s.stops.lower_bound(zoom);
                auto maxIt = s.stops.upper_bound(zoom);
                
                // lower_bound yields first element >= zoom, but we want the *last*
                // element <= zoom, so if we found a stop > zoom, back up by one.
                if (minIt != s.stops.begin() && minIt != s.stops.end() && minIt->first > zoom) {
                    minIt--;
                }
                
                return CoveringRanges {
                    zoom,
                    Range<float> {
                        minIt == s.stops.end() ? s.stops.rbegin()->first : minIt->first,
                        maxIt == s.stops.end() ? s.stops.rbegin()->first : maxIt->first
                    },
                    Range<InnerStops> {
                        s.innerStops(minIt == s.stops.end() ? s.stops.rbegin()->second : minIt->second),
                        s.innerStops(maxIt == s.stops.end() ? s.stops.rbegin()->second : maxIt->second)
                    }
                };
            }
        );
    }

    // Given a range of zoom values (typically two adjacent integer zoom levels, e.g. 5.0 and 6.0),
    // return the covering ranges for both. This is used in the course of partial evaluation for
    // data-driven paint properties.
    Range<CoveringRanges> rangeOfCoveringRanges(Range<float> zoomRange) {
        return Range<CoveringRanges> {
            coveringRanges(zoomRange.min),
            coveringRanges(zoomRange.max)
        };
    }

    // Given the covering ranges for range of zoom values (typically two adjacent integer zoom levels,
    // e.g. 5.0 and 6.0), and a feature, return the results of fully evaluating the function for that
    // feature at each of the two zoom levels. These two results are what go into the paint vertex buffers
    // for vertices associated with this feature. The shader will interpolate between them at render time.
    template <class Feature>
    Range<T> evaluate(const Range<CoveringRanges>& ranges, const Feature& feature, T finalDefaultValue) {
        optional<Value> value = feature.getValue(property);
        if (!value) {
            return Range<T> {
                defaultValue.value_or(finalDefaultValue),
                defaultValue.value_or(finalDefaultValue)
            };
        }
        return Range<T> {
            evaluateFinal(ranges.min, *value, finalDefaultValue),
            evaluateFinal(ranges.max, *value, finalDefaultValue)
        };
    }

    // Fully evaluate the function for a zoom value and feature. This is used when evaluating data-driven
    // layout properties.
    template <class Feature>
    T evaluate(float zoom, const Feature& feature, T finalDefaultValue) const {
        optional<Value> value = feature.getValue(property);
        if (!value) {
            return defaultValue.value_or(finalDefaultValue);
        }
        return evaluateFinal(coveringRanges(zoom), *value, finalDefaultValue);
    }

    friend bool operator==(const CompositeFunction& lhs,
                           const CompositeFunction& rhs) {
        return std::tie(lhs.property, lhs.stops, lhs.defaultValue)
            == std::tie(rhs.property, rhs.stops, rhs.defaultValue);
    }

    std::string property;
    Stops stops;
    optional<T> defaultValue;
    bool useIntegerZoom = false;

private:
    T evaluateFinal(const CoveringRanges& ranges, const Value& value, T finalDefaultValue) const {
        auto eval = [&] (const auto& s) {
            return s.evaluate(value).value_or(defaultValue.value_or(finalDefaultValue));
        };
        return util::interpolate(
            ranges.coveringStopsRange.min.match(eval),
            ranges.coveringStopsRange.max.match(eval),
            util::interpolationFactor(1.0f, ranges.coveringZoomRange, ranges.zoom));
    }
};

} // namespace style
} // namespace mbgl
