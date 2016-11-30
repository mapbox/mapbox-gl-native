#pragma once

#include <cassert>
#include <utility>
#include <vector>

namespace mbgl {
namespace style {

template <typename T>
class Function {
public:
    using Stop = std::pair<float, T>;
    using Stops = std::vector<Stop>;

    Function(Stops stops_, float base_)
        : base(base_), stops(std::move(stops_)) {
        assert(stops.size() > 0);
    }

    float getBase() const { return base; }
    const std::vector<std::pair<float, T>>& getStops() const { return stops; }

    T evaluate(float z) const;

    friend bool operator==(const Function& lhs, const Function& rhs) {
        return lhs.base == rhs.base && lhs.stops == rhs.stops;
    }

    friend bool operator!=(const Function& lhs, const Function& rhs) {
        return !(lhs == rhs);
    }

private:
    float base = 1;
    std::vector<std::pair<float, T>> stops;
};

} // namespace style
} // namespace mbgl
