#include <mbgl/style/expression/coalesce.hpp>
#include <mbgl/style/expression/check_subtype.hpp>

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

std::vector<optional<Value>> Coalesce::possibleOutputs() const {
    std::vector<optional<Value>> result;
    for (const auto& arg : args) {
        for (auto& output : arg->possibleOutputs()) {
            result.push_back(std::move(output));
        }
    }
    return result;
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
    optional<type::Type> expectedType = ctx.getExpected();
    if (expectedType && *expectedType != type::Value) {
        outputType = expectedType;
    }

    Coalesce::Args args;
    args.reserve(length - 1);
    for (std::size_t i = 1; i < length; i++) {
        auto parsed = ctx.parse(arrayMember(value, i), i, outputType, ParsingContext::omitTypeAnnotations);
        if (!parsed) {
            return parsed;
        }
        if (!outputType) {
            outputType = (*parsed)->getType();
        }
        args.push_back(std::move(*parsed));
    }
    assert(outputType);

    // Above, we parse arguments without inferred type annotation so that
    // they don't produce a runtime error for `null` input, which would
    // preempt the desired null-coalescing behavior.
    // Thus, if any of our arguments would have needed an annotation, we
    // need to wrap the enclosing coalesce expression with it instead.
    bool needsAnnotation = expectedType &&
        std::any_of(args.begin(), args.end(), [&] (const auto& arg) {
            return type::checkSubtype(*expectedType, arg->getType());
        });

    return ParseResult(std::make_unique<Coalesce>(needsAnnotation ? type::Value : *outputType, std::move(args)));
}

} // namespace expression
} // namespace style
} // namespace mbgl
