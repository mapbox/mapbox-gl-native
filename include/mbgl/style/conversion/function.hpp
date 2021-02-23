#pragma once

#include <mbgl/style/property_expression.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/value.hpp>

namespace mbgl {
namespace style {
namespace conversion {

bool hasTokens(const std::string&);
std::unique_ptr<expression::Expression> convertTokenStringToFormatExpression(const std::string&);
std::unique_ptr<expression::Expression> convertTokenStringToImageExpression(const std::string&);
std::unique_ptr<expression::Expression> convertTokenStringToExpression(const std::string&);

optional<std::unique_ptr<expression::Expression>> convertFunctionToExpression(expression::type::Type, const Convertible&, Error&, bool convertTokens);

template <class T>
optional<PropertyExpression<T>> convertFunctionToExpression(const Convertible& value, Error& error, bool convertTokens);

} // namespace conversion
} // namespace style
} // namespace mbgl
