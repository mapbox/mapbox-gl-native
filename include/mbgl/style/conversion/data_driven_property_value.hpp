#pragma once

#include <mbgl/style/data_driven_property_value.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/function.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <class T>
struct Converter<DataDrivenPropertyValue<T>> {
    template <class V>
    Result<DataDrivenPropertyValue<T>> operator()(const V& value) const {
        if (isUndefined(value)) {
            return {};
        } else if (!isObject(value)) {
            Result<T> constant = convert<T>(value);
            if (!constant) {
                return constant.error();
            }
            return DataDrivenPropertyValue<T>(*constant);
        } else if (!objectMember(value, "property")) {
            Result<CameraFunction<T>> function = convert<CameraFunction<T>>(value);
            if (!function) {
                return function.error();
            }
            return DataDrivenPropertyValue<T>(*function);
        } else {
            Result<CompositeFunction<T>> composite = convert<CompositeFunction<T>>(value);
            if (composite) {
                return DataDrivenPropertyValue<T>(*composite);
            }
            Result<SourceFunction<T>> source = convert<SourceFunction<T>>(value);
            if (!source) {
                return source.error();
            }
            return DataDrivenPropertyValue<T>(*source);
        }
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
