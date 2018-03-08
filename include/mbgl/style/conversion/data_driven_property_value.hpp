#pragma once

#include <mbgl/style/data_driven_property_value.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/function.hpp>
#include <mbgl/style/expression/is_expression.hpp>
#include <mbgl/style/expression/is_constant.hpp>
#include <mbgl/style/expression/find_zoom_curve.hpp>
#include <mbgl/style/expression/literal.hpp>
#include <mbgl/style/expression/value.hpp>
#include <mbgl/style/expression/parsing_context.hpp>

#include <unordered_set>


namespace mbgl {
namespace style {
namespace conversion {

template <class T>
struct Converter<DataDrivenPropertyValue<T>> {

    optional<DataDrivenPropertyValue<T>> operator()(const Convertible& value, Error& error) const {
        using namespace mbgl::style::expression;
        
        if (isUndefined(value)) {
            return DataDrivenPropertyValue<T>();
        } else if (isExpression(value)) {
            ParsingContext ctx(valueTypeToExpressionType<T>());
            ParseResult expression = ctx.parseLayerPropertyExpression(value);
            if (!expression) {
                error = { ctx.getCombinedErrors() };
                return {};
            }
            
            bool featureConstant = isFeatureConstant(**expression);
            bool zoomConstant = isZoomConstant(**expression);
            
            if (featureConstant && !zoomConstant) {
                return DataDrivenPropertyValue<T>(CameraFunction<T>(std::move(*expression)));
            } else if (!featureConstant && zoomConstant) {
                return DataDrivenPropertyValue<T>(SourceFunction<T>(std::move(*expression)));
            } else if (!featureConstant && !zoomConstant) {
                return DataDrivenPropertyValue<T>(CompositeFunction<T>(std::move(*expression)));
            } else {
                auto literal = dynamic_cast<Literal*>(expression->get());
                assert(literal);
                optional<T> constant = fromExpressionValue<T>(literal->getValue());
                if (!constant) {
                    return {};
                }
                return DataDrivenPropertyValue<T>(*constant);
            }
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
