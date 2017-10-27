#include <mbgl/style/expression/match.hpp>
#include <mbgl/style/expression/parsing_context.hpp>

namespace mbgl {
namespace style {
namespace expression {

template <typename T>
void Match<T>::eachChild(std::function<void(const Expression*)> visit) const {
    visit(input.get());
    for (const std::pair<T, std::shared_ptr<Expression>>& branch : branches) {
        visit(branch.second.get());
    }
    visit(otherwise.get());
}

template<> EvaluationResult Match<std::string>::evaluate(const EvaluationParameters& params) const {
    const EvaluationResult inputValue = input->evaluate(params);
    if (!inputValue) {
        return inputValue.error();
    }

    auto it = branches.find(inputValue->get<std::string>());
    if (it != branches.end()) {
        return (*it).second->evaluate(params);
    }

    return otherwise->evaluate(params);
}

template<> EvaluationResult Match<int64_t>::evaluate(const EvaluationParameters& params) const {
    const EvaluationResult inputValue = input->evaluate(params);
    if (!inputValue) {
        return inputValue.error();
    }
    
    const auto numeric = inputValue->get<double>();
    int64_t rounded = std::floor(numeric);
    if (numeric == rounded) {
        auto it = branches.find(rounded);
        if (it != branches.end()) {
            return (*it).second->evaluate(params);
        }
    }
    
    return otherwise->evaluate(params);
}

template class Match<int64_t>;
template class Match<std::string>;

using InputType = variant<int64_t, std::string>;

using namespace mbgl::style::conversion;
optional<InputType> parseInputValue(const Convertible& input, ParsingContext ctx, optional<type::Type>& inputType) {
    using namespace mbgl::style::conversion;
    optional<InputType> result;
    optional<type::Type> type;

    auto value = toValue(input);

    if (value) {
        value->match(
            [&] (uint64_t n) {
                if (!Value::isSafeInteger(n)) {
                    ctx.error("Branch labels must be integers no larger than " + std::to_string(Value::maxSafeInteger()) + ".");
                } else {
                    type = {type::Number};
                    result = {static_cast<int64_t>(n)};
                }
            },
            [&] (int64_t n) {
                if (!Value::isSafeInteger(n)) {
                    ctx.error("Branch labels must be integers no larger than " + std::to_string(Value::maxSafeInteger()) + ".");
                } else {
                    type = {type::Number};
                    result = {n};
                }
            },
            [&] (double n) {
                if (!Value::isSafeInteger(n)) {
                    ctx.error("Branch labels must be integers no larger than " + std::to_string(Value::maxSafeInteger()) + ".");
                } else if (n != std::floor(n)) {
                    ctx.error("Numeric branch labels must be integer values.");
                } else {
                    type = {type::Number};
                    result = {static_cast<int64_t>(n)};
                }
            },
            [&] (const std::string& s) {
                type = {type::String};
                result = {s};
            },
            [&] (const auto&) {
                ctx.error("Branch labels must be numbers or strings.");
            }
        );
    } else {
        ctx.error("Branch labels must be numbers or strings.");
    }

    if (!type) {
        return result;
    }

    if (!inputType) {
        inputType = type;
    } else if (ctx.checkType(*type)) {
        return optional<InputType>();
    }

    return result;
}

template <typename T>
static ParseResult create(type::Type outputType,
                          std::unique_ptr<Expression>input,
                          std::vector<std::pair<std::vector<InputType>,
                                                std::unique_ptr<Expression>>> branches,
                          std::unique_ptr<Expression> otherwise,
                          ParsingContext ctx) {
    typename Match<T>::Branches typedBranches;
    
    std::size_t index = 2;

    typedBranches.reserve(branches.size());
    for (std::pair<std::vector<InputType>,
                   std::unique_ptr<Expression>>& pair : branches) {
        std::shared_ptr<Expression> result = std::move(pair.second);
        for (const InputType& label : pair.first) {
            const auto& typedLabel = label.template get<T>();
            if (typedBranches.find(typedLabel) != typedBranches.end()) {
                ctx.error("Branch labels must be unique.", index);
                return ParseResult();
            }
            typedBranches.emplace(typedLabel, result);
        }
        
        index += 2;
    }
    return ParseResult(std::make_unique<Match<T>>(
        outputType,
        std::move(input),
        std::move(typedBranches),
        std::move(otherwise)
    ));
}

ParseResult parseMatch(const Convertible& value, ParsingContext ctx) {
    assert(isArray(value));
    auto length = arrayLength(value);
    if (length < 5) {
        ctx.error(
            "Expected at least 4 arguments, but found only " + std::to_string(length - 1) + "."
        );
        return ParseResult();
    }

    // Expect odd-length array: ["match", input, 2 * (n pairs)..., otherwise]
    if (length % 2 != 1) {
        ctx.error("Expected an even number of arguments.");
        return ParseResult();
    }

    optional<type::Type> inputType;
    optional<type::Type> outputType;
    if (ctx.expected && *ctx.expected != type::Value) {
        outputType = ctx.expected;
    }

    std::vector<std::pair<std::vector<InputType>,
                          std::unique_ptr<Expression>>> branches;

    branches.reserve((length - 3) / 2);
    for (size_t i = 2; i + 1 < length; i += 2) {
        const auto& label = arrayMember(value, i);

        std::vector<InputType> labels;
        // Match pair inputs are provided as either a literal value or a
        // raw JSON array of string / number / boolean values.
        if (isArray(label)) {
            auto groupLength = arrayLength(label);
            if (groupLength == 0) {
                ctx.concat(i).error("Expected at least one branch label.");
                return ParseResult();
            }
            
            labels.reserve(groupLength);
            for (size_t j = 0; j < groupLength; j++) {
                const optional<InputType> inputValue = parseInputValue(arrayMember(label, j), ctx.concat(i, inputType), inputType);
                if (!inputValue) {
                    return ParseResult();
                }
                labels.push_back(*inputValue);
            }
        } else {
            const optional<InputType> inputValue = parseInputValue(label, ctx.concat(i, inputType), inputType);
            if (!inputValue) {
                return ParseResult();
            }
            labels.push_back(*inputValue);
        }
        
        ParseResult output = ctx.concat(i + 1, outputType).parse(arrayMember(value, i + 1));
        if (!output) {
            return ParseResult();
        }
        
        if (!outputType) {
            outputType = (*output)->getType();
        }
        
        branches.push_back(std::make_pair(std::move(labels), std::move(*output)));
    }

    auto input = ctx.concat(1, inputType).parse(arrayMember(value, 1));
    if (!input) {
        return ParseResult();
    }

    auto otherwise = ctx.concat(length - 1, outputType).parse(arrayMember(value, length - 1));
    if (!otherwise) {
        return ParseResult();
    }

    assert(inputType && outputType);

    return inputType->match(
        [&](const type::NumberType&) {
            return create<int64_t>(*outputType, std::move(*input), std::move(branches), std::move(*otherwise), ctx);
        },
        [&](const type::StringType&) {
            return create<std::string>(*outputType, std::move(*input), std::move(branches), std::move(*otherwise), ctx);
        },
        [&](const auto&) {
            // unreachable: inputType is set by parseInputValue(), which only
            // accepts string and (integer) numeric values.
            assert(false);
            return ParseResult();
        }
    );
}

} // namespace expression
} // namespace style
} // namespace mbgl
