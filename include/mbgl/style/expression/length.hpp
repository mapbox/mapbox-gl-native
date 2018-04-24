#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/expression/parsing_context.hpp>

#include <memory>
#include <vector>

namespace mbgl {
namespace style {
namespace expression {

class Length : public Expression {
public:
    Length(std::unique_ptr<Expression> input);

    static ParseResult parse(const mbgl::style::conversion::Convertible& value, ParsingContext& ctx);

    EvaluationResult evaluate(const EvaluationContext& params) const override;
    void eachChild(const std::function<void(const Expression&)>& visit) const override;
    bool operator==(const Expression& e) const override;
    std::vector<optional<Value>> possibleOutputs() const override;
    std::string getOperator() const override { return "length"; }

private:
    std::unique_ptr<Expression> input;
};

} // namespace expression
} // namespace style
} // namespace mbgl
