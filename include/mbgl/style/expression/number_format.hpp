#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/parsing_context.hpp>

namespace mbgl {
namespace style {
namespace expression {

class NumberFormat final : public Expression {
public:
    NumberFormat(std::unique_ptr<Expression> number_,
                 std::unique_ptr<Expression> locale_,
                 std::unique_ptr<Expression> currency_,
                 std::unique_ptr<Expression> minFractionDigits_,
                 std::unique_ptr<Expression> maxFractionDigits_);

    ~NumberFormat();

    static ParseResult parse(const mbgl::style::conversion::Convertible& value, ParsingContext& ctx);

    EvaluationResult evaluate(const EvaluationContext& params) const override;
    void eachChild(const std::function<void(const Expression&)>& visit) const override;
    bool operator==(const Expression& e) const override;
    std::vector<optional<Value>> possibleOutputs() const override;

    mbgl::Value serialize() const override;
    std::string getOperator() const override { return "number-format"; }

private:
    std::unique_ptr<Expression> number;
    std::unique_ptr<Expression> locale;
    std::unique_ptr<Expression> currency;
    std::unique_ptr<Expression> minFractionDigits;
    std::unique_ptr<Expression> maxFractionDigits;
};

} // namespace expression
} // namespace style
} // namespace mbgl
