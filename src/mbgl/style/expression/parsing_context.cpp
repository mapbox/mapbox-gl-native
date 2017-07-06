
#include <mbgl/style/expression/parsing_context.hpp>
#include <mbgl/style/expression/at.hpp>
#include <mbgl/style/expression/array_assertion.hpp>
#include <mbgl/style/expression/case.hpp>
#include <mbgl/style/expression/coalesce.hpp>
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
        auto wrapForType = [&](const std::string& wrapper, std::unique_ptr<Expression> expression) {
            std::vector<std::unique_ptr<Expression>> args;
            args.push_back(std::move(expression));
            return createCompoundExpression(wrapper, std::move(args), *this);
        };
        
        const type::Type actual = (*parsed)->getType();
        if (*expected == type::Color && (actual == type::String || actual == type::Value)) {
            parsed = wrapForType("to-color", std::move(*parsed));
        } else if (*expected != type::Value && actual == type::Value) {
            if (*expected == type::String) {
                parsed = wrapForType("string", std::move(*parsed));
            } else if (*expected == type::Number) {
                parsed = wrapForType("number", std::move(*parsed));
            } else if (*expected == type::Boolean) {
                parsed = wrapForType("boolean", std::move(*parsed));
            }
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
