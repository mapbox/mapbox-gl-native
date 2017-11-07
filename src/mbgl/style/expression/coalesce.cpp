#include <mbgl/style/expression/coalesce.hpp>

namespace mbgl {
namespace style {
namespace expression {

EvaluationResult Coalesce::evaluate(const EvaluationContext& params) const {
    EvaluationResult result = Null;
    for (const auto& arg : args) {
        result = arg->evaluate(params);
        if (!result || *result != Null) break;
    }
    return result;
}

void Coalesce::eachChild(const std::function<void(const Expression&)>& visit) const {
    for (const std::unique_ptr<Expression>& arg : args) {
        visit(*arg);
    }
}

bool Coalesce::operator==(const Expression& e) const {
    if (auto rhs = dynamic_cast<const Coalesce*>(&e)) {
        return Expression::childrenEqual(args, rhs->args);
    }
    return false;
}

using namespace mbgl::style::conversion;
ParseResult Coalesce::parse(const Convertible& value, ParsingContext& ctx) {
    assert(isArray(value));
    auto length = arrayLength(value);
    if (length < 2) {
        ctx.error("Expected at least one argument.");
        return ParseResult();
    }
 
    optional<type::Type> outputType;
    if (ctx.getExpected() && *ctx.getExpected() != type::Value) {
        outputType = ctx.getExpected();
    }

    Coalesce::Args args;
    args.reserve(length - 1);
    for (std::size_t i = 1; i < length; i++) {
        auto parsed = ctx.parse(arrayMember(value, i), i, outputType);
        if (!parsed) {
            return parsed;
        }
        if (!outputType) {
            outputType = (*parsed)->getType();
        }
        args.push_back(std::move(*parsed));
    }
 
    assert(outputType);
    return ParseResult(std::make_unique<Coalesce>(*outputType, std::move(args)));
}

} // namespace expression
} // namespace style
} // namespace mbgl
