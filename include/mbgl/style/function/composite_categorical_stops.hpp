#pragma once

#include <mbgl/style/function/categorical_stops.hpp>

namespace mbgl {
namespace style {

template <class T>
class CompositeCategoricalStops {
public:
    using Stops = std::map<float, std::map<CategoricalValue, T>>;
    Stops stops;

    CompositeCategoricalStops() = default;
    CompositeCategoricalStops(Stops stops_)
        : stops(std::move(stops_)) {
    }

    CategoricalStops<T> innerStops(const std::map<CategoricalValue, T>& stops_) const {
        return CategoricalStops<T>(stops_);
    }

    friend bool operator==(const CompositeCategoricalStops& lhs,
                           const CompositeCategoricalStops& rhs) {
        return lhs.stops == rhs.stops;
    }
};

} // namespace style
} // namespace mbgl
