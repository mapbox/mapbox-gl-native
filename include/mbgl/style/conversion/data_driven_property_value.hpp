#pragma once

#include <mbgl/style/data_driven_property_value.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/function.hpp>
#include <mbgl/style/expression/is_expression.hpp>
#include <mbgl/style/expression/is_constant.hpp>
#include <mbgl/style/expression/literal.hpp>
#include <mbgl/style/expression/value.hpp>
#include <mbgl/style/expression/parsing_context.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <class T>
struct Converter<DataDrivenPropertyValue<T>> {
    optional<DataDrivenPropertyValue<T>> operator()(const Convertible& value, Error& error, bool convertTokens) const {
        using namespace mbgl::style::expression;

        if (isUndefined(value)) {
            return DataDrivenPropertyValue<T>();
        }

        optional<PropertyExpression<T>> expression;

        if (isExpression(value)) {
            ParsingContext ctx(valueTypeToExpressionType<T>());
            ParseResult parsed = ctx.parseLayerPropertyExpression(value);
            if (!parsed) {
                error = { ctx.getCombinedErrors() };
                return {};
            }
            expression = PropertyExpression<T>(std::move(*parsed));
        } else if (isObject(value)) {
            expression = convertFunctionToExpression<T>(value, error, convertTokens);
        } else {
            optional<T> constant = convert<T>(value, error);
            if (!constant) {
                return {};
            }
            return convertTokens ? maybeConvertTokens(*constant) : DataDrivenPropertyValue<T>(*constant);
        }

        if (!expression) {
            return {};
        } else if (!(*expression).isFeatureConstant() || !(*expression).isZoomConstant()) {
            return { std::move(*expression) };
        } else if ((*expression).getExpression().getKind() == Kind::Literal) {
            optional<T> constant = fromExpressionValue<T>(
                static_cast<const Literal&>((*expression).getExpression()).getValue());
            if (!constant) {
                return {};
            }
            return DataDrivenPropertyValue<T>(*constant);
        } else {
            assert(false);
            error = { "expected a literal expression" };
            return {};
        }
    }

    template <class S>
    DataDrivenPropertyValue<T> maybeConvertTokens(const S& t) const {
        return DataDrivenPropertyValue<T>(t);
    };

    DataDrivenPropertyValue<T> maybeConvertTokens(const std::string& t) const {
        return hasTokens(t)
            ? DataDrivenPropertyValue<T>(PropertyExpression<T>(convertTokenStringToExpression(t)))
            : DataDrivenPropertyValue<T>(t);
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
