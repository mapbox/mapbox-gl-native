#pragma once

#include <mbgl/style/property_value.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/function.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/expression/value.hpp>
#include <mbgl/style/expression/is_constant.hpp>
#include <mbgl/style/expression/is_expression.hpp>
#include <mbgl/style/expression/parsing_context.hpp>
#include <mbgl/style/expression/literal.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <class T>
struct Converter<PropertyValue<T>> {
    optional<PropertyValue<T>> operator()(const Convertible& value, Error& error, bool allowDataExpressions, bool convertTokens) const;

    template <class S>
    PropertyValue<T> maybeConvertTokens(const S& t) const {
        return PropertyValue<T>(t);
    };

    PropertyValue<T> maybeConvertTokens(const std::string& t) const {
        return hasTokens(t)
            ? PropertyValue<T>(PropertyExpression<T>(convertTokenStringToExpression(t)))
            : PropertyValue<T>(t);
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
