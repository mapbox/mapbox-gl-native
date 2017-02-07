#pragma once

#include <mbgl/style/function/interval_stops.hpp>

#include <map>

namespace mbgl {
namespace style {

template <class T>
class CompositeIntervalStops {
public:
    using Stops = std::map<float, std::map<float, T>>;
    Stops stops;

    CompositeIntervalStops() = default;
    CompositeIntervalStops(Stops stops_)
        : stops(std::move(stops_)) {
    }

    IntervalStops<T> innerStops(const std::map<float, T>& stops_) const {
        return IntervalStops<T>(stops_);
    }

    friend bool operator==(const CompositeIntervalStops& lhs,
                           const CompositeIntervalStops& rhs) {
        return lhs.stops == rhs.stops;
    }
};

} // namespace style
} // namespace mbgl
