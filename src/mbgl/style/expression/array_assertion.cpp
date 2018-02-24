#include <mbgl/style/expression/array_assertion.hpp>
#include <mbgl/style/expression/check_subtype.hpp>
#include <mbgl/util/string.hpp>

namespace mbgl {
namespace style {
namespace expression {

EvaluationResult ArrayAssertion::evaluate(const EvaluationContext& params) const {
    auto result = input->evaluate(params);
    if (!result) {
        return result.error();
    }
    type::Type expected = getType();
    type::Type actual = typeOf(*result);
    if (checkSubtype(expected, actual)) {
        return EvaluationError {
            "Expected value to be of type " + toString(expected) +
            ", but found " + toString(actual) + " instead."
        };
    }
    return *result;
}

void ArrayAssertion::eachChild(const std::function<void(const Expression&)>& visit) const {
    visit(*input);
}

using namespace mbgl::style::conversion;
ParseResult ArrayAssertion::parse(const Convertible& value, ParsingContext& ctx) {

    static std::unordered_map<std::string, type::Type> itemTypes {
        {"string", type::String},
        {"number", type::Number},
        {"boolean", type::Boolean}
    };

    auto length = arrayLength(value);
    if (length < 2 || length > 4) {
        ctx.error("Expected 1, 2, or 3 arguments, but found " + util::toString(length - 1) + " instead.");
        return ParseResult();
    }

    optional<type::Type> itemType;
    optional<std::size_t> N;
    if (length > 2) {
        optional<std::string> itemTypeName = toString(arrayMember(value, 1));
        auto it = itemTypeName ? itemTypes.find(*itemTypeName) : itemTypes.end();
        if (it == itemTypes.end()) {
            ctx.error(
                R"(The item type argument of "array" must be one of string, number, boolean)",
                1
            );
            return ParseResult();
        }
        itemType = it->second;
    } else {
        itemType = {type::Value};
    }

    if (length > 3) {
        auto n = toNumber(arrayMember(value, 2));
        if (!n || *n != std::floor(*n)) {
            ctx.error(
                R"(The length argument to "array" must be a positive integer literal.)",
                2
            );
            return ParseResult();
        }
        N = optional<std::size_t>(*n);
    }

    auto input = ctx.parse(arrayMember(value, length - 1), length - 1, {type::Value});
    if (!input) {
        return input;
    }

    return ParseResult(std::make_unique<ArrayAssertion>(
        type::Array(*itemType, N),
        std::move(*input)
    ));
}

mbgl::Value ArrayAssertion::serialize() const {
    std::vector<mbgl::Value> serialized;
    serialized.emplace_back(getOperator());
    
    
    const auto array = getType().get<type::Array>();
    if (array.itemType.is<type::StringType>()
     || array.itemType.is<type::NumberType>()
     || array.itemType.is<type::BooleanType>()) {
        serialized.emplace_back(type::toString(array.itemType));
        if (array.N) {
            serialized.emplace_back(uint64_t(*array.N));
        }
    }
    
    serialized.emplace_back(input->serialize());
    return serialized;
}

} // namespace expression
} // namespace style
} // namespace mbgl
