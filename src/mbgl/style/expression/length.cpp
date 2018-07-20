#include <mbgl/style/expression/length.hpp>
#include <mbgl/util/string.hpp>

namespace mbgl {
namespace style {
namespace expression {

Length::Length(std::unique_ptr<Expression> input_)
    : Expression(Kind::Length, type::Number),
      input(std::move(input_)) {
}

EvaluationResult Length::evaluate(const EvaluationContext& params) const {
    const EvaluationResult value = input->evaluate(params);
    if (!value) return value;
    return value->match(
        [] (const std::string& s) {
            return EvaluationResult { double(s.size()) };
        },
        [] (const std::vector<Value>& v) {
            return EvaluationResult { double(v.size()) };
        },
        [&] (const auto&) -> EvaluationResult {
            return EvaluationError { "Expected value to be of type string or array, but found " + toString(typeOf(*value)) + " instead." };
        });
}

void Length::eachChild(const std::function<void(const Expression&)>& visit) const {
    visit(*input);
}

bool Length::operator==(const Expression& e) const {
    if (e.getKind() == Kind::Length) {
        auto eq = static_cast<const Length*>(&e);
        return *eq->input == *input;
    }
    return false;
}

std::vector<optional<Value>> Length::possibleOutputs() const {
    return { nullopt };
}

using namespace mbgl::style::conversion;
ParseResult Length::parse(const Convertible& value, ParsingContext& ctx) {
    std::size_t length = arrayLength(value);

    if (length != 2) {
        ctx.error("Expected one argument, but found " + util::toString(length) + " instead.");
        return ParseResult();
    }

    ParseResult input = ctx.parse(arrayMember(value, 1), 1);
    if (!input) return ParseResult();

    type::Type type = (*input)->getType();
    if (!type.is<type::Array>() && !type.is<type::StringType>() && !type.is<type::ValueType>()) {
        ctx.error("Expected argument of type string or array, but found " + toString(type) + " instead.");
        return ParseResult();
    }

    return ParseResult(std::make_unique<Length>(std::move(*input)));
}

} // namespace expression
} // namespace style
} // namespace mbgl
