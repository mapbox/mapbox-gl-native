#pragma once

#include <mbgl/style/expression/expression.hpp>

namespace mbgl {
namespace style {
namespace expression {

class ParsingContext;

class ImageExpression final : public Expression {
public:
    explicit ImageExpression(std::unique_ptr<Expression> imageID);

    EvaluationResult evaluate(const EvaluationContext&) const override;
    static ParseResult parse(const mbgl::style::conversion::Convertible&, ParsingContext&);

    void eachChild(const std::function<void(const Expression&)>&) const override;

    bool operator==(const Expression& e) const override;

    std::vector<optional<Value>> possibleOutputs() const override { return {nullopt}; }

    mbgl::Value serialize() const override;
    std::string getOperator() const override { return "image"; }

private:
    std::shared_ptr<Expression> imageID;
};

} // namespace expression
} // namespace style
} // namespace mbgl
