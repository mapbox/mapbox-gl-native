
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
#include <mbgl/style/expression/length.hpp>
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
    if (expression.getKind() == Kind::Var) {
        auto varExpression = static_cast<const Var*>(&expression);
        return isConstant(*varExpression->getBoundExpression());
    }

    if (expression.getKind() == Kind::CompoundExpression) {
        auto compound = static_cast<const CompoundExpression*>(&expression);
        if (compound->getOperator() == "error") {
            return false;
        }
    }

    bool isTypeAnnotation = expression.getKind() == Kind::Coercion ||
        expression.getKind() == Kind::Assertion ||
        expression.getKind() == Kind::ArrayAssertion;
    
    bool childrenConstant = true;
    expression.eachChild([&](const Expression& child) {
        // We can _almost_ assume that if `expressions` children are constant,
        // they would already have been evaluated to Literal values when they
        // were parsed.  Type annotations are the exception, because they might
        // have been inferred and added after a child was parsed.

        // So we recurse into isConstant() for the children of type annotations,
        // but otherwise simply check whether they are Literals.
        if (isTypeAnnotation) {
            childrenConstant = childrenConstant && isConstant(child);
        } else {
            childrenConstant = childrenConstant && child.getKind() == Kind::Literal;
        }
    });
    if (!childrenConstant) {
        return false;
    }
    
    return isFeatureConstant(expression) &&
        isGlobalPropertyConstant(expression, std::array<std::string, 2>{{"zoom", "heatmap-density"}});
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
        {"collator", CollatorExpression::parse},
        {"interpolate", parseInterpolate},
        {"length", Length::parse},
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

    auto array = [&](std::unique_ptr<Expression> expression) {
        std::vector<std::unique_ptr<Expression>> args;
        args.push_back(std::move(expression));
        return args;
    };

    if (expected) {
        const type::Type actual = (*parsed)->getType();
        if ((*expected == type::String || *expected == type::Number || *expected == type::Boolean || *expected == type::Object) && actual == type::Value) {
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

    // If an expression's arguments are all constant, we can evaluate
    // it immediately and replace it with a literal value in the
    // parsed result.
    if ((*parsed)->getKind() != Kind::Literal && isConstant(**parsed)) {
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

ParseResult ParsingContext::parseExpression(const Convertible& value, TypeAnnotationOption typeAnnotationOption) {
    return parse(value, typeAnnotationOption);
}

ParseResult ParsingContext::parseLayerPropertyExpression(const Convertible& value, TypeAnnotationOption typeAnnotationOption) {
    ParseResult parsed = parse(value, typeAnnotationOption);
    if (parsed && !isZoomConstant(**parsed)) {
        optional<variant<const Interpolate*, const Step*, ParsingError>> zoomCurve = findZoomCurve(parsed->get());
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

const std::string ParsingContext::getCombinedErrors() const {
    std::string combinedError;
    for (const ParsingError& parsingError : *errors) {
        if (combinedError.size() > 0) {
            combinedError += "\n";
        }
        if (parsingError.key.size() > 0) {
            combinedError += parsingError.key + ": ";
        }
        combinedError += parsingError.message;
    }
    return combinedError;
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
