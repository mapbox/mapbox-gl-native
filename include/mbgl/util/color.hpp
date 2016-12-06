#pragma once

#include <mbgl/util/optional.hpp>

#include <cassert>
#include <string>

namespace mbgl {

// Stores a premultiplied color, with all four channels ranging from 0..1
class Color {
public:
    constexpr Color() = default;
    constexpr Color(float r_, float g_, float b_, float a_)
        : r(r_), g(g_), b(b_), a(a_) {
// Some g++ versions can't deal with assertions within a constexpr constructor: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=65973
#if !defined(__GNUC__) || \
    (__GNUC__ == 5 && __GNUC_MINOR__ >= 4) || \
    (__GNUC__ == 6 && __GNUC_MINOR__ >= 1) || \
     __GNUC__ >= 7
        assert(r_ >= 0.0f);
        assert(r_ <= 1.0f);
        assert(g_ >= 0.0f);
        assert(g_ <= 1.0f);
        assert(b_ >= 0.0f);
        assert(b_ <= 1.0f);
        assert(a_ >= 0.0f);
        assert(a_ <= 1.0f);
#endif
    }

    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 0.0f;

    static constexpr Color black() { return { 0.0f, 0.0f, 0.0f, 1.0f }; };
    static constexpr Color white() { return { 1.0f, 1.0f, 1.0f, 1.0f }; };

    static constexpr Color red()   { return { 1.0f, 0.0f, 0.0f, 1.0f }; };
    static constexpr Color green() { return { 0.0f, 1.0f, 0.0f, 1.0f }; };
    static constexpr Color blue()  { return { 0.0f, 0.0f, 1.0f, 1.0f }; };

    static optional<Color> parse(const std::string&);
};

constexpr bool operator==(const Color& colorA, const Color& colorB) {
    return colorA.r == colorB.r && colorA.g == colorB.g && colorA.b == colorB.b && colorA.a == colorB.a;
}

constexpr bool operator!=(const Color& colorA, const Color& colorB) {
    return !(colorA == colorB);
}

constexpr Color operator*(const Color& color, float alpha) {
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
