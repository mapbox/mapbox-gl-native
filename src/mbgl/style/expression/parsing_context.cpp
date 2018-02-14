
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
#include <mbgl/style/expression/equals.hpp>
#include <mbgl/style/expression/interpolate.hpp>
#include <mbgl/style/expression/let.hpp>
#include <mbgl/style/expression/literal.hpp>
#include <mbgl/style/expression/match.hpp>
#include <mbgl/style/expression/step.hpp>

#include <mbgl/style/expression/find_zoom_curve.hpp>

#include <mbgl/style/conversion/get_json_type.hpp>

#include <mbgl/util/string.hpp>

namespace mbgl {
namespace style {
namespace expression {

bool isConstant(const Expression& expression) {
    if (dynamic_cast<const Var*>(&expression)) {
        return false;
    }
    
    if (auto compound = dynamic_cast<const CompoundExpressionBase*>(&expression)) {
        if (compound->getName() == "error") {
            return false;
        }
    }
    
    bool literalArgs = true;
    expression.eachChild([&](const Expression& child) {
        if (!dynamic_cast<const Literal*>(&child)) {
            literalArgs = false;
        }
    });
    if (!literalArgs) {
        return false;
    }
    
    return isFeatureConstant(expression) &&
        isGlobalPropertyConstant(expression, std::array<std::string, 2>{{"zoom", "heatmap-density"}}) &&
        isGlobalPropertyConstant(expression, std::array<std::string, 2>{{"zoom", "line-progress"}});
}

using namespace mbgl::style::conversion;

ParseResult ParsingContext::parse(const Convertible& value,
                                  std::size_t index_,
                                  optional<type::Type> expected_,
                                  TypeAnnotationOption typeAnnotationOption) {
    ParsingContext child(key + "[" + util::toString(index_) + "]",
                         errors,
                         std::move(expected_),
                         scope);
    return child.parse(value, typeAnnotationOption);
}

ParseResult ParsingContext::parse(const Convertible& value, std::size_t index_, optional<type::Type> expected_,
                                  const std::map<std::string, std::shared_ptr<Expression>>& bindings) {
    ParsingContext child(key + "[" + util::toString(index_) + "]",
                         errors,
                         std::move(expected_),
                         std::make_shared<detail::Scope>(bindings, scope));
    return child.parse(value);
}

const ExpressionRegistry& getExpressionRegistry() {
    static ExpressionRegistry registry {{
        {"==", Equals::parse},
        {"!=", Equals::parse},
        {"all", All::parse},
        {"any", Any::parse},
        {"array", ArrayAssertion::parse},
        {"at", At::parse},
        {"boolean", Assertion::parse},
        {"case", Case::parse},
        {"coalesce", Coalesce::parse},
        {"interpolate", parseInterpolate},
        {"let", Let::parse},
        {"literal", Literal::parse},
        {"match", parseMatch},
        {"number", Assertion::parse},
        {"object", Assertion::parse},
        {"step", Step::parse},
        {"string", Assertion::parse},
        {"to-color", Coercion::parse},
        {"to-number", Coercion::parse},
        {"var", Var::parse}
    }};
    return registry;
}

ParseResult ParsingContext::parse(const Convertible& value, TypeAnnotationOption typeAnnotationOption) {
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
        
        const ExpressionRegistry& registry = getExpressionRegistry();
        auto parseFunction = registry.find(*op);
        if (parseFunction != registry.end()) {
            parsed = parseFunction->second(value, *this);
        } else {
            parsed = parseCompoundExpression(*op, value, *this);
        }
    } else {
        parsed = Literal::parse(value, *this);
    }

    if (!parsed) {
        assert(errors->size() > 0);
        return parsed;
    }

    if (expected) {
        auto array = [&](std::unique_ptr<Expression> expression) {
            std::vector<std::unique_ptr<Expression>> args;
            args.push_back(std::move(expression));
            return args;
        };

        const type::Type actual = (*parsed)->getType();
        if ((*expected == type::String || *expected == type::Number || *expected == type::Boolean) && actual == type::Value) {
            if (typeAnnotationOption == includeTypeAnnotations) {
                parsed = { std::make_unique<Assertion>(*expected, array(std::move(*parsed))) };
            }
        } else if (expected->is<type::Array>() && actual == type::Value) {
            if (typeAnnotationOption == includeTypeAnnotations) {
                parsed = { std::make_unique<ArrayAssertion>(expected->get<type::Array>(), std::move(*parsed)) };
            }
        } else if (*expected == type::Color && (actual == type::Value || actual == type::String)) {
            if (typeAnnotationOption == includeTypeAnnotations) {
                parsed = { std::make_unique<Coercion>(*expected, array(std::move(*parsed))) };
            }
        } else {
            checkType((*parsed)->getType());
            if (errors->size() > 0) {
                return ParseResult();
            }
        }
    }

    // If an expression's arguments are all literals, we can evaluate
    // it immediately and replace it with a literal value in the
    // parsed result.
    if (!dynamic_cast<Literal *>(parsed->get()) && isConstant(**parsed)) {
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

    // if this is the root expression, enforce constraints on the use ["zoom"].
    if (key.size() == 0 && !isZoomConstant(**parsed)) {
        optional<variant<const InterpolateBase*, const Step*, ParsingError>> zoomCurve = findZoomCurve(parsed->get());
        if (!zoomCurve) {
            error(R"("zoom" expression may only be used as input to a top-level "step" or "interpolate" expression.)");
            return ParseResult();
        } else if (zoomCurve->is<ParsingError>()) {
            error(zoomCurve->get<ParsingError>().message);
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
