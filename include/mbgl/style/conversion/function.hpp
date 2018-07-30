#pragma once

#include <mbgl/style/property_expression.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/value.hpp>

namespace mbgl {
namespace style {
namespace conversion {

bool hasTokens(const std::string&);
std::unique_ptr<expression::Expression> convertTokenStringToExpression(const std::string&);

optional<std::unique_ptr<expression::Expression>> convertFunctionToExpression(expression::type::Type, const Convertible&, Error&, bool convertTokens);

template <class T>
optional<PropertyExpression<T>> convertFunctionToExpression(const Convertible& value, Error& error, bool convertTokens) {
    auto expression = convertFunctionToExpression(expression::valueTypeToExpressionType<T>(), value, error, convertTokens);
    if (!expression) {
        return {};
    }

    optional<T> defaultValue;

    auto defaultValueValue = objectMember(value, "default");
    if (defaultValueValue) {
        defaultValue = convert<T>(*defaultValueValue, error);
        if (!defaultValue) {
            error = { R"(wrong type for "default": )" + error.message };
            return {};
        }
    }

    return PropertyExpression<T>(std::move(*expression), defaultValue);
}

} // namespace conversion
} // namespace style
} // namespace mbgl
