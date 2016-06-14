#pragma once

#include <array>

namespace mbgl {

// Stores a premultiplied color, with all four channels ranging from 0..1
class Color {
public:
    Color(float r_, float g_, float b_, float a_)
        : r(r_), g(g_), b(b_), a(a_) {}
    Color()
        : r(0.0f), g(0.0f), b(0.0f), a(0.0f) {}

    float r;
    float g;
    float b;
    float a;
};

inline bool operator== (const Color& colorA, const Color& colorB) {
    return colorA.r == colorB.r && colorA.g == colorB.g && colorA.b == colorB.b && colorA.a == colorB.a;
}

inline bool operator!= (const Color& colorA, const Color& colorB) {
    return !(colorA.r == colorB.r && colorA.g == colorB.g && colorA.b == colorB.b && colorA.a == colorB.a);
}

} // namespace mbgl
