#pragma once

#include <mbgl/util/optional.hpp>
#include <mbgl/util/color.hpp>
#include <string>

namespace mbgl {

// Stores a LAB color, with all four channels ranging from 0..1
class ColorLAB {
public:
    float l = 0.0f;
    float a = 0.0f;
    float b = 0.0f;
    float A = 0.0f;
    optional<Color> to_rgb();
};

constexpr bool operator==(const ColorLAB& colorA, const ColorLAB& colorB) {
    return colorA.l == colorB.l && colorA.a == colorB.a && colorA.b == colorB.b && colorA.A == colorB.A;
}

constexpr bool operator!=(const ColorLAB& colorA, const ColorLAB& colorB) {
    return !(colorA == colorB);
}

} // namespace mbgl
