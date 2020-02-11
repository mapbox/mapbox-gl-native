#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/expression/expression.hpp>
#include <memory>

namespace mbgl {
namespace style {
namespace expression {

class In final : public Expression {
public:
    In(std::unique_ptr<Expression> needle_, std::unique_ptr<Expression> haystack_)
        : Expression(Kind::In, type::Boolean), needle(std::move(needle_)), haystack(std::move(haystack_)) {}

    static ParseResult parse(const mbgl::style::conversion::Convertible& value, ParsingContext& ctx);

    EvaluationResult evaluate(const EvaluationContext& params) const override;
    void eachChild(const std::function<void(const Expression&)>&) const override;

    bool operator==(const Expression& e) const override {
        if (e.getKind() == Kind::In) {
            auto rhs = static_cast<const In*>(&e);
            return *needle == *(rhs->needle) && *haystack == *(rhs->haystack);
        }
        return false;
    }

    std::vector<optional<Value>> possibleOutputs() const override { return {{true}, {false}}; }

    std::string getOperator() const override { return "in"; }

private:
    std::unique_ptr<Expression> needle;
    std::unique_ptr<Expression> haystack;
};

} // namespace expression
} // namespace style
} // namespace mbgl
