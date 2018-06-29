#pragma once

#include <mbgl/style/function/camera_function.hpp>
#include <mbgl/style/function/source_function.hpp>
#include <mbgl/style/function/composite_function.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/value.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <class T>
optional<optional<T>> convertDefaultValue(const Convertible& value, Error& error) {
    auto defaultValueValue = objectMember(value, "default");
    if (!defaultValueValue) {
        return optional<T>();
    }

    auto defaultValue = convert<T>(*defaultValueValue, error);
    if (!defaultValue) {
        error = { R"(wrong type for "default": )" + error.message };
        return {};
    }

    return { *defaultValue };
}

optional<std::unique_ptr<expression::Expression>> convertCameraFunctionToExpression(expression::type::Type, const Convertible&, Error&);
optional<std::unique_ptr<expression::Expression>> convertSourceFunctionToExpression(expression::type::Type, const Convertible&, Error&);
optional<std::unique_ptr<expression::Expression>> convertCompositeFunctionToExpression(expression::type::Type, const Convertible&, Error&);

template <class T>
struct Converter<CameraFunction<T>> {
    optional<CameraFunction<T>> operator()(const Convertible& value, Error& error) const {
        auto expression = convertCameraFunctionToExpression(expression::valueTypeToExpressionType<T>(), value, error);
        if (!expression) {
            return {};
        }
        return CameraFunction<T>(std::move(*expression), false);
    }
};

template <class T>
struct Converter<SourceFunction<T>> {
    optional<SourceFunction<T>> operator()(const Convertible& value, Error& error) const {
        auto expression = convertSourceFunctionToExpression(expression::valueTypeToExpressionType<T>(), value, error);
        if (!expression) {
            return {};
        }
        auto defaultValue = convertDefaultValue<T>(value, error);
        if (!defaultValue) {
            return {};
        }
        return SourceFunction<T>(std::move(*expression), *defaultValue);
    }
};

template <class T>
struct Converter<CompositeFunction<T>> {
    optional<CompositeFunction<T>> operator()(const Convertible& value, Error& error) const {
        auto expression = convertCompositeFunctionToExpression(expression::valueTypeToExpressionType<T>(), value, error);
        if (!expression) {
            return {};
        }
        auto defaultValue = convertDefaultValue<T>(value, error);
        if (!defaultValue) {
            return {};
        }
        return CompositeFunction<T>(std::move(*expression), *defaultValue);
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
