#pragma once

#include <vector>
#include <utility>

using EnumType = uint32_t;

enum class ColorSpace : EnumType {
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

    explicit Function(Stops stops_, float base_)
        : base(base_), stops(std::move(stops_)) {}

    float getBase() const { return base; }
    ColorSpace getColorSpace() const { return colorSpace; }
    const std::vector<std::pair<float, T>>& getStops() const { return stops; }

private:
    float base = 1;
    ColorSpace colorSpace = ColorSpace::RGB;
    std::vector<std::pair<float, T>> stops;

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
