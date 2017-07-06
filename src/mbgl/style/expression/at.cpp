#include <mbgl/style/expression/at.hpp>


namespace mbgl {
namespace style {
namespace expression {

EvaluationResult At::evaluate(const EvaluationParameters& params) const {
    const EvaluationResult evaluatedIndex = index->evaluate(params);
    const EvaluationResult evaluatedInput = input->evaluate(params);
    if (!evaluatedIndex) {
        return evaluatedIndex.error();
    }
    if (!evaluatedInput) {
        return evaluatedInput.error();
    }
    
    const auto i = *fromExpressionValue<double>(*evaluatedIndex);
    const auto inputArray = *fromExpressionValue<std::vector<Value>>(*evaluatedInput);
    
    if (i < 0 || i >= inputArray.size()) {
        return EvaluationError {
            "Array index out of bounds: " + stringify(i) +
            " > " + std::to_string(inputArray.size()) + "."
        };
    }
    if (i != std::floor(i)) {
        return EvaluationError {
            "Array index must be an integer, but found " + stringify(i) + " instead."
        };
    }
    return inputArray[static_cast<std::size_t>(i)];
}

void At::accept(std::function<void(const Expression*)> visit) const {
    visit(this);
    index->accept(visit);
    input->accept(visit);
}

ParseResult At::parse(const mbgl::style::conversion::Convertible& value, ParsingContext ctx) {
    using namespace mbgl::style::conversion;
    assert(isArray(value));

    std::size_t length = arrayLength(value);
    if (length != 3) {
        ctx.error("Expected 2 arguments, but found " + std::to_string(length - 1) + " instead.");
        return ParseResult();
    }

    ParseResult index = ctx.concat(1, {type::Number}).parse(arrayMember(value, 1));
    ParseResult input = ctx.concat(2, {type::Array(ctx.expected ? *ctx.expected : type::Value)}).parse(arrayMember(value, 2));

    if (!index || !input) return ParseResult();

    return ParseResult(std::make_unique<At>(std::move(*index), std::move(*input)));

}

} // namespace expression
} // namespace style
} // namespace mbgl
