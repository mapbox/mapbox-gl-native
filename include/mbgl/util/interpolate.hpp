#pragma once

#include <mbgl/util/color.hpp>
#include <mbgl/util/range.hpp>
#include <mbgl/style/position.hpp>
#include <mbgl/style/expression/value.hpp>

#include <array>
#include <vector>
#include <string>
#include <type_traits>
#include <utility>

namespace mbgl {
namespace util {

float interpolationFactor(float base, Range<float> range, float z);

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


// In order to accept Array<Number, N> as an output value for Curve
// expressions, we need to have an interpolatable std::vector type.
// However, style properties like line-dasharray are represented using
// std::vector<float>, and should NOT be considered interpolatable.
// So, we use std::vector<Value> to represent expression array values,
// asserting that (a) the vectors are the same size, and (b) they contain
// only numeric values.  (These invariants should be relatively safe,
// being enforced by the expression type system.)
template<>
struct Interpolator<std::vector<style::expression::Value>> {
    std::vector<style::expression::Value> operator()(const std::vector<style::expression::Value>& a,
                                  const std::vector<style::expression::Value>& b,
                                  const double t) const {
        assert(a.size() == b.size());
        if (a.size() == 0) return {};
        std::vector<style::expression::Value> result;
        for (std::size_t i = 0; i < a.size(); i++) {
            assert(a[i].template is<double>());
            assert(b[i].template is<double>());
            style::expression::Value item = interpolate(
                a[i].template get<double>(),
                b[i].template get<double>(),
                t);
            result.push_back(item);
        }
        return result;
    }
};

template <>
struct Interpolator<style::Position> {
public:
    style::Position operator()(const style::Position& a, const style::Position& b, const double t) {
        auto pos = style::Position();
        auto interpolated = interpolate(a.getCartesian(), b.getCartesian(), t);
        pos.setCartesian(interpolated);
        return { pos };
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

struct Uninterpolated {
    template <class T>
    T operator()(const T& a, const T&, const double) const {
        return a;
    }
};

template <>
struct Interpolator<bool>
    : Uninterpolated {};

template <class T>
struct Interpolator<T, typename std::enable_if_t<std::is_enum<T>::value>>
    : Uninterpolated {};

template <>
struct Interpolator<std::string>
    : Uninterpolated {};

template <class T>
struct Interpolator<std::vector<T>>
    : Uninterpolated {};

template <class T>
struct Interpolatable
    : std::conditional_t<
      !std::is_base_of<Uninterpolated, Interpolator<T>>::value,
      std::true_type,
      std::false_type> {};



} // namespace util
} // namespace mbgl
