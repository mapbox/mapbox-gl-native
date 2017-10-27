
#include <mbgl/style/expression/parsing_context.hpp>
#include <mbgl/style/expression/check_subtype.hpp>
#include <mbgl/style/expression/is_constant.hpp>
#include <mbgl/style/expression/type.hpp>

#include <mbgl/style/expression/expression.hpp>
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

#include <mbgl/style/conversion/get_json_type.hpp>

namespace mbgl {
namespace style {
namespace expression {

bool isConstant(const Expression* expression) {
    if (dynamic_cast<const Var*>(expression)) {
        return false;
    }
    
    if (auto compound = dynamic_cast<const CompoundExpressionBase*>(expression)) {
        if (compound->getName() == "error") {
            return false;
        }
    }
    
    bool literalArgs = true;
    expression->eachChild([&](const Expression* child) {
        if (!dynamic_cast<const Literal*>(child)) {
            literalArgs = false;
        }
    });
    if (!literalArgs) {
        return false;
    }
    
    return isFeatureConstant(expression) &&
        isGlobalPropertyConstant(expression, std::array<std::string, 2>{{"zoom", "heatmap-density"}});
}

using namespace mbgl::style::conversion;

ParseResult ParsingContext::parse(const Convertible& value, std::size_t index_, optional<type::Type> expected_) {
    ParsingContext child(key + "[" + std::to_string(index_) + "]",
                         errors,
                         std::move(expected_),
                         scope);
    return child.parse(value);
}

ParseResult ParsingContext::parse(const Convertible& value, std::size_t index_, optional<type::Type> expected_,
                                  const std::map<std::string, std::shared_ptr<Expression>>& bindings) {
    ParsingContext child(key + "[" + std::to_string(index_) + "]",
                         errors,
                         std::move(expected_),
                         std::make_shared<detail::Scope>(bindings, scope));
    return child.parse(value);
}

using namespace mbgl::style::conversion;
ParseResult ParsingContext::parse(const Convertible& value)
{
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
        assert(errors->size() > 0);
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
        if (errors->size() > 0) {
            return ParseResult();
        }
    }
    
    // If an expression's arguments are all literals, we can evaluate
    // it immediately and replace it with a literal value in the
    // parsed result.
    if (parsed && !dynamic_cast<Literal *>(parsed->get()) && isConstant(parsed->get())) {
        EvaluationContext params(nullptr);
        EvaluationResult evaluated((*parsed)->evaluate(params));
        if (!evaluated) {
            error(evaluated.error().message);
            return ParseResult();
        }
        
        const type::Type type = (*parsed)->getType();
        if (type.is<type::Array>()) {
            // keep the original expression's array type, even if the evaluated
            // type is more specific.
            return ParseResult(std::make_unique<Literal>(
                  type.get<type::Array>(),
                  evaluated->get<std::vector<Value>>())
            );
        } else {
            return ParseResult(std::make_unique<Literal>(*evaluated));
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
