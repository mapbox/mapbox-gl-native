#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/expression/parsing_context.hpp>

#include <memory>
#include <vector>

namespace mbgl {
namespace style {
namespace expression {

class Case : public Expression {
public:
    using Branch = std::pair<std::unique_ptr<Expression>, std::unique_ptr<Expression>>;

    Case(type::Type type_, std::vector<Branch> branches_, std::unique_ptr<Expression> otherwise_)
        : Expression(type_), branches(std::move(branches_)), otherwise(std::move(otherwise_)) {
    }

    static ParseResult parse(const mbgl::style::conversion::Convertible& value, ParsingContext& ctx);

    EvaluationResult evaluate(const EvaluationContext& params) const override;
    void eachChild(const std::function<void(const Expression&)>& visit) const override;
    
    bool operator==(const Expression& e) const override;

    std::vector<optional<Value>> possibleOutputs() const override;

    std::string getOperator() const override { return "case"; }
private:
    std::vector<Branch> branches;
    std::unique_ptr<Expression> otherwise;
};

} // namespace expression
} // namespace style
} // namespace mbgl
