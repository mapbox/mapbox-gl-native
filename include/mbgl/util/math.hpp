#ifndef MBGL_UTIL_MATH
#define MBGL_UTIL_MATH

#include <cmath>
#include <array>

#include <mbgl/util/vec.hpp>

namespace mbgl {
namespace util {


template <typename T>
inline T max(T a, T b) {
    return b > a ? b : a;
}

template <typename T>
inline T max(T a, T b, T c) {
    return max(max(a, b), c);
}

template <typename T>
inline T max(T a, T b, T c, T d) {
    return max(max(a, b), max(c, d));
}

template <typename T>
inline T min(T a, T b) {
    return b < a ? b : a;
}

template <typename T>
inline T min(T a, T b, T c) {
    return min(min(a, b), c);
}

template <typename T>
inline T min(T a, T b, T c, T d) {
    return min(min(a, b), min(c, d));
}

// Find the angle of the two vectors, solving the formula for the cross product
// a x b = |a||b|sin(θ) for θ.
template <typename T = double, typename S>
inline T angle_between(S ax, S ay, S bx, S by) {
    return std::atan2((ax * by - ay * bx), ax * bx + ay * by);
}

template <typename T = double, typename S>
inline T angle_between(const vec2<S>& a, const vec2<S>& b) {
    return angle_between(a.x, a.y, b.x, b.y);
}

template <typename T = double, typename S>
inline T angle_to(const vec2<S>& a, const vec2<S>& b) {
    return std::atan2(a.y - b.y, a.x - b.x);
}

// Reflect an angle around 0 degrees
template <typename T>
inline std::array<T, 2> flip(const std::array<T, 2>& c) {
    return {{
        static_cast<T>(2 * M_PI - c[0]),
        static_cast<T>(2 * M_PI - c[1])
    }};
}

template <typename T, typename S1, typename S2>
inline vec2<T> normal(const S1& a, const S2& b) {
    T dx = b.x - a.x;
    T dy = b.y - a.y;
    T c = std::sqrt(dx * dx + dy * dy);
    return { dx / c, dy / c };
}

template <typename T>
inline T perp(const T& a) {
    return T(-a.y, a.x);
}

template <typename T, typename S1, typename S2>
inline T dist(const S1& a, const S2& b) {
    T dx = b.x - a.x;
    T dy = b.y - a.y;
    T c = std::sqrt(dx * dx + dy * dy);
    return c;
}

template <typename T>
inline T length(T a, T b) {
    return std::sqrt(a * a + b * b);
}

// Take the magnitude of vector a.
template <typename T = double, typename S>
inline T mag(const S& a) {
    return std::sqrt(a.x * a.x + a.y * a.y);
}

template <typename S>
inline S unit(const S& a) {
    auto magnitude = mag(a);
    if (magnitude == 0) {
        return a;
    }
    return a * (1 / magnitude);
}

template <typename T>
T clamp(T value, T min, T max) {
    return value < min ? min : (value > max ? max : value);
}

// Constrains n to the given range (including min, excluding max) via modular
// arithmetic.
template <typename T>
T wrap(T value, T min, T max) {
    T d = max - min;
    return std::fmod((std::fmod((value - min), d) + d), d) + min;
}

template <typename T>
T smoothstep(T edge0, T edge1, T x) {
    T t = clamp((x - edge0) / (edge1 - edge0), T(0), T(1));
    return t * t * (T(3) - T(2) * t);
}

// Computes the log2(x) rounded up to the next integer.
// (== number of bits required to store x)
uint32_t ceil_log2(uint64_t x);

}
}

#endif
