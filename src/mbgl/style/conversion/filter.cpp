#include <mbgl/style/conversion/filter.hpp>
#include <mbgl/style/expression/literal.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/type.hpp>
#include <mbgl/style/expression/compound_expression.hpp>
#include <mbgl/style/expression/boolean_operator.hpp>

namespace mbgl {
namespace style {
namespace conversion {
    
static bool isExpression(const Convertible& filter);
    std::unique_ptr<expression::Expression> convertLegacyFilter(const Convertible& values, Error& error);

optional<Filter> Converter<Filter>::operator()(const Convertible& value, Error& error) const {
    
    optional<std::unique_ptr<expression::Expression>> expression;
    
    if (isExpression(value)) {
        expression::ParsingContext parsingContext(expression::type::Boolean);
        expression::ParseResult parseResult = parsingContext.parseExpression(value);
        if (!parseResult) {
            error = { parsingContext.getCombinedErrors() };
            return {};
        }
        expression = std::move(*parseResult);
    } else {
        expression = convertLegacyFilter(value, error);
    }
    
    if (!expression) return {};
    else return { Filter { std::move(*expression) } };
}

// This is a port from https://github.com/mapbox/mapbox-gl-js/blob/master/src/style-spec/feature_filter/index.js
static bool isExpression(const Convertible& filter) {
    if (!isArray(filter) || arrayLength(filter) == 0) {
        return false;
    }
    
    optional<std::string> op = toString(arrayMember(filter, 0));
    
    if (!op) {
        return false;
        
    } else if (*op == "has") {
        if (arrayLength(filter) < 2) return false;
        optional<std::string> operand = toString(arrayMember(filter, 1));
        return operand && *operand != "$id" && *operand != "$type";
        
    } else if (*op == "in" || *op == "!in" || *op == "!has" || *op == "none") {
        return false;
        
    } else if (*op == "==" || *op == "!=" || *op == ">" || *op == ">=" || *op == "<" || *op == "<=") {
        return arrayLength(filter) == 3 && (isArray(arrayMember(filter, 1)) || isArray(arrayMember(filter, 2)));
        
    } else if (*op == "any" || *op == "all") {
        for (std::size_t i = 1; i < arrayLength(filter); i++) {
            Convertible f = arrayMember(filter, i);
            if (!isExpression(f) && !toBool(f)) {
                return false;
            }
        }
        return true;
        
    } else {
        return true;
    }
}
    
std::unique_ptr<expression::Expression> createExpression(std::string op, std::vector<std::unique_ptr<expression::Expression>> args, Error& error) {
    if (op == "any") {
        return std::make_unique<expression::Any>(std::move(args));
        
    } else if (op == "all") {
        return std::make_unique<expression::All>(std::move(args));

    } else {
        expression::ParsingContext parsingContext(expression::type::Boolean);
        expression::ParseResult parseResult = createCompoundExpression(op, std::move(args), parsingContext);
        if (!parseResult) {
            error = { parsingContext.getCombinedErrors() };
            return {};
        } else {
            return std::move(*parseResult);
        }
    }
}

std::unique_ptr<expression::Expression> createExpression(std::string op, std::unique_ptr<expression::Expression> expression, Error& error) {
    std::vector<std::unique_ptr<expression::Expression>> args;
    args.push_back(std::move(expression));
    return createExpression(op, std::move(args), error);
}
    
std::unique_ptr<expression::Expression> convertLiteral(const Convertible& convertible) {
    optional<mapbox::geometry::value> value = toValue(convertible);
    expression::Value expressionValue = value ? expression::toExpressionValue(*value) : expression::Null;
    return std::make_unique<expression::Literal>(expressionValue);
}

std::vector<std::unique_ptr<expression::Expression>> convertLiteralArray(const Convertible &input, std::size_t startIndex = 0) {
    std::vector<std::unique_ptr<expression::Expression>> output;
    for (std::size_t i = startIndex; i < arrayLength(input); i++) {
        output.push_back(convertLiteral(arrayMember(input, i)));
    }
    return output;
}

std::unique_ptr<expression::Expression> convertLegacyComparisonFilter(const Convertible& values, Error& error, optional<std::string> opOverride = {}) {
    optional<std::string> op = opOverride ? opOverride : toString(arrayMember(values, 0));
    optional<std::string> property = toString(arrayMember(values, 1));
    
    if (!property) {
        error = { "filter property must be a string" };
        return {};
    } else if (*property == "$type") {
        return createExpression("filter-type-" + *op, convertLiteralArray(values, 2), error);
    } else if (*property == "$id") {
        return createExpression("filter-id-" + *op, convertLiteralArray(values, 2), error);
    } else {
        return createExpression("filter-" + *op, convertLiteralArray(values, 1), error);
    }
}
    
std::unique_ptr<expression::Expression> convertLegacyHasFilter(std::string property, Error& error) {
    if (property == "$type") {
        return std::make_unique<expression::Literal>(true);
    } else if (property == "$id") {
        return createExpression("filter-has-id", std::vector<std::unique_ptr<expression::Expression>>(), error);
    } else {
        return createExpression("filter-has", std::make_unique<expression::Literal>(property), error);
    }
}

std::unique_ptr<expression::Expression> convertLegacyInFilter(const Convertible& values, Error& error) {
    optional<std::string> property = toString(arrayMember(values, 1));
    
    if (!property) {
        error = { "filter property must be a string" };
        return {};
    } else if (arrayLength(values) == 0) {
        return std::make_unique<expression::Literal>(false);
    } else if (*property == "$type") {
        return createExpression("filter-type-in", convertLiteralArray(values, 2), error);
    } else if (*property == "id") {
        return createExpression("filter-id-in", convertLiteralArray(values, 2), error);
    } else {
        return createExpression("filter-in", convertLiteralArray(values, 1), error);
    }
}

std::vector<std::unique_ptr<expression::Expression>> convertLegacyFilterArray(const Convertible &input, Error& error, std::size_t startIndex = 0) {
    std::vector<std::unique_ptr<expression::Expression>> output;
    for (std::size_t i = startIndex; i < arrayLength(input); i++) {
        output.push_back(convertLegacyFilter(arrayMember(input, i), error));
    }
    return output;
}
    
std::unique_ptr<expression::Expression> convertLegacyFilter(const Convertible& values, Error& error) {
    if (isUndefined(values)) {
        return std::make_unique<expression::Literal>(true);
    }
    
    optional<std::string> op = toString(arrayMember(values, 0));

    if (!op) {
        error = { "filter operator must be a string" };
        return {};
    } else if (arrayLength(values) <= 1) {
        return std::make_unique<expression::Literal>(*op != "any");
    } else {
        return (
            *op == "==" ||
            *op == "<" ||
            *op == ">" ||
            *op == "<=" ||
            *op == ">=" ? convertLegacyComparisonFilter(values, error) :
            *op == "!=" ? createExpression("!", convertLegacyComparisonFilter(values, error, {"=="}), error) :
            *op == "any" ? createExpression("any", convertLegacyFilterArray(values, error, 1), error) :
            *op == "all" ? createExpression("all", convertLegacyFilterArray(values, error, 1), error) :
            *op == "none" ? createExpression("!", createExpression("any", convertLegacyFilterArray(values, error, 1), error), error) :
            *op == "in" ? convertLegacyInFilter(values, error) :
            *op == "!in" ? createExpression("!", convertLegacyInFilter(values, error), error) :
            *op == "has" ? convertLegacyHasFilter(*toString(arrayMember(values, 1)), error) :
            *op == "!has" ? createExpression("!", convertLegacyHasFilter(*toString(arrayMember(values, 1)), error), error) :
           std::make_unique<expression::Literal>(true)
        );
    }
}

} // namespace conversion
} // namespace style
} // namespace mbgl
