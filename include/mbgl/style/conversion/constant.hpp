#pragma once

#include <mbgl/style/conversion.hpp>
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
    optional<T> operator()(const Convertible& value, Error& error) const {
        optional<std::string> string = toString(value);
        if (!string) {
            error = { "value must be a string" };
            return {};
        }

        const auto result = Enum<T>::toEnum(*string);
        if (!result) {
            error = { "value must be a valid enumeration value" };
            return {};
        }

        return *result;
    }
};

template <>
struct Converter<Color> {
    optional<Color> operator()(const Convertible& value, Error& error) const;
};

template <size_t N>
struct Converter<std::array<float, N>> {
    optional<std::array<float, N>> operator()(const Convertible& value, Error& error) const {
        if (!isArray(value) || arrayLength(value) != N) {
            error = { "value must be an array of " + util::toString(N) + " numbers" };
            return {};
        }

        std::array<float, N> result;
        for (size_t i = 0; i < N; i++) {
            optional<float> n = toNumber(arrayMember(value, i));
            if (!n) {
                error = { "value must be an array of " + util::toString(N) + " numbers" };
                return {};
            }
            result[i] = *n;
        }
        return result;
    }
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
