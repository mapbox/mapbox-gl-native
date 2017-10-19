#include <mbgl/style/expression/boolean_operator.hpp>

namespace mbgl {
namespace style {
namespace expression {

EvaluationResult Any::evaluate(const EvaluationParameters& params) const {
    for (auto it = inputs.begin(); it != inputs.end(); it++) {
        const EvaluationResult result = (*it)->evaluate(params);
        if (!result) return result;
        if (result->get<bool>()) return EvaluationResult(true);
    }
    return EvaluationResult(false);
}

void Any::accept(std::function<void(const Expression*)> visit) const {
    visit(this);
    for (const std::unique_ptr<Expression>& input : inputs) {
        input->accept(visit);
    }
}

EvaluationResult All::evaluate(const EvaluationParameters& params) const {
    for (auto it = inputs.begin(); it != inputs.end(); it++) {
        const EvaluationResult result = (*it)->evaluate(params);
        if (!result) return result;
        if (!result->get<bool>()) return EvaluationResult(false);
    }
    return EvaluationResult(true);
}

void All::accept(std::function<void(const Expression*)> visit) const {
    visit(this);
    for (const std::unique_ptr<Expression>& input : inputs) {
        input->accept(visit);
    }
}

template <class T>
ParseResult parseBooleanOp(const mbgl::style::conversion::Convertible& value, ParsingContext ctx) {
    using namespace mbgl::style::conversion;
    assert(isArray(value));
    auto length = arrayLength(value);
 
    std::vector<std::unique_ptr<Expression>> parsedInputs;

    for (std::size_t i = 1; i < length; i++) {
        auto parsed = ctx.concat(i, {type::Boolean}).parse(arrayMember(value, i));
        if (!parsed) {
            return parsed;
        }
        
        parsedInputs.push_back(std::move(*parsed));
    }
 
    return ParseResult(std::make_unique<T>(std::move(parsedInputs)));
}

ParseResult Any::parse(const mbgl::style::conversion::Convertible& value, ParsingContext ctx) {
    return parseBooleanOp<Any>(value, ctx);
}

ParseResult All::parse(const mbgl::style::conversion::Convertible& value, ParsingContext ctx) {
    return parseBooleanOp<All>(value, ctx);
}


} // namespace expression
} // namespace style
} // namespace mbgl

