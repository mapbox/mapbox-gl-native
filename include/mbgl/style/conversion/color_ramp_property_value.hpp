#pragma once

#include <mbgl/style/color_ramp_property_value.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/expression/value.hpp>
#include <mbgl/style/expression/is_constant.hpp>
#include <mbgl/style/expression/is_expression.hpp>
#include <mbgl/style/expression/parsing_context.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<ColorRampPropertyValue> {
    optional<ColorRampPropertyValue> operator()(const Convertible& value, Error& error, bool /* convertTokens */ = false) const {
        using namespace mbgl::style::expression;
        if (isUndefined(value)) {
            return ColorRampPropertyValue();
        } else if (isExpression(value)) {
            ParsingContext ctx(type::Color);
            ParseResult expression = ctx.parseLayerPropertyExpression(value);
            if (!expression) {
                error = { ctx.getCombinedErrors() };
                return {};
            }
            assert(*expression);
            if (!isFeatureConstant(**expression)) {
                error = { "property expressions not supported" };
                return {};
            }
            if (!isZoomConstant(**expression)) {
                error = { "zoom expressions not supported" };
                return {};
            }
            return {ColorRampPropertyValue(std::move(*expression))};
        } else {
            error = { "color ramp must be an expression" };
            return {};
        }
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
