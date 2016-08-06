#pragma once

#include <mbgl/util/optional.hpp>

#include <string>

namespace mbgl {

// Stores a premultiplied color, with all four channels ranging from 0..1
class Color {
public:
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 0.0f;

    static constexpr Color black() { return { 0.0f, 0.0f, 0.0f, 1.0f }; };
    static constexpr Color white() { return { 1.0f, 1.0f, 1.0f, 1.0f }; };

    static optional<Color> parse(const std::string&);
};

constexpr bool operator==(const Color& colorA, const Color& colorB) {
    return colorA.r == colorB.r && colorA.g == colorB.g && colorA.b == colorB.b && colorA.a == colorB.a;
}

constexpr bool operator!=(const Color& colorA, const Color& colorB) {
    return !(colorA == colorB);
}

constexpr Color operator*(const Color& color, float alpha) {
    return {
        color.r * alpha,
        color.g * alpha,
        color.b * alpha,
        color.a * alpha
    };
}

} // namespace mbgl
