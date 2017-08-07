#pragma once

#include <mbgl/style/function/exponential_stops.hpp>

#include <map>

namespace mbgl {
namespace style {

template <class T>
class CompositeExponentialStops {
public:
    using Stops = std::map<float, std::map<float, T>>;

    Stops stops;
    float base = 1.0f;

    CompositeExponentialStops() = default;
    CompositeExponentialStops(Stops stops_, float base_ = 1.0f)
        : stops(std::move(stops_)),
          base(base_) {
    }

    ExponentialStops<T> innerStops(const std::map<float, T>& stops_) const {
        return ExponentialStops<T>(stops_, base);
    }

    friend bool operator==(const CompositeExponentialStops& lhs,
                           const CompositeExponentialStops& rhs) {
        return lhs.stops == rhs.stops && lhs.base == rhs.base;
    }
};

} // namespace style
} // namespace mbgl
