#pragma once

#include <mbgl/util/feature.hpp>
#include <mbgl/util/variant.hpp>

#include <cassert>
#include <utility>
#include <map>

namespace mbgl {
namespace style {

class CategoricalValue : public variant<bool, int64_t, std::string> {
public:
    using variant<bool, int64_t, std::string>::variant;
};

template <class T>
class CategoricalStops {
public:
    using Stops = std::map<CategoricalValue, T>;

    Stops stops;

    CategoricalStops() = default;
    CategoricalStops(Stops stops_)
        : stops(std::move(stops_)) {
        assert(stops.size() > 0);
    }

    optional<T> evaluate(const Value&) const;

    friend bool operator==(const CategoricalStops& lhs,
                           const CategoricalStops& rhs) {
        return lhs.stops == rhs.stops;
    }
};

} // namespace style
} // namespace mbgl
