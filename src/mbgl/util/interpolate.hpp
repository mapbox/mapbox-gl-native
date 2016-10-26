#pragma once

#include <array>
#include <vector>
#include <string>
#include <type_traits>
#include <utility>
#include <mbgl/util/color.hpp>

namespace mbgl {
namespace util {

template <class T, class Enabled = void>
struct Interpolator;

template <typename T>
T interpolate(const T& a, const T& b, const double t) {
    return Interpolator<T>()(a, b, t);
}


template <class T, class Enabled>
struct Interpolator {
    T operator()(const T& a, const T& b, const double t) const {
        return a * (1.0 - t) + b * t;
    }
};

template <class T, std::size_t N>
struct Interpolator<std::array<T, N>> {
private:
    using Array = std::array<T, N>;

    template <std::size_t... I>
    Array operator()(const Array& a, const Array& b, const double t, std::index_sequence<I...>) {
        return {{ interpolate(a[I], b[I], t)... }};
    }

public:
    Array operator()(const Array& a, const Array& b, const double t) {
        return operator()(a, b, t, std::make_index_sequence<N>());
    }
};

template <>
struct Interpolator<Color> {
public:
    Color operator()(const Color& a, const Color& b, const double t) {
        return {
            interpolate(a.r, b.r, t),
            interpolate(a.g, b.g, t),
            interpolate(a.b, b.b, t),
            interpolate(a.a, b.a, t)
        };
    }
};

template <>
struct Interpolator<ColorLAB> {
public:
    ColorLAB operator()(const ColorLAB& a, const ColorLAB& b, const double t) {
        return {
            interpolate(a.l, b.l, t),
            interpolate(a.a, b.a, t),
            interpolate(a.b, b.b, t),
            interpolate(a.A, b.A, t)
        };
    }
};

template <>
struct Interpolator<ColorHCL> {
public:
    ColorHCL operator()(const ColorHCL& a, const ColorHCL& b, const double t) {
        return {
            interpolate(a.h, b.h, t),
            interpolate(a.c, b.c, t),
            interpolate(a.l, b.l, t),
            interpolate(a.A, b.A, t)
        };
    }
};

struct Uninterpolated {
    template <class T>
    T operator()(const T& a, const T&, const double) const {
        return a;
    }
};

template <class T>
struct Interpolator<T, typename std::enable_if_t<std::is_enum<T>::value>>
    : Uninterpolated {};

template <>
struct Interpolator<std::string>
    : Uninterpolated {};

template <class T>
struct Interpolator<std::vector<T>>
    : Uninterpolated {};

} // namespace util
} // namespace mbgl
