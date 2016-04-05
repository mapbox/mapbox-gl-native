#ifndef MBGL_UTIL_MATH
#define MBGL_UTIL_MATH

#include <cmath>
#include <array>
#include <limits>

#include <mbgl/util/vec.hpp>

namespace mbgl {
namespace util {


template <typename T>
typename std::enable_if_t<std::is_integral<T>::value, T> max(T a, T b) {
    return std::max(a, b);
}

template <typename T>
typename std::enable_if_t<std::is_floating_point<T>::value, T> max(T a, T b) {
    return ::fmax(a, b);
}

template <typename T, typename... Ts>
typename std::enable_if_t<std::is_arithmetic<T>::value, T> max(T a, T b, Ts... args) {
    return max(a, max(b, args...));
}

template <typename T>
typename std::enable_if_t<std::is_integral<T>::value, T> min(T a, T b) {
    return std::min(a, b);
}

template <typename T>
typename std::enable_if_t<std::is_floating_point<T>::value, T> min(T a, T b) {
    return ::fmin(a, b);
}

template <typename T, typename... Ts>
typename std::enable_if_t<std::is_arithmetic<T>::value, T> min(T a, T b, Ts... args) {
    return min(a, min(b, args...));
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

template <typename T, typename S1, typename S2>
inline T distSqr(const S1& a, const S2& b) {
    T dx = b.x - a.x;
    T dy = b.y - a.y;
    T c = dx * dx + dy * dy;
    return c;
}

template <typename T>
inline T round(const T& a) {
    return T(::round(a.x), ::round(a.y));
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

template <typename T, typename S = double>
inline T rotate(const T& a, S angle) {
    S cos = std::cos(angle);
    S sin = std::sin(angle);
    S x = cos * a.x - sin * a.y;
    S y = sin * a.x + cos * a.y;
    return T(x, y);
}

template <typename T>
T clamp(T value, T min_, T max_) {
    return max(min_, min(max_, value));
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

} // namespace util
} // namespace mbgl

#endif
