#include <mbgl/style/expression/boolean_operator.hpp>

namespace mbgl {
namespace style {
namespace expression {

EvaluationResult Any::evaluate(const EvaluationContext& params) const {
    for (auto it = inputs.begin(); it != inputs.end(); it++) {
        const EvaluationResult result = (*it)->evaluate(params);
        if (!result) return result;
        if (result->get<bool>()) return EvaluationResult(true);
    }
    return EvaluationResult(false);
}

void Any::eachChild(const std::function<void(const Expression&)>& visit) const {
    for (const std::unique_ptr<Expression>& input : inputs) {
        visit(*input);
    }
}

bool Any::operator==(const Expression& e) const {
    if (auto rhs = dynamic_cast<const Any*>(&e)) {
        return Expression::childrenEqual(inputs, rhs->inputs);
    }
    return false;
}

std::vector<optional<Value>> Any::possibleOutputs() const {
    return {{ true }, { false }};
}


EvaluationResult All::evaluate(const EvaluationContext& params) const {
    for (auto it = inputs.begin(); it != inputs.end(); it++) {
        const EvaluationResult result = (*it)->evaluate(params);
        if (!result) return result;
        if (!result->get<bool>()) return EvaluationResult(false);
    }
    return EvaluationResult(true);
}

void All::eachChild(const std::function<void(const Expression&)>& visit) const {
    for (const std::unique_ptr<Expression>& input : inputs) {
        visit(*input);
    }
}

bool All::operator==(const Expression& e) const {
    if (auto rhs = dynamic_cast<const All*>(&e)) {
        return Expression::childrenEqual(inputs, rhs->inputs);
    }
    return false;
}

std::vector<optional<Value>> All::possibleOutputs() const {
    return {{ true }, { false }};
}

using namespace mbgl::style::conversion;

template <class T>
ParseResult parseBooleanOp(const Convertible& value, ParsingContext& ctx) {
    
    assert(isArray(value));
    auto length = arrayLength(value);
 
    std::vector<std::unique_ptr<Expression>> parsedInputs;
    
    parsedInputs.reserve(length - 1);
    for (std::size_t i = 1; i < length; i++) {
        auto parsed = ctx.parse(arrayMember(value, i), i, {type::Boolean});
        if (!parsed) {
            return parsed;
        }
        
        parsedInputs.push_back(std::move(*parsed));
    }
 
    return ParseResult(std::make_unique<T>(std::move(parsedInputs)));
}

ParseResult Any::parse(const Convertible& value, ParsingContext& ctx) {
    return parseBooleanOp<Any>(value, ctx);
}

ParseResult All::parse(const Convertible& value, ParsingContext& ctx) {
    return parseBooleanOp<All>(value, ctx);
}


} // namespace expression
} // namespace style
} // namespace mbgl

