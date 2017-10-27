#include <mbgl/style/expression/at.hpp>


namespace mbgl {
namespace style {
namespace expression {

EvaluationResult At::evaluate(const EvaluationContext& params) const {
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

void At::eachChild(const std::function<void(const Expression*)>& visit) const {
    visit(index.get());
    visit(input.get());
}

using namespace mbgl::style::conversion;
ParseResult At::parse(const Convertible& value, ParsingContext& ctx) {
    assert(isArray(value));

    std::size_t length = arrayLength(value);
    if (length != 3) {
        ctx.error("Expected 2 arguments, but found " + std::to_string(length - 1) + " instead.");
        return ParseResult();
    }

    ParseResult index = ctx.parse(arrayMember(value, 1), 1, {type::Number});
    
    type::Type inputType = type::Array(ctx.getExpected() ? *ctx.getExpected() : type::Value);
    ParseResult input = ctx.parse(arrayMember(value, 2), 2, {inputType});

    if (!index || !input) return ParseResult();

    return ParseResult(std::make_unique<At>(std::move(*index), std::move(*input)));

}

} // namespace expression
} // namespace style
} // namespace mbgl
