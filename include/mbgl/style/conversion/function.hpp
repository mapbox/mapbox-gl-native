#pragma once

#include <mbgl/style/function.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/constant.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <class T>
struct Converter<Function<T>> {
    template <class V>
    Result<Function<T>> operator()(const V& value) const {
        if (!isObject(value)) {
            return Error { "function must be an object" };
        }

        auto stopsValue = objectMember(value, "stops");
        if (!stopsValue) {
            return Error { "function value must specify stops" };
        }

        if (!isArray(*stopsValue)) {
            return Error { "function stops must be an array" };
        }

        if (arrayLength(*stopsValue) == 0) {
            return Error { "function must have at least one stop" };
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

            Result<T> v = convert<T>(arrayMember(stopValue, 1));
            if (!v) {
                return v.error();
            }

            stops.emplace_back(*z, *v);
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
};

} // namespace conversion
} // namespace style
} // namespace mbgl
