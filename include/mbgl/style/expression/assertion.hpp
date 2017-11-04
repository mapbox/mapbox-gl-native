#pragma once
#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/expression/parsing_context.hpp>
#include <memory>
#include <vector>

namespace mbgl {
namespace style {
namespace expression {

class Assertion : public Expression  {
public:
    Assertion(type::Type type_, std::vector<std::unique_ptr<Expression>> inputs_) :
        Expression(type_),
        inputs(std::move(inputs_))
    {}

    static ParseResult parse(const mbgl::style::conversion::Convertible& value, ParsingContext& ctx);

    EvaluationResult evaluate(const EvaluationContext& params) const override;
    void eachChild(const std::function<void(const Expression*)>& visit) const override;
    
    bool operator==(const Expression& e) const override {
        if (auto rhs = dynamic_cast<const Assertion*>(&e)) {
            if (getType() != rhs->getType() || inputs.size() != rhs->inputs.size()) return false;
            for (auto leftChild = inputs.begin(), rightChild = rhs->inputs.begin();
                 leftChild != inputs.end();
                 leftChild++, rightChild++)
             {
                 if (**leftChild != **rightChild) return false;
             }
             return true;
        }
        return false;
    }

private:
    std::vector<std::unique_ptr<Expression>> inputs;
};

} // namespace expression
} // namespace style
} // namespace mbgl

