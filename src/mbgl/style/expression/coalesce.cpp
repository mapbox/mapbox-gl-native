#include <mbgl/style/expression/coalesce.hpp>

namespace mbgl {
namespace style {
namespace expression {

EvaluationResult Coalesce::evaluate(const EvaluationParameters& params) const {
    for (auto it = args.begin(); it != args.end(); it++) {
        const EvaluationResult result = (*it)->evaluate(params);
        if (!result || *result != Null || std::next(it) == args.end()) return result;
    }
    
    return Null;
}

void Coalesce::eachChild(std::function<void(const Expression*)> visit) const {
    for (const std::unique_ptr<Expression>& arg : args) {
        visit(arg.get());
    }
}

ParseResult Coalesce::parse(const mbgl::style::conversion::Convertible& value, ParsingContext ctx) {
    using namespace mbgl::style::conversion;
    assert(isArray(value));
    auto length = arrayLength(value);
    if (length < 2) {
        ctx.error("Expected at least one argument.");
        return ParseResult();
    }
 
    Coalesce::Args args;
    optional<type::Type> outputType;
    if (ctx.expected && *ctx.expected != type::Value) {
        outputType = ctx.expected;
    }

    for (std::size_t i = 1; i < length; i++) {
        auto parsed = ctx.concat(i, outputType).parse(arrayMember(value, i));
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
