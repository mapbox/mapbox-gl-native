#pragma once

#include <mbgl/util/optional.hpp>

#include <cassert>
#include <string>
#include <array>

namespace mbgl {

// Stores a premultiplied color, with all four channels ranging from 0..1
class Color {
public:
    Color() = default;
    Color(float r_, float g_, float b_, float a_)
        : r(r_), g(g_), b(b_), a(a_) {
        assert(r_ >= 0.0f);
        assert(r_ <= 1.0f);
        assert(g_ >= 0.0f);
        assert(g_ <= 1.0f);
        assert(b_ >= 0.0f);
        assert(b_ <= 1.0f);
        assert(a_ >= 0.0f);
        assert(a_ <= 1.0f);
    }

    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 0.0f;

    static Color black() { return { 0.0f, 0.0f, 0.0f, 1.0f }; };
    static Color white() { return { 1.0f, 1.0f, 1.0f, 1.0f }; };

    static Color red()   { return { 1.0f, 0.0f, 0.0f, 1.0f }; };
    static Color green() { return { 0.0f, 1.0f, 0.0f, 1.0f }; };
    static Color blue()  { return { 0.0f, 0.0f, 1.0f, 1.0f }; };

    static optional<Color> parse(const std::string&);
    std::string stringify() const;
    std::array<double, 4> toArray() const;
};

inline bool operator==(const Color& colorA, const Color& colorB) {
    return colorA.r == colorB.r && colorA.g == colorB.g && colorA.b == colorB.b && colorA.a == colorB.a;
}

inline bool operator!=(const Color& colorA, const Color& colorB) {
    return !(colorA == colorB);
}

inline Color operator*(const Color& color, float alpha) {
    assert(alpha >= 0.0f);
    assert(alpha <= 1.0f);
    return {
        color.r * alpha,
        color.g * alpha,
        color.b * alpha,
        color.a * alpha
    };
}

} // namespace mbgl
