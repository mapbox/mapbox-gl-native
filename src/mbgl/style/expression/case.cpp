#include <mbgl/style/expression/case.hpp>

namespace mbgl {
namespace style {
namespace expression {

EvaluationResult Case::evaluate(const EvaluationParameters& params) const {
    for (const auto& branch : branches) {
        const EvaluationResult evaluatedTest = branch.first->evaluate(params);
        if (!evaluatedTest) {
            return evaluatedTest.error();
        }
        if (evaluatedTest->get<bool>()) {
            return branch.second->evaluate(params);
        }
    }
    
    return otherwise->evaluate(params);
}

void Case::eachChild(std::function<void(const Expression*)> visit) const {
    for (const Branch& branch : branches) {
        visit(branch.first.get());
        visit(branch.second.get());
    }
    visit(otherwise.get());
}

ParseResult Case::parse(const mbgl::style::conversion::Convertible& value, ParsingContext ctx) {
    using namespace mbgl::style::conversion;

    assert(isArray(value));
    auto length = arrayLength(value);
    if (length < 4) {
        ctx.error("Expected at least 3 arguments, but found only " + std::to_string(length - 1) + ".");
        return ParseResult();
    }

    // Expect even-length array: ["case", 2 * (n pairs)..., otherwise]
    if (length % 2 != 0) {
        ctx.error("Expected an odd number of arguments");
        return ParseResult();
    }

    optional<type::Type> outputType;
    if (ctx.expected && *ctx.expected != type::Value) {
        outputType = ctx.expected;
    }

    std::vector<Case::Branch> branches;
    for (size_t i = 1; i + 1 < length; i += 2) {
        auto test = ctx.concat(i, {type::Boolean}).parse(arrayMember(value, i));
        if (!test) {
            return test;
        }

        auto output = ctx.concat(i + 1, outputType).parse(arrayMember(value, i + 1));
        if (!output) {
            return output;
        }
        
        if (!outputType) {
            outputType = (*output)->getType();
        }

        branches.push_back(std::make_pair(std::move(*test), std::move(*output)));
    }

    assert(outputType);

    auto otherwise = ctx.concat(length - 1, outputType).parse(arrayMember(value, length - 1));
    if (!otherwise) {
        return otherwise;
    }

    return ParseResult(std::make_unique<Case>(*outputType,
                                  std::move(branches),
                                  std::move(*otherwise)));
}

} // namespace expression
} // namespace style
} // namespace mbgl
