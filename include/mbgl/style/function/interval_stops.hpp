#pragma once

#include <mbgl/util/feature.hpp>

#include <map>

namespace mbgl {
namespace style {

template <class T>
class IntervalStops {
public:
    using Stops = std::map<float, T>;
    Stops stops;

    IntervalStops() = default;
    IntervalStops(Stops stops_)
        : stops(std::move(stops_)) {
    }

    optional<T> evaluate(float z) const {
        if (stops.empty()) {
            return {};
        }

        auto it = stops.upper_bound(z);
        if (it == stops.end()) {
            return stops.rbegin()->second;
        } else if (it == stops.begin()) {
            return stops.begin()->second;
        } else {
            return std::prev(it)->second;
        }
    }

    optional<T> evaluate(const Value& value) const {
        optional<float> z = numericValue<float>(value);
        if (!z) {
            return {};
        }
        return evaluate(*z);
    }

    friend bool operator==(const IntervalStops& lhs,
                           const IntervalStops& rhs) {
        return lhs.stops == rhs.stops;
    }
};

} // namespace style
} // namespace mbgl
