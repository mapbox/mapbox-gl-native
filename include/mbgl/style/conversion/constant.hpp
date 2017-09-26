#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/util/optional.hpp>
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
    optional<bool> operator()(const Value& value, Error& error) const {
        optional<bool> converted = toBool(value);
        if (!converted) {
            error = { "value must be a boolean" };
            return {};
        }
        return *converted;
    }
};

template <>
struct Converter<float> {
    optional<float> operator()(const Value& value, Error& error) const {
        optional<float> converted = toNumber(value);
        if (!converted) {
            error = { "value must be a number" };
            return {};
        }
        return *converted;
    }
};

template <>
struct Converter<std::string> {
    optional<std::string> operator()(const Value& value, Error& error) const {
        optional<std::string> converted = toString(value);
        if (!converted) {
            error = { "value must be a string" };
            return {};
        }
        return *converted;
    }
};

template <class T>
struct Converter<T, typename std::enable_if_t<std::is_enum<T>::value>> {
    optional<T> operator()(const Value& value, Error& error) const {
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
    optional<Color> operator()(const Value& value, Error& error) const {
        optional<std::string> string = toString(value);
        if (!string) {
            error = { "value must be a string" };
            return {};
        }

        optional<Color> color = Color::parse(*string);
        if (!color) {
            error = { "value must be a valid color" };
            return {};
        }

        return *color;
    }
};

template <size_t N>
struct Converter<std::array<float, N>> {
    optional<std::array<float, N>> operator()(const Value& value, Error& error) const {
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
    optional<std::vector<float>> operator()(const Value& value, Error& error) const {
        if (!isArray(value)) {
            error = { "value must be an array" };
            return {};
        }

        std::vector<float> result;
        result.reserve(arrayLength(value));

        for (std::size_t i = 0; i < arrayLength(value); ++i) {
            optional<float> number = toNumber(arrayMember(value, i));
            if (!number) {
                error = { "value must be an array of numbers" };
                return {};
            }
            result.push_back(*number);
        }

        return result;
    }
};

template <>
struct Converter<std::vector<std::string>> {
    optional<std::vector<std::string>> operator()(const Value& value, Error& error) const {
        if (!isArray(value)) {
            error = { "value must be an array" };
            return {};
        }

        std::vector<std::string> result;
        result.reserve(arrayLength(value));

        for (std::size_t i = 0; i < arrayLength(value); ++i) {
            optional<std::string> string = toString(arrayMember(value, i));
            if (!string) {
                error = { "value must be an array of strings" };
                return {};
            }
            result.push_back(*string);
        }

        return result;
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
