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
    
using namespace mbgl::style::expression;
    
static bool isExpression(const Convertible& filter);
ParseResult convertLegacyFilter(const Convertible& values, Error& error);
optional<mbgl::Value> serializeLegacyFilter(const Convertible& values);

optional<Filter> Converter<Filter>::operator()(const Convertible& value, Error& error) const {
    if (isExpression(value)) {
        ParsingContext parsingContext(type::Boolean);
        ParseResult parseResult = parsingContext.parseExpression(value);
        if (!parseResult) {
            error = { parsingContext.getCombinedErrors() };
            return {};
        } else {
            return { Filter(std::move(parseResult)) };
        }
    } else {
        ParseResult expression = convertLegacyFilter(value, error);
        if (!expression) {
            assert(error.message.size() > 0);
            return {};
        }
        return Filter(optional<std::unique_ptr<Expression>>(std::move(*expression)), serializeLegacyFilter(value));
    }
}

// This is a port from https://github.com/mapbox/mapbox-gl-js/blob/master/src/style-spec/feature_filter/index.js
bool isExpression(const Convertible& filter) {
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
        return arrayLength(filter) != 3 || isArray(arrayMember(filter, 1)) || isArray(arrayMember(filter, 2));
        
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
    
ParseResult createExpression(std::string op, optional<std::vector<std::unique_ptr<Expression>>> args, Error& error) {
    if (!args) return {};
    assert(std::all_of(args->begin(), args->end(), [](const std::unique_ptr<Expression> &e) {
        return bool(e.get());
    }));
    
    if (op == "any") {
        return {std::make_unique<Any>(std::move(*args))};
    } else if (op == "all") {
        return {std::make_unique<All>(std::move(*args))};
    } else {
        ParsingContext parsingContext(type::Boolean);
        ParseResult parseResult = createCompoundExpression(op, std::move(*args), parsingContext);
        if (!parseResult) {
            error = { parsingContext.getCombinedErrors() };
            return {};
        } else {
            return parseResult;
        }
    }
}

ParseResult createExpression(std::string op, ParseResult arg, Error& error) {
    if (!arg) {
        return {};
    }
    
    std::vector<std::unique_ptr<Expression>> args;
    args.push_back(std::move(*arg));
    return createExpression(op, std::move(args), error);
}

ParseResult convertLiteral(const Convertible& convertible, Error& error) {
    ParsingContext parsingContext;
    ParseResult parseResult = Literal::parse(convertible, parsingContext);
    if (parseResult) {
        return parseResult;
    } else {
        error = { parsingContext.getCombinedErrors() };
        return {};
    }
}

optional<std::vector<std::unique_ptr<Expression>>> convertLiteralArray(const Convertible &input, Error& error, std::size_t startIndex = 0) {
    std::vector<std::unique_ptr<Expression>> output;
    for (std::size_t i = startIndex; i < arrayLength(input); i++) {
        ParseResult literal = convertLiteral(arrayMember(input, i), error);
        if (!literal) {
            return {};
        }
        output.push_back(std::move(*literal));
    }
    return {std::move(output)};
}

ParseResult convertLegacyComparisonFilter(const Convertible& values, Error& error, optional<std::string> opOverride = {}) {
    optional<std::string> op = opOverride ? opOverride : toString(arrayMember(values, 0));
    optional<std::string> property = toString(arrayMember(values, 1));
    
    if (!property) {
        error = { "filter property must be a string" };
        return {};
    } else if (*property == "$type") {
        return createExpression("filter-type-" + *op, convertLiteralArray(values, error, 2), error);
    } else if (*property == "$id") {
        return createExpression("filter-id-" + *op, convertLiteralArray(values, error, 2), error);
    } else {
        return createExpression("filter-" + *op, convertLiteralArray(values, error, 1), error);
    }
}
    
ParseResult convertLegacyHasFilter(const Convertible& values, Error& error) {
    optional<std::string> property = toString(arrayMember(values, 1));
    
    if (!property) {
        error = { "filter property must be a string" };
        return {};
    } else if (*property == "$type") {
        return {std::make_unique<Literal>(true)};
    } else if (*property == "$id") {
        return createExpression("filter-has-id", std::vector<std::unique_ptr<Expression>>(), error);
    } else {
        return createExpression("filter-has", {std::make_unique<Literal>(*property)}, error);
    }
}

ParseResult convertLegacyInFilter(const Convertible& values, Error& error) {
    optional<std::string> property = toString(arrayMember(values, 1));
    
    if (!property) {
        error = { "filter property must be a string" };
        return {};
    } else if (arrayLength(values) == 0) {
        return {std::make_unique<Literal>(false)};
    } else if (*property == "$type") {
        return createExpression("filter-type-in", convertLiteralArray(values, error, 2), error);
    } else if (*property == "$id") {
        return createExpression("filter-id-in", convertLiteralArray(values, error, 2), error);
    } else {
        return createExpression("filter-in", convertLiteralArray(values, error, 1), error);
    }
}

optional<std::vector<std::unique_ptr<Expression>>> convertLegacyFilterArray(const Convertible &input, Error& error, std::size_t startIndex = 0) {
    std::vector<std::unique_ptr<Expression>> output;
    for (std::size_t i = startIndex; i < arrayLength(input); i++) {
        optional<std::unique_ptr<Expression>> child = convertLegacyFilter(arrayMember(input, i), error);
        if (!child) {
            return {};
        }
        output.push_back(std::move(*child));
    }
    return {std::move(output)};
}
    
ParseResult convertLegacyFilter(const Convertible& values, Error& error) {
    if (isUndefined(values)) {
        return {std::make_unique<Literal>(true)};
    }
    
    optional<std::string> op = toString(arrayMember(values, 0));

    if (!op) {
        error = { "filter operator must be a string" };
        return {};
    } else if (arrayLength(values) <= 1) {
        return {std::make_unique<Literal>(*op != "any")};
    } else {
        return {
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
            *op == "has" ? convertLegacyHasFilter(values, error) :
            *op == "!has" ? createExpression("!", convertLegacyHasFilter(values, error), error) :
            ParseResult(std::make_unique<Literal>(true))
        };
    }
}

optional<mbgl::Value> serializeLegacyFilter(const Convertible& values) {
    if (isUndefined(values)) {
        return {};
    } else if (isArray(values)) {
        std::vector<mbgl::Value> result;
        for (std::size_t i = 0; i < arrayLength(values); i++) {
            auto arrayValue = serializeLegacyFilter(arrayMember(values, i));
            if (arrayValue) {
                result.push_back(*arrayValue);
            } else {
                result.push_back(NullValue());
            }
        }
        return (mbgl::Value)result;
    }
    return toValue(values);
}

} // namespace conversion
} // namespace style
} // namespace mbgl
