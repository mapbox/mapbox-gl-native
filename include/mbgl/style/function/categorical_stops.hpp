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
    T defaultValue;

    CategoricalStops() = default;
    CategoricalStops(Stops stops_, T defaultValue_ = T())
        : stops(std::move(stops_)),
          defaultValue(std::move(defaultValue_)) {
        assert(stops.size() > 0);
    }

    T evaluate(const Value&) const;

    friend bool operator==(const CategoricalStops& lhs,
                           const CategoricalStops& rhs) {
        return lhs.stops == rhs.stops && lhs.defaultValue == rhs.defaultValue;
    }
};

} // namespace style
} // namespace mbgl
