#include <mbgl/style/expression/case.hpp>
#include <mbgl/util/string.hpp>

namespace mbgl {
namespace style {
namespace expression {

EvaluationResult Case::evaluate(const EvaluationContext& params) const {
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

void Case::eachChild(const std::function<void(const Expression&)>& visit) const {
    for (const Branch& branch : branches) {
        visit(*branch.first);
        visit(*branch.second);
    }
    visit(*otherwise);
}

bool Case::operator==(const Expression& e) const {
    if (auto rhs = dynamic_cast<const Case*>(&e)) {
        return *otherwise == *(rhs->otherwise) && Expression::childrenEqual(branches, rhs->branches);
    }
    return false;
}

std::vector<optional<Value>> Case::possibleOutputs() const {
    std::vector<optional<Value>> result;
    for (const auto& branch : branches) {
        for (auto& output : branch.second->possibleOutputs()) {
            result.push_back(std::move(output));
        }
    }
    for (auto& output : otherwise->possibleOutputs()) {
        result.push_back(std::move(output));
    }
    return result;
}

using namespace mbgl::style::conversion;
ParseResult Case::parse(const Convertible& value, ParsingContext& ctx) {
    assert(isArray(value));
    auto length = arrayLength(value);
    if (length < 4) {
        ctx.error("Expected at least 3 arguments, but found only " + util::toString(length - 1) + ".");
        return ParseResult();
    }

    // Expect even-length array: ["case", 2 * (n pairs)..., otherwise]
    if (length % 2 != 0) {
        ctx.error("Expected an odd number of arguments");
        return ParseResult();
    }

    optional<type::Type> outputType;
    if (ctx.getExpected() && *ctx.getExpected() != type::Value) {
        outputType = ctx.getExpected();
    }

    std::vector<Case::Branch> branches;
    branches.reserve((length - 2) / 2);
    for (size_t i = 1; i + 1 < length; i += 2) {
        auto test = ctx.parse(arrayMember(value, i), i, {type::Boolean});
        if (!test) {
            return test;
        }

        auto output = ctx.parse(arrayMember(value, i + 1), i + 1, outputType);
        if (!output) {
            return output;
        }
        
        if (!outputType) {
            outputType = (*output)->getType();
        }

        branches.push_back(std::make_pair(std::move(*test), std::move(*output)));
    }

    assert(outputType);

    auto otherwise = ctx.parse(arrayMember(value, length - 1), length - 1, outputType);
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
