
#include <mbgl/style/expression/parsing_context.hpp>
#include <mbgl/style/expression/at.hpp>
#include <mbgl/style/expression/array_assertion.hpp>
#include <mbgl/style/expression/assertion.hpp>
#include <mbgl/style/expression/boolean_operator.hpp>
#include <mbgl/style/expression/case.hpp>
#include <mbgl/style/expression/coalesce.hpp>
#include <mbgl/style/expression/coercion.hpp>
#include <mbgl/style/expression/compound_expression.hpp>
#include <mbgl/style/expression/curve.hpp>
#include <mbgl/style/expression/let.hpp>
#include <mbgl/style/expression/literal.hpp>
#include <mbgl/style/expression/match.hpp>
#include <mbgl/style/expression/type.hpp>
#include <mbgl/style/conversion/get_json_type.hpp>
#include <mbgl/style/expression/check_subtype.hpp>

namespace mbgl {
namespace style {
namespace expression {

ParseResult ParsingContext::parse(const mbgl::style::conversion::Convertible& value)
{
    using namespace mbgl::style::conversion;
    
    ParseResult parsed;
    
    if (isArray(value)) {
        const std::size_t length = arrayLength(value);
        if (length == 0) {
            error(R"(Expected an array with at least one element. If you wanted a literal array, use ["literal", []].)");
            return ParseResult();
        }
        
        const optional<std::string> op = toString(arrayMember(value, 0));
        if (!op) {
            error(
                "Expression name must be a string, but found " + getJSONType(arrayMember(value, 0)) +
                    R"( instead. If you wanted a literal array, use ["literal", [...]].)",
                0
            );
            return ParseResult();
        }
        
        if (*op == "literal") {
            if (length != 2) {
                error(
                    "'literal' expression requires exactly one argument, but found " + std::to_string(length - 1) + " instead."
                );
                return ParseResult();
            }
            
            parsed = Literal::parse(arrayMember(value, 1), *this);
        } else if (*op == "match") {
            parsed = parseMatch(value, *this);
        } else if (*op == "curve") {
            parsed = parseCurve(value, *this);
        } else if (*op == "coalesce") {
            parsed = Coalesce::parse(value, *this);
        } else if (*op == "case") {
            parsed = Case::parse(value, *this);
        } else if (*op == "array") {
            parsed = ArrayAssertion::parse(value, *this);
        } else if (*op == "let") {
            parsed = Let::parse(value, *this);
        } else if (*op == "var") {
            parsed = Var::parse(value, *this);
        } else if (*op == "at") {
            parsed = At::parse(value, *this);
        } else if (*op == "string" || *op == "number" || *op == "boolean" || *op == "object") {
            parsed = Assertion::parse(value, *this);
        } else if (*op == "to-color" || *op == "to-number") {
            parsed = Coercion::parse(value, *this);
        } else if (*op == "any") {
            parsed = Any::parse(value, *this);
        } else if (*op == "all") {
            parsed = All::parse(value, *this);
        } else {
            parsed = parseCompoundExpression(*op, value, *this);
        }
    } else {
        if (isObject(value)) {
            error(R"(Bare objects invalid. Use ["literal", {...}] instead.)");
            return ParseResult();
        }
        
        parsed = Literal::parse(value, *this);
    }
    
    if (!parsed) {
        assert(errors.size() > 0);
    } else if (expected) {
        auto wrapForType = [&](const type::Type& target, std::unique_ptr<Expression> expression) -> std::unique_ptr<Expression> {
            std::vector<std::unique_ptr<Expression>> args;
            args.push_back(std::move(expression));
            if (target == type::Color) {
                return std::make_unique<Coercion>(target, std::move(args));
            } else {
                return std::make_unique<Assertion>(target, std::move(args));
            }
        };
        
        const type::Type actual = (*parsed)->getType();
        if (*expected == type::Color && (actual == type::String || actual == type::Value)) {
            parsed = wrapForType(type::Color, std::move(*parsed));
        } else if ((*expected == type::String || *expected == type::Number || *expected == type::Boolean) && actual == type::Value) {
            parsed = wrapForType(*expected, std::move(*parsed));
        }
    
        checkType((*parsed)->getType());
        if (errors.size() > 0) {
            return ParseResult();
        }
    }
    
    return parsed;
}

optional<std::string> ParsingContext::checkType(const type::Type& t) {
    assert(expected);
    optional<std::string> err = type::checkSubtype(*expected, t);
    if (err) {
        error(*err);
    }
    return err;
}

} // namespace expression
} // namespace style
} // namespace mbgl
