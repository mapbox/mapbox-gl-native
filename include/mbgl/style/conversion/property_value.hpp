#pragma once

#include <mbgl/style/property_value.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/function.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <class T>
struct Converter<PropertyValue<T>> {
    optional<PropertyValue<T>> operator()(const Value& value, Error& error) const {
        if (isUndefined(value)) {
            return PropertyValue<T>();
        } else if (isObject(value)) {
            optional<CameraFunction<T>> function = convert<CameraFunction<T>>(value, error);
            if (!function) {
                return {};
            }
            return { *function };
        } else {
            optional<T> constant = convert<T>(value, error);
            if (!constant) {
                return {};
            }
            return { *constant };
        }
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
