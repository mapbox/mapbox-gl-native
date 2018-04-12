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
    
// TODO factor this out
using namespace mbgl::style::expression;

using GeometryValue = mapbox::geometry::value;

// This is a port from https://github.com/mapbox/mapbox-gl-js/blob/master/src/style-spec/feature_filter/index.js
static bool isExpressionFilter(const Convertible& filter) {
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
            if (!isExpressionFilter(f) && !toBool(f)) {
                return false;
            }
        }
        return true;
        
    } else {
        return true;
    }
}

static bool isLegacyFilter(const Convertible& filter) {
    return !isExpressionFilter(filter);
}

std::unique_ptr<Expression> createLiteralExpression(const Convertible& value) {
    // TODO handle null case
    return std::make_unique<Literal>(toExpressionValue(*toValue(value)));
}
    
std::vector<std::unique_ptr<Expression>> createLiteralExpressionArray(const Convertible &input, std::size_t startIndex = 0) {
    std::vector<std::unique_ptr<Expression>> output;
    for (std::size_t i = startIndex; i < arrayLength(input); i++) {
        output.push_back(createLiteralExpression(arrayMember(input, i)));
    }
    return output;
}
    
std::unique_ptr<Expression> createExpression(std::string op, std::vector<std::unique_ptr<Expression>> args, Error& error) {
    
    if (op == "any") {
        return std::make_unique<Any>(std::move(args));
        
    } else if (op == "all") {
        return std::make_unique<All>(std::move(args));

    } else {
        ParsingContext context;
        ParseResult parseResult = createCompoundExpression(op, std::move(args), context);
        
        if (!parseResult || context.getErrors().size() > 0) {
            error = { context.getErrors()[0].message };
            return {};
        } else {
            return std::move(*parseResult);
        }
    }
}

std::unique_ptr<Expression> convertLegacyFilter(const Convertible& values, Error& error);

std::vector<std::unique_ptr<Expression>> createLegacyFilter2Array(const Convertible &input, Error& error, std::size_t startIndex = 0) {
    std::vector<std::unique_ptr<Expression>> output;
    for (std::size_t i = startIndex; i < arrayLength(input); i++) {
        output.push_back(convertLegacyFilter(arrayMember(input, i), error));
    }
    return output;
}

optional<Filter> Converter<Filter>::operator()(const Convertible& value, Error& error) const {
    
    optional<std::unique_ptr<Expression>> expression;
    
    if (isLegacyFilter(value)) {
        expression = convertLegacyFilter(value, error);
    } else {
        
        expression::ParsingContext ctx(expression::type::Boolean);
        expression::ParseResult parseResult = ctx.parseExpression(value);
        if (!parseResult) {
            error = { ctx.getCombinedErrors() };
            return {};
        }
        
        expression = std::move(*parseResult);
    }
    
    if (!expression) return {};
    
    return { Filter { std::move(*expression) } };
}

    std::unique_ptr<Expression> convertComparisonOp(const Convertible& values, Error& error, optional<std::string> opOverride = {}) {
    optional<std::string> op = opOverride ? opOverride : toString(arrayMember(values, 0));
    optional<std::string> property = toString(arrayMember(values, 1));
    
    if (!property) {
        error = { "filter property must be a string" };
        return {};
    } else if (*property == "$type") {
        return createExpression("filter-type-" + *op, createLiteralExpressionArray(values, 2), error);
    } else if (*property == "$id") {
        return createExpression("filter-id-" + *op, createLiteralExpressionArray(values, 2), error);
    } else {
        return createExpression("filter-" + *op, createLiteralExpressionArray(values, 1), error);
    }
}
    
std::unique_ptr<Expression> convertHasOp(std::string property, Error& error) {
    if (property == "$type") {
        return std::make_unique<Literal>(true);
    } else if (property == "$id") {
        return createExpression("filter-has-id", {}, error);
    } else {
        std::vector<std::unique_ptr<Expression>> args;
        args.push_back(std::make_unique<Literal>(property));
        return createExpression("filter-has", std::move(args), error);
    }
}
    
std::unique_ptr<Expression> convertNegation(std::unique_ptr<Expression> expression, Error& error) {
    std::vector<std::unique_ptr<Expression>> args;
    args.push_back(std::move(expression));
    return createExpression("!", std::move(args), error);
}

std::unique_ptr<Expression> convertInOp(const Convertible& values, Error& error) {
    optional<std::string> property = toString(arrayMember(values, 1));
    
    if (!property) {
        error = { "filter property must be a string" };
        return {};
    } else if (arrayLength(values) == 0) {
        return std::make_unique<Literal>(false);
    } else if (*property == "$type") {
        return createExpression("filter-type-in", createLiteralExpressionArray(values, 2), error);
    } else if (*property == "id") {
        return createExpression("filter-id-in", createLiteralExpressionArray(values, 2), error);
    } else {
        return createExpression("filter-in", createLiteralExpressionArray(values, 1), error);
    }
}
    
std::unique_ptr<Expression> convertLegacyFilter(const Convertible& values, Error& error) {
    
    if (isUndefined(values)) {
        return std::make_unique<Literal>(true);
    }
    
    optional<std::string> op = toString(arrayMember(values, 0));
    
    if (!op) {
        error = { "filter operator must be a string" };
        return {};
    } else if (arrayLength(values) <= 1) {
        return std::make_unique<Literal>(*op != "any");
    } else {
        return (
            *op == "==" ||
            *op == "<" ||
            *op == ">" ||
            *op == "<=" ||
            *op == ">=" ? convertComparisonOp(values, error) :
            *op == "!=" ? convertNegation(convertComparisonOp(values, error, {"=="}), error) :
            *op == "any" ? createExpression("any", createLegacyFilter2Array(values, error, 1), error) :
            *op == "all" ? createExpression("all", createLegacyFilter2Array(values, error, 1), error) :
            *op == "none" ? convertNegation(createExpression("any", createLegacyFilter2Array(values, error, 1), error), error) :
            *op == "in" ? convertInOp(values, error) :
            *op == "!in" ? convertNegation(convertInOp(values, error), error) :
            *op == "has" ? convertHasOp(*toString(arrayMember(values, 1)), error) :
            *op == "!has" ? convertNegation(convertHasOp(*toString(arrayMember(values, 1)), error), error) :
           std::make_unique<Literal>(true)
        );
    }
}
    
} // namespace conversion
} // namespace style
} // namespace mbgl

