#pragma once

#include <mbgl/style/property_value.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/function.hpp>
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
    optional<PropertyValue<T>> operator()(const Convertible& value, Error& error, bool allowDataExpressions, bool convertTokens) const {
        using namespace mbgl::style::expression;

        if (isUndefined(value)) {
            return PropertyValue<T>();
        }

        optional<PropertyExpression<T>> expression;

        if (isExpression(value)) {
            ParsingContext ctx(valueTypeToExpressionType<T>());
            ParseResult parsed = ctx.parseLayerPropertyExpression(value);
            if (!parsed) {
                error.message = ctx.getCombinedErrors();
                return nullopt;
            }
            expression = PropertyExpression<T>(std::move(*parsed));
        } else if (isObject(value)) {
            expression = convertFunctionToExpression<T>(value, error, convertTokens);
        } else {
            optional<T> constant = convert<T>(value, error);
            if (!constant) {
                return nullopt;
            }
            return convertTokens ? maybeConvertTokens(*constant) : PropertyValue<T>(*constant);
        }

        if (!expression) {
            return nullopt;
        } else if (!allowDataExpressions && !(*expression).isFeatureConstant()) {
            error.message = "data expressions not supported";
            return nullopt;
        } else if (!(*expression).isFeatureConstant() || !(*expression).isZoomConstant()) {
            return { std::move(*expression) };
        } else if ((*expression).getExpression().getKind() == Kind::Literal) {
            optional<T> constant = fromExpressionValue<T>(
                static_cast<const Literal&>((*expression).getExpression()).getValue());
            if (!constant) {
                return nullopt;
            }
            return PropertyValue<T>(*constant);
        } else {
            assert(false);
            error.message = "expected a literal expression";
            return nullopt;
        }
    }

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
