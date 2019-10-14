#include <mbgl/style/expression/assertion.hpp>
#include <mbgl/style/expression/check_subtype.hpp>
#include <mbgl/style/conversion_impl.hpp>

namespace mbgl {
namespace style {
namespace expression {

using namespace mbgl::style::conversion;

Assertion::Assertion(type::Type type_, std::vector<std::unique_ptr<Expression>> inputs_) :
    Expression(Kind::Assertion, type_),
    inputs(std::move(inputs_))
{
    assert(!inputs.empty());
}

ParseResult Assertion::parse(const Convertible& value, ParsingContext& ctx) {
    static std::unordered_map<std::string, type::Type> types{
        {"string", type::String},
        {"image", type::String}, // TODO: replace once we implement image expressions
        {"number", type::Number},
        {"boolean", type::Boolean},
        {"object", type::Object}};

    std::size_t length = arrayLength(value);

    if (length < 2) {
        ctx.error("Expected at least one argument.");
        return ParseResult();
    }

    std::size_t i = 1;
    type::Type type;

    std::string name = *toString(arrayMember(value, 0));
    if (name == "array") {
        optional<type::Type> itemType;
        if (length > 2) {
            optional<std::string> itemTypeName = toString(arrayMember(value, 1));
            auto it = itemTypeName ? types.find(*itemTypeName) : types.end();
            if (it == types.end() || it->second == type::Object) {
                ctx.error(
                    R"(The item type argument of "array" must be one of string, number, boolean)",
                    1
                );
                return ParseResult();
            }
            itemType = it->second;
            i++;
        } else {
            itemType = {type::Value};
        }

        optional<std::size_t> N;
        if (length > 3) {
            auto m = arrayMember(value, 2);
            optional<float> n = toNumber(m);
            if (!isUndefined(m) &&
                (!n || *n < 0 || *n != std::floor(*n))) {
                ctx.error(
                    R"(The length argument to "array" must be a positive integer literal.)",
                    2
                );
                return ParseResult();
            }
            if (n) {
                N = optional<std::size_t>(*n);
            }
            i++;
        }

        type = type::Array(*itemType, N);
    } else {
        type = types.at(name);
    }

    std::vector<std::unique_ptr<Expression>> parsed;
    parsed.reserve(length - 1);
    for (; i < length; i++) {
        ParseResult input = ctx.parse(arrayMember(value, i), i, {type::Value});
        if (!input) return ParseResult();
        parsed.push_back(std::move(*input));
    }

    return ParseResult(std::make_unique<Assertion>(type, std::move(parsed)));
}

std::string Assertion::getOperator() const {
    return getType().is<type::Array>() ? "array" : type::toString(getType());
}

EvaluationResult Assertion::evaluate(const EvaluationContext& params) const {
    for (std::size_t i = 0; i < inputs.size(); i++) {
        EvaluationResult value = inputs[i]->evaluate(params);
        if (!value) return value;
        if (!type::checkSubtype(getType(), typeOf(*value))) {
            return value;
        } else if (i == inputs.size() - 1) {
            return EvaluationError {
                "Expected value to be of type " + toString(getType()) +
                ", but found " + toString(typeOf(*value)) + " instead."
            };
        }
    }

    assert(false);
    return EvaluationError { "Unreachable" };
};

void Assertion::eachChild(const std::function<void(const Expression&)>& visit) const {
    for(const std::unique_ptr<Expression>& input : inputs) {
        visit(*input);
    }
};

bool Assertion::operator==(const Expression& e) const {
    if (e.getKind() == Kind::Assertion) {
        auto rhs = static_cast<const Assertion*>(&e);
        return getType() == rhs->getType() && Expression::childrenEqual(inputs, rhs->inputs);
    }
    return false;
}

std::vector<optional<Value>> Assertion::possibleOutputs() const {
    std::vector<optional<Value>> result;
    for (const auto& input : inputs) {
        for (auto& output : input->possibleOutputs()) {
            result.push_back(std::move(output));
        }
    }
    return result;
}

mbgl::Value Assertion::serialize() const {
    std::vector<mbgl::Value> serialized;
    serialized.emplace_back(getOperator());

    if (getType().is<type::Array>()) {
        const auto array = getType().get<type::Array>();
        if (array.itemType.is<type::StringType>()
         || array.itemType.is<type::NumberType>()
         || array.itemType.is<type::BooleanType>()) {
            serialized.emplace_back(type::toString(array.itemType));
            if (array.N) {
                serialized.emplace_back(uint64_t(*array.N));
            } else if (inputs.size() > 1) {
                serialized.emplace_back(mbgl::NullValue());
            }
        }
    }

    for (const auto& input : inputs) {
        serialized.emplace_back(input->serialize());
    }

    return serialized;
}

} // namespace expression
} // namespace style
} // namespace mbgl


