#pragma once

#include <cmath>
#include <array>
#include <limits>

#include <mbgl/util/geometry.hpp>

namespace mbgl {
namespace util {

// TODO: split this file up into individual headers, following mbgl/math/*.hpp.

// Find the angle of the two vectors, solving the formula for the cross product
// a x b = |a||b|sin(θ) for θ.
template <typename T = double, typename S>
T angle_between(const Point<S>& a, const Point<S>& b) {
    return std::atan2((a.x * b.y - a.y * b.x), a.x * b.x + a.y * b.y);
}

template <typename T = double, typename S>
T angle_to(const Point<S>& a, const Point<S>& b) {
    return std::atan2(a.y - b.y, a.x - b.x);
}

// Reflect an angle around 0 degrees
template <typename T>
std::array<T, 2> flip(const std::array<T, 2>& c) {
    return {{
        static_cast<T>(2 * M_PI - c[0]),
        static_cast<T>(2 * M_PI - c[1])
    }};
}

template <typename T, typename S1, typename S2>
Point<T> normal(const S1& a, const S2& b) {
    T dx = b.x - a.x;
    T dy = b.y - a.y;
    T c = std::sqrt(dx * dx + dy * dy);
    return { dx / c, dy / c };
}

template <typename T>
T perp(const T& a) {
    return T(-a.y, a.x);
}

template <typename T, typename S1, typename S2>
T dist(const S1& a, const S2& b) {
    T dx = b.x - a.x;
    T dy = b.y - a.y;
    T c = std::sqrt(dx * dx + dy * dy);
    return c;
}

template <typename T, typename S1, typename S2>
T distSqr(const S1& a, const S2& b) {
    T dx = b.x - a.x;
    T dy = b.y - a.y;
    T c = dx * dx + dy * dy;
    return c;
}

template <typename T>
T round(const T& a) {
    return T(::round(a.x), ::round(a.y));
}

template <typename T>
T length(T a, T b) {
    return std::sqrt(a * a + b * b);
}

// Take the magnitude of vector a.
template <typename T = double, typename S>
T mag(const S& a) {
    return std::sqrt(a.x * a.x + a.y * a.y);
}

template <typename T = double, typename S>
S unit(const S& a) {
    auto magnitude = mag<T>(a);
    if (magnitude == 0) {
        return a;
    }
    return a * (1 / magnitude);
}

template <typename T, typename S = double>
T rotate(const T& a, S angle) {
    S cos = std::cos(angle);
    S sin = std::sin(angle);
    S x = cos * a.x - sin * a.y;
    S y = sin * a.x + cos * a.y;
    return T(x, y);
}

template <typename T>
Point<T> matrixMultiply(const std::array<T, 4>& m, const Point<T>& p) {
    return Point<T>(m[0] * p.x + m[1] * p.y, m[2] * p.x + m[3] * p.y);
}

template <typename T>
T smoothstep(T edge0, T edge1, T x) {
    T t = clamp((x - edge0) / (edge1 - edge0), T(0), T(1));
    return t * t * (T(3) - T(2) * t);
}

template <typename T>
inline T division(const T dividend, const T divisor, const T nan) {
    if (divisor == 0) {
        if (dividend == 0) {
            return nan;
        } else {
            return ::copysign(std::numeric_limits<T>::infinity(), dividend);
        }
    } else {
        return dividend / divisor;
    }
}

} // namespace util
} // namespace mbgl
