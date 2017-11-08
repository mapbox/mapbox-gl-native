#pragma once

#include <mbgl/style/property_value.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/function.hpp>
#include <mbgl/style/conversion/expression.hpp>
#include <mbgl/style/expression/value.hpp>
#include <mbgl/style/expression/is_constant.hpp>
#include <mbgl/style/expression/is_expression.hpp>
#include <mbgl/style/expression/find_zoom_curve.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <class T>
struct Converter<PropertyValue<T>> {
    optional<PropertyValue<T>> operator()(const Convertible& value, Error& error) const {
        if (isUndefined(value)) {
            return PropertyValue<T>();
        } else if (isExpression(value)) {
            optional<std::unique_ptr<Expression>> expression = convert<std::unique_ptr<Expression>>(value, error, valueTypeToExpressionType<T>());
            if (!expression) {
                return {};
            }
            if (isFeatureConstant(**expression)) {
                return { CameraFunction<T>(std::move(*expression)) };
            } else {
                error = { "property expressions not supported" };
                return {};
            }
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
