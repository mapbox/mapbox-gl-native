#pragma once

#include <mbgl/util/optional.hpp>
#include <mbgl/util/color.hpp>
#include <string>

namespace mbgl {

// Stores a HCL color, with all four channels ranging from 0..1
class ColorHCL {
public:
    float h = 0.0f;
    float c = 0.0f;
    float l = 0.0f;
    float A = 0.0f;
};

constexpr bool operator==(const ColorHCL& colorA, const ColorHCL& colorB) {
    return colorA.h == colorB.h &&
        colorA.c == colorB.c &&
        colorA.l == colorB.l &&
        colorA.A == colorB.A;
}

constexpr bool operator!=(const ColorHCL& colorA, const ColorHCL& colorB) {
    return !(colorA == colorB);
}

} // namespace mbgl
