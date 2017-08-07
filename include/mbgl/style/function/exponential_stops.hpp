#pragma once

#include <mbgl/util/feature.hpp>
#include <mbgl/util/interpolate.hpp>

#include <map>

namespace mbgl {
namespace style {

template <class T>
class ExponentialStops {
public:
    using Stops = std::map<float, T>;

    Stops stops;
    float base = 1.0f;

    ExponentialStops() = default;
    ExponentialStops(Stops stops_, float base_ = 1.0f)
        : stops(std::move(stops_)),
          base(base_) {
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
            return util::interpolate(std::prev(it)->second, it->second,
                util::interpolationFactor(base, { std::prev(it)->first, it->first }, z));
        }
    }

    optional<T> evaluate(const Value& value) const {
        optional<float> z = numericValue<float>(value);
        if (!z) {
            return {};
        }
        return evaluate(*z);
    }

    friend bool operator==(const ExponentialStops& lhs,
                           const ExponentialStops& rhs) {
        return lhs.stops == rhs.stops && lhs.base == rhs.base;
    }
};

} // namespace style
} // namespace mbgl
