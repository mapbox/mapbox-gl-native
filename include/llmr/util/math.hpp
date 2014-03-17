#ifndef LLMR_UTIL_MATH
#define LLMR_UTIL_MATH

#include <cmath>
#include <array>

#include "vec.hpp"

namespace llmr {
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
    return atan2((ax * by - ay * bx), ax * bx + ay * by);
}

template <typename T = double, typename S>
inline T angle_between(const vec2<S>& a, const vec2<S>& b) {
    return angle_between(a.x, a.y, b.x, b.y);
}

template <typename T = double, typename S>
inline T angle_to(const vec2<S>& a, const vec2<S>& b) {
    return atan2(a.y - b.y, a.x - b.x);
}

template <typename T, typename S1, typename S2>
inline T interp(S1 a, S2 b, T t) {
    return (a * ((T)1 - t)) + (b * t);
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
    T c = sqrt(dx * dx + dy * dy);
    return { dx / c, dy / c };
}

template <typename T, typename S1, typename S2>
inline T dist(const S1& a, const S2& b) {
    T dx = b.x - a.x;
    T dy = b.y - a.y;
    T c = sqrt(dx * dx + dy * dy);
    return c;
}

// Take the magnitude of vector a.
template <typename T = double, typename S>
inline T mag(const S& a) {
    return sqrt(a.x * a.x + a.y * a.y);
}

}
}

#endif
