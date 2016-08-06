#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/enum.hpp>

#include <array>
#include <string>
#include <vector>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<bool> {
    template <class V>
    Result<bool> operator()(const V& value) const {
        optional<bool> converted = toBool(value);
        if (!converted) {
            return Error { "value must be a boolean" };
        }
        return *converted;
    }
};

template <>
struct Converter<float> {
    template <class V>
    Result<float> operator()(const V& value) const {
        optional<float> converted = toNumber(value);
        if (!converted) {
            return Error { "value must be a number" };
        }
        return *converted;
    }
};

template <>
struct Converter<std::string> {
    template <class V>
    Result<std::string> operator()(const V& value) const {
        optional<std::string> converted = toString(value);
        if (!converted) {
            return Error { "value must be a string" };
        }
        return *converted;
    }
};

template <class T>
struct Converter<T, typename std::enable_if_t<std::is_enum<T>::value>> {
    template <class V>
    Result<T> operator()(const V& value) const {
        optional<std::string> string = toString(value);
        if (!string) {
            return Error { "value must be a string" };
        }

        const auto result = Enum<T>::toEnum(*string);
        if (!result) {
            return Error { "value must be a valid enumeration value" };
        }

        return *result;
    }
};

template <>
struct Converter<Color> {
    template <class V>
    Result<Color> operator()(const V& value) const {
        optional<std::string> string = toString(value);
        if (!string) {
            return Error { "value must be a string" };
        }

        optional<Color> color = Color::parse(*string);
        if (!color) {
            return Error { "value must be a valid color" };
        }

        return *color;
    }
};

template <>
struct Converter<std::array<float, 2>> {
    template <class V>
    Result<std::array<float, 2>> operator()(const V& value) const {
        if (!isArray(value) || arrayLength(value) != 2) {
            return Error { "value must be an array of two numbers" };
        }

        optional<float> first = toNumber(arrayMember(value, 0));
        optional<float> second = toNumber(arrayMember(value, 1));
        if (!first || !second) {
            return Error { "value must be an array of two numbers" };
        }

        return std::array<float, 2> {{ *first, *second }};
    }
};

template <>
struct Converter<std::array<float, 4>> {
    template <class V>
    Result<std::array<float, 4>> operator()(const V& value) const {
        if (!isArray(value) || arrayLength(value) != 4) {
            return Error { "value must be an array of four numbers" };
        }

        optional<float> first = toNumber(arrayMember(value, 0));
        optional<float> second = toNumber(arrayMember(value, 1));
        optional<float> third = toNumber(arrayMember(value, 2));
        optional<float> fourth = toNumber(arrayMember(value, 3));
        if (!first || !second) {
            return Error { "value must be an array of four numbers" };
        }

        return std::array<float, 4> {{ *first, *second, *third, *fourth }};
    }
};

template <>
struct Converter<std::vector<float>> {
    template <class V>
    Result<std::vector<float>> operator()(const V& value) const {
        if (!isArray(value)) {
            return Error { "value must be an array" };
        }

        std::vector<float> result;
        result.reserve(arrayLength(value));

        for (std::size_t i = 0; i < arrayLength(value); ++i) {
            optional<float> number = toNumber(arrayMember(value, i));
            if (!number) {
                return Error { "value must be an array of numbers" };
            }
            result.push_back(*number);
        }

        return result;
    }
};

template <>
struct Converter<std::vector<std::string>> {
    template <class V>
    Result<std::vector<std::string>> operator()(const V& value) const {
        if (!isArray(value)) {
            return Error { "value must be an array" };
        }

        std::vector<std::string> result;
        result.reserve(arrayLength(value));

        for (std::size_t i = 0; i < arrayLength(value); ++i) {
            optional<std::string> string = toString(arrayMember(value, i));
            if (!string) {
                return Error { "value must be an array of strings" };
            }
            result.push_back(*string);
        }

        return result;
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
