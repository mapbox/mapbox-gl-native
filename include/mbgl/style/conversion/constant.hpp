#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/enum.hpp>
#include <mbgl/util/string.hpp>

#include <array>
#include <string>
#include <vector>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<bool> {
    optional<bool> operator()(const Convertible& value, Error& error) const;
};

template <>
struct Converter<float> {
    optional<float> operator()(const Convertible& value, Error& error) const;
};

template <>
struct Converter<std::string> {
    optional<std::string> operator()(const Convertible& value, Error& error) const;
};

template <class T>
struct Converter<T, typename std::enable_if_t<std::is_enum<T>::value>> {
    optional<T> operator()(const Convertible& value, Error& error) const;
};

template <class T>
struct Converter<std::vector<T>, typename std::enable_if_t<std::is_enum<T>::value>> {
    optional<std::vector<T>> operator()(const Convertible& value, Error& error) const;
};

template <>
struct Converter<Color> {
    optional<Color> operator()(const Convertible& value, Error& error) const;
};

template <size_t N>
struct Converter<std::array<float, N>> {
    optional<std::array<float, N>> operator()(const Convertible& value, Error& error) const;
};

template <>
struct Converter<std::vector<float>> {
    optional<std::vector<float>> operator()(const Convertible& value, Error& error) const;
};

template <>
struct Converter<std::vector<std::string>> {
    optional<std::vector<std::string>> operator()(const Convertible& value, Error& error) const;
};

} // namespace conversion
} // namespace style
} // namespace mbgl
