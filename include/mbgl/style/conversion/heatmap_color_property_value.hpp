#pragma once

#include <mbgl/style/heatmap_color_property_value.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/function.hpp>
#include <mbgl/style/expression/value.hpp>
#include <mbgl/style/expression/is_constant.hpp>
#include <mbgl/style/expression/is_expression.hpp>
#include <mbgl/style/expression/find_zoom_curve.hpp>
#include <mbgl/style/expression/parsing_context.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<HeatmapColorPropertyValue> {
    optional<HeatmapColorPropertyValue> operator()(const Convertible& value, Error& error) const {
        using namespace mbgl::style::expression;
        if (isUndefined(value)) {
            return HeatmapColorPropertyValue();
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
            return {HeatmapColorPropertyValue(std::move(*expression))};
        } else {
            error = { "heatmap-color must be an expression" };
            return {};
        }
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
