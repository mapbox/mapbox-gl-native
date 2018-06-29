#pragma once

#include <mbgl/style/expression/collator_expression.hpp>
#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/parsing_context.hpp>
#include <mbgl/style/conversion.hpp>

#include <memory>

namespace mbgl {
namespace style {
namespace expression {

class Equals : public Expression {
public:
    Equals(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs, optional<std::unique_ptr<Expression>> collator, bool negate);

    static ParseResult parse(const mbgl::style::conversion::Convertible&, ParsingContext&);

    void eachChild(const std::function<void(const Expression&)>& visit) const override;
    bool operator==(const Expression&) const override;
    EvaluationResult evaluate(const EvaluationContext&) const override;
    std::vector<optional<Value>> possibleOutputs() const override;

    std::string getOperator() const override { return negate ? "!=" : "=="; }
private:
    std::unique_ptr<Expression> lhs;
    std::unique_ptr<Expression> rhs;
    optional<std::unique_ptr<Expression>> collator;
    bool negate;
};

} // namespace expression
} // namespace style
} // namespace mbgl
