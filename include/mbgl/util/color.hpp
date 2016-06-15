#pragma once

#include <array>

namespace mbgl {

// Stores a premultiplied color, with all four channels ranging from 0..1
class Color {
public:
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 0.0f;
};

inline bool operator== (const Color& colorA, const Color& colorB) {
    return colorA.r == colorB.r && colorA.g == colorB.g && colorA.b == colorB.b && colorA.a == colorB.a;
}

inline bool operator!= (const Color& colorA, const Color& colorB) {
    return !(colorA.r == colorB.r && colorA.g == colorB.g && colorA.b == colorB.b && colorA.a == colorB.a);
}

} // namespace mbgl
