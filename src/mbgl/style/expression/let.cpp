#include <mbgl/style/expression/let.hpp>
#include <mbgl/style/conversion/get_json_type.hpp>
#include <mbgl/util/string.hpp>

namespace mbgl {
namespace style {
namespace expression {

EvaluationResult Let::evaluate(const EvaluationContext& params) const {
    return result->evaluate(params);
}

void Let::eachChild(const std::function<void(const Expression&)>& visit) const {
    for (auto it = bindings.begin(); it != bindings.end(); it++) {
        visit(*it->second);
    }
    visit(*result);
}

using namespace mbgl::style::conversion;

ParseResult Let::parse(const Convertible& value, ParsingContext& ctx) {
    assert(isArray(value));

    std::size_t length = arrayLength(value);

    if (length < 4) {
        ctx.error("Expected at least 3 arguments, but found " + util::toString(length - 1) + " instead.");
        return ParseResult();
    }

    std::map<std::string, std::shared_ptr<Expression>> bindings_;
    for(std::size_t i = 1; i < length - 1; i += 2) {
        optional<std::string> name = toString(arrayMember(value, i));
        if (!name) {
            ctx.error("Expected string, but found " + getJSONType(arrayMember(value, i)) + " instead.", i);
            return ParseResult();
        }
        
        bool isValidName = std::all_of(name->begin(), name->end(), [](unsigned char c) {
            return ::isalnum(c) || c == '_';
        });
        if (!isValidName) {
            ctx.error("Variable names must contain only alphanumeric characters or '_'.", 1);
            return ParseResult();
        }
        
        ParseResult bindingValue = ctx.parse(arrayMember(value, i + 1), i + 1);
        if (!bindingValue) {
            return ParseResult();
        }
        
        bindings_.emplace(*name, std::move(*bindingValue));
    }

    ParseResult result_ = ctx.parse(arrayMember(value, length - 1), length - 1, ctx.getExpected(), bindings_);
    if (!result_) {
        return ParseResult();
    }

    return ParseResult(std::make_unique<Let>(std::move(bindings_), std::move(*result_)));
}

EvaluationResult Var::evaluate(const EvaluationContext& params) const {
    return value->evaluate(params);
}

void Var::eachChild(const std::function<void(const Expression&)>&) const {}

ParseResult Var::parse(const Convertible& value_, ParsingContext& ctx) {
    assert(isArray(value_));

    if (arrayLength(value_) != 2 || !toString(arrayMember(value_, 1))) {
        ctx.error("'var' expression requires exactly one string literal argument.");
        return ParseResult();
    }

    std::string name_ = *toString(arrayMember(value_, 1));

    optional<std::shared_ptr<Expression>> bindingValue = ctx.getBinding(name_);
    if (!bindingValue) {
        ctx.error(R"(Unknown variable ")" + name_ +  R"(". Make sure ")" +
            name_ + R"(" has been bound in an enclosing "let" expression before using it.)", 1);
        return ParseResult();
    }

    return ParseResult(std::make_unique<Var>(name_, std::move(*bindingValue)));
}

} // namespace expression
} // namespace style
} // namespace mbgl
