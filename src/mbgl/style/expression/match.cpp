#include <mbgl/style/expression/match.hpp>
#include <mbgl/style/expression/check_subtype.hpp>
#include <mbgl/style/expression/parsing_context.hpp>
#include <mbgl/util/string.hpp>

namespace mbgl {
namespace style {
namespace expression {

template <typename T>
void Match<T>::eachChild(const std::function<void(const Expression&)>& visit) const {
    visit(*input);
    for (const std::pair<T, std::shared_ptr<Expression>>& branch : branches) {
        visit(*branch.second);
    }
    visit(*otherwise);
}

template <typename T>
bool Match<T>::operator==(const Expression& e) const {
    if (auto rhs = dynamic_cast<const Match*>(&e)) {
        return (*input == *(rhs->input) &&
                *otherwise == *(rhs->otherwise) &&
                Expression::childrenEqual(branches, rhs->branches));
    }
    return false;
}

template <typename T>
std::vector<optional<Value>> Match<T>::possibleOutputs() const {
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

template <typename T>
mbgl::Value Match<T>::serialize() const {
    std::vector<mbgl::Value> serialized;
    serialized.emplace_back(getOperator());
    serialized.emplace_back(input->serialize());
    
    // Sort so serialization has an arbitrary defined order, even though branch order doesn't affect evaluation
    std::map<T, std::shared_ptr<Expression>> sortedBranches(branches.begin(), branches.end());

    // Group branches by unique match expression to support condensed serializations
    // of the form [case1, case2, ...] -> matchExpression
    std::map<Expression*, size_t> outputLookup;
    std::vector<std::pair<Expression*, std::vector<mbgl::Value>>> groupedByOutput;
    for (auto& entry : sortedBranches) {
        auto outputIndex = outputLookup.find(entry.second.get());
        if (outputIndex == outputLookup.end()) {
            // First time seeing this output, add it to the end of the grouped list
            outputLookup[entry.second.get()] = groupedByOutput.size();
            groupedByOutput.emplace_back(entry.second.get(), std::vector<mbgl::Value>{{entry.first}});
        } else {
            // We've seen this expression before, add the label to that output's group
            groupedByOutput[outputIndex->second].second.emplace_back(entry.first);
        }
    };
    
    for (auto& entry : groupedByOutput) {
        entry.second.size() == 1
            ? serialized.emplace_back(entry.second[0])       // Only a single label matches this output expression
            : serialized.emplace_back(entry.second);         // Array of literal labels pointing to this output expression
        serialized.emplace_back(entry.first->serialize());   // The output expression itself
    }
    
    serialized.emplace_back(otherwise->serialize());
    return serialized;
}
    

template<> EvaluationResult Match<std::string>::evaluate(const EvaluationContext& params) const {
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

template<> EvaluationResult Match<int64_t>::evaluate(const EvaluationContext& params) const {
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
optional<InputType> parseInputValue(const Convertible& input, ParsingContext& parentContext, std::size_t index, optional<type::Type>& inputType) {
    using namespace mbgl::style::conversion;
    optional<InputType> result;
    optional<type::Type> type;

    auto value = toValue(input);

    if (value) {
        value->match(
            [&] (uint64_t n) {
                if (!Value::isSafeInteger(n)) {
                    parentContext.error("Branch labels must be integers no larger than " + util::toString(Value::maxSafeInteger()) + ".", index);
                } else {
                    type = {type::Number};
                    result = {static_cast<int64_t>(n)};
                }
            },
            [&] (int64_t n) {
                if (!Value::isSafeInteger(n)) {
                    parentContext.error("Branch labels must be integers no larger than " + util::toString(Value::maxSafeInteger()) + ".", index);
                } else {
                    type = {type::Number};
                    result = {n};
                }
            },
            [&] (double n) {
                if (!Value::isSafeInteger(n)) {
                    parentContext.error("Branch labels must be integers no larger than " + util::toString(Value::maxSafeInteger()) + ".", index);
                } else if (n != std::floor(n)) {
                    parentContext.error("Numeric branch labels must be integer values.", index);
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
                parentContext.error("Branch labels must be numbers or strings.", index);
            }
        );
    } else {
        parentContext.error("Branch labels must be numbers or strings.", index);
    }

    if (!type) {
        return result;
    }

    if (!inputType) {
        inputType = type;
    } else {
        optional<std::string> err = type::checkSubtype(*inputType, *type);
        if (err) {
            parentContext.error(*err, index);
            return optional<InputType>();
        }
    }

    return result;
}

template <typename T>
static ParseResult create(type::Type outputType,
                          std::unique_ptr<Expression>input,
                          std::vector<std::pair<std::vector<InputType>,
                                                std::unique_ptr<Expression>>> branches,
                          std::unique_ptr<Expression> otherwise,
                          ParsingContext& ctx) {
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

ParseResult parseMatch(const Convertible& value, ParsingContext& ctx) {
    assert(isArray(value));
    auto length = arrayLength(value);
    if (length < 5) {
        ctx.error(
            "Expected at least 4 arguments, but found only " + util::toString(length - 1) + "."
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
    if (ctx.getExpected() && *ctx.getExpected() != type::Value) {
        outputType = ctx.getExpected();
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
                ctx.error("Expected at least one branch label.", i);
                return ParseResult();
            }
            
            labels.reserve(groupLength);
            for (size_t j = 0; j < groupLength; j++) {
                const optional<InputType> inputValue = parseInputValue(arrayMember(label, j), ctx, i, inputType);
                if (!inputValue) {
                    return ParseResult();
                }
                labels.push_back(*inputValue);
            }
        } else {
            const optional<InputType> inputValue = parseInputValue(label, ctx, i, inputType);
            if (!inputValue) {
                return ParseResult();
            }
            labels.push_back(*inputValue);
        }
        
        ParseResult output = ctx.parse(arrayMember(value, i + 1), i + 1, outputType);
        if (!output) {
            return ParseResult();
        }
        
        if (!outputType) {
            outputType = (*output)->getType();
        }
        
        branches.push_back(std::make_pair(std::move(labels), std::move(*output)));
    }

    auto input = ctx.parse(arrayMember(value, 1), 1, inputType);
    if (!input) {
        return ParseResult();
    }

    auto otherwise = ctx.parse(arrayMember(value, length - 1), length - 1, outputType);
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
