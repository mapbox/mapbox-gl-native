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
    optional<DataDrivenPropertyValue<T>> operator()(const Value& value, Error& error) const {
        if (isUndefined(value)) {
            return DataDrivenPropertyValue<T>();
        } else if (!isObject(value)) {
            optional<T> constant = convert<T>(value, error);
            if (!constant) {
                return {};
            }
            return DataDrivenPropertyValue<T>(*constant);
        } else if (!objectMember(value, "property")) {
            optional<CameraFunction<T>> function = convert<CameraFunction<T>>(value, error);
            if (!function) {
                return {};
            }
            return DataDrivenPropertyValue<T>(*function);
        } else {
            optional<CompositeFunction<T>> composite = convert<CompositeFunction<T>>(value, error);
            if (composite) {
                return DataDrivenPropertyValue<T>(*composite);
            }
            optional<SourceFunction<T>> source = convert<SourceFunction<T>>(value, error);
            if (!source) {
                return {};
            }
            return DataDrivenPropertyValue<T>(*source);
        }
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
