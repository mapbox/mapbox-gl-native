#include <mbgl/style/expression/assertion.hpp>
#include <mbgl/style/expression/check_subtype.hpp>

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
    static std::unordered_map<std::string, type::Type> types {
        {"string", type::String},
        {"number", type::Number},
        {"boolean", type::Boolean},
        {"object", type::Object}
    };

    std::size_t length = arrayLength(value);

    if (length < 2) {
        ctx.error("Expected at least one argument.");
        return ParseResult();
    }

    auto it = types.find(*toString(arrayMember(value, 0)));
    assert(it != types.end());
    
    std::vector<std::unique_ptr<Expression>> parsed;
    parsed.reserve(length - 1);
    for (std::size_t i = 1; i < length; i++) {
        ParseResult input = ctx.parse(arrayMember(value, i), i, {type::Value});
        if (!input) return ParseResult();
        parsed.push_back(std::move(*input));
    }

    return ParseResult(std::make_unique<Assertion>(it->second, std::move(parsed)));
}

std::string Assertion::getOperator() const {
    return type::toString(getType());
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

} // namespace expression
} // namespace style
} // namespace mbgl


