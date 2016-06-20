#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/transition_options.hpp>

#include <mbgl/util/variant.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/enum.hpp>

#include <array>
#include <string>
#include <vector>

namespace mbgl {
namespace style {
namespace conversion {

/*
   This namespace defines a generic conversion from a templated type `V` representing a JSON object conforming
   to a style property from the Mapbox Style Specification, to `PropertyValue<T>`:

       template <class T, class V>
       Result<PropertyValue<T>> convertPropertyValue(const V& value);

   This is used concretely for conversions from RapidJSON types in mbgl core, and from v8 types in
   the node bindings.

   The requirements are that the following are legal expressions for a value `v` of type `const V&`:

      * `isArray(v)` -- returns a boolean indicating whether `v` represents a JSON array
      * `arrayLength(v)` -- called only if `isArray(v)`; returns a size_t length
      * `arrayMember(v)` -- called only if `isArray(v)`; returns `V` or `V&`

      * `isObject(v)` -- returns a boolean indicating whether `v` represents a JSON object
      * `objectMember(v, name)` -- called only if `isObject(v)`; `name` is `const char *`; return value:
         * is true when evaluated in a boolean context iff the named member exists
         * is convertable to a `V` or `V&` when dereferenced

      * `toBool(v)` -- returns `optional<bool>`, absence indicating `v` is not a JSON boolean
      * `toNumber(v)` -- returns `optional<float>`, absence indicating `v` is not a JSON number
      * `toString(v)` -- returns `optional<std::string>`, absence indicating `v` is not a JSON string

   If for any reason the conversion fails, the result of `convertPropertyValue` will be the `Error` variant,
   which includes explanatory text.
*/

struct Error { const char * message; };

template <class V>
using Result = variant<V, Error>;

template <class V, class T, class Enable = void>
struct ConstantConverter {};

template <class V>
struct ConstantConverter<V, bool> {
    Result<bool> operator()(const V& value) const {
        optional<bool> converted = toBool(value);
        if (!converted) {
            return Error { "value must be a boolean" };
        }
        return *converted;
    }
};

template <class V>
struct ConstantConverter<V, float> {
    Result<float> operator()(const V& value) const {
        optional<float> converted = toNumber(value);
        if (!converted) {
            return Error { "value must be a number" };
        }
        return *converted;
    }
};

template <class V>
struct ConstantConverter<V, std::string> {
    Result<std::string> operator()(const V& value) const {
        optional<std::string> converted = toString(value);
        if (!converted) {
            return Error { "value must be a string" };
        }
        return *converted;
    }
};

template <class V, class T>
struct ConstantConverter<V, T, typename std::enable_if_t<std::is_enum<T>::value>> {
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

template <class V>
struct ConstantConverter<V, Color> {
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

template <class V>
struct ConstantConverter<V, std::array<float, 2>> {
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

template <class V>
struct ConstantConverter<V, std::array<float, 4>> {
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

template <class V>
struct ConstantConverter<V, std::vector<float>> {
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

template <class V>
struct ConstantConverter<V, std::vector<std::string>> {
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

template <class T, class V>
Result<PropertyValue<T>> convertPropertyValue(const V& value) {
    if (!isObject(value)) {
        Result<T> constant = ConstantConverter<V, T>()(value);
        if (constant.template is<Error>()) {
            return constant.template get<Error>();
        }
        return constant.template get<T>();
    }

    auto stopsValue = objectMember(value, "stops");
    if (!stopsValue) {
        return Error { "function value must specify stops" };
    }

    if (!isArray(*stopsValue)) {
        return Error { "function stops must be an array" };
    }

    std::vector<std::pair<float, T>> stops;
    for (std::size_t i = 0; i < arrayLength(*stopsValue); ++i) {
        const auto& stopValue = arrayMember(*stopsValue, i);

        if (!isArray(stopValue)) {
            return Error { "function stop must be an array" };
        }

        if (arrayLength(stopValue) != 2) {
            return Error { "function stop must have two elements" };
        }

        optional<float> z = toNumber(arrayMember(stopValue, 0));
        if (!z) {
            return Error { "function stop zoom level must be a number" };
        }

        Result<T> v = ConstantConverter<V, T>()(arrayMember(stopValue, 1));
        if (v.template is<Error>()) {
            return v.template get<Error>();
        }

        stops.emplace_back(*z, v.template get<T>());
    }

    auto baseValue = objectMember(value, "base");
    if (!baseValue) {
        return Function<T>(stops, 1.0f);
    }

    optional<float> base = toNumber(*baseValue);
    if (!base) {
        return Error { "function base must be a number"};
    }

    return Function<T>(stops, *base);
}

} // namespace conversion
} // namespace style
} // namespace mbgl
