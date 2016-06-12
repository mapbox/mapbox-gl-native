#pragma once

#include <vector>
#include <utility>

namespace mbgl {
namespace style {

template <typename T>
class Function {
public:
    using Stop = std::pair<float, T>;
    using Stops = std::vector<Stop>;

    explicit Function(Stops stops_, float base_)
        : base(base_), stops(std::move(stops_)) {}

    float getBase() const { return base; }
    const std::vector<std::pair<float, T>>& getStops() const { return stops; }

private:
    float base = 1;
    std::vector<std::pair<float, T>> stops;
};

} // namespace style
} // namespace mbgl
