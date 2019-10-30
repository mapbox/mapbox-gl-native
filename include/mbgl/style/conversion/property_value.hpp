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
    
    PropertyValue<T> maybeConvertTokens(const expression::Formatted& t) const {
        // This only works with a single-section `Formatted` created automatically
        // by parsing a plain-text `text-field` property.
        // Token conversion happens later than the initial string->Formatted conversion
        // General purpose `format` expressions with embedded tokens are not supported
        const std::string& firstUnformattedSection = t.sections[0].text;
        return hasTokens(firstUnformattedSection)
            ? PropertyValue<T>(PropertyExpression<T>(convertTokenStringToFormatExpression(firstUnformattedSection)))
            : PropertyValue<T>(t);
    }

    PropertyValue<T> maybeConvertTokens(const expression::Image& image) const {
        return hasTokens(image.id())
                   ? PropertyValue<T>(PropertyExpression<T>(convertTokenStringToImageExpression(image.id())))
                   : PropertyValue<T>(image);
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
