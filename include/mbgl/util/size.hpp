#pragma once

#include <cstdint>
#include <array>

namespace mbgl {

class Size {
public:
    constexpr Size() = default;

    constexpr Size(const uint32_t width_, const uint32_t height_) : width(width_), height(height_) {
    }

    constexpr uint32_t area() const {
        return width * height;
    }
    
    constexpr float aspectRatio() const {
        return static_cast<float>(width) / static_cast<float>(height);
    }

    constexpr bool isEmpty() const {
        return width == 0 || height == 0;
    }

    uint32_t width = 0;
    uint32_t height = 0;
};

constexpr inline bool operator==(const Size& a, const Size& b) {
    return a.width == b.width && a.height == b.height;
}

constexpr inline bool operator!=(const Size& a, const Size& b) {
    return !(a == b);
}

} // namespace mbgl
