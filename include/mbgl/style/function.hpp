#pragma once

#include <vector>
#include <utility>

enum class ColorSpace {
    RGB,
    LAB,
    HCL
};

namespace mbgl {
namespace style {

template <typename T>
class Function {
public:
    using Stop = std::pair<float, T>;
    using Stops = std::vector<Stop>;

    explicit Function(Stops stops_, float base_, ColorSpace colorSpace_ = ColorSpace::RGB)
        : base(base_), stops(std::move(stops_)), colorSpace(colorSpace_) {}

    float getBase() const { return base; }
    ColorSpace getColorSpace() const { return colorSpace; }
    const std::vector<std::pair<float, T>>& getStops() const { return stops; }

private:
    float base = 1;
    std::vector<std::pair<float, T>> stops;
    ColorSpace colorSpace = ColorSpace::RGB;

    template <class S> friend bool operator==(const Function<S>&, const Function<S>&);
};

template <class T>
bool operator==(const Function<T>& lhs, const Function<T>& rhs) {
    return lhs.base == rhs.base &&
        lhs.stops == rhs.stops &&
        lhs.colorSpace == rhs.colorSpace;
}

template <class T>
bool operator!=(const Function<T>& lhs, const Function<T>& rhs) {
    return !(lhs == rhs);
}

} // namespace style
} // namespace mbgl
