#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/conversion.hpp>
#include <memory>

namespace mbgl {
namespace style {
namespace expression {

class Any : public Expression  {
public:
    Any(std::vector<std::unique_ptr<Expression>> inputs_) :
        Expression(type::Boolean),
        inputs(std::move(inputs_))
    {}

    static ParseResult parse(const mbgl::style::conversion::Convertible& value, ParsingContext& ctx);

    EvaluationResult evaluate(const EvaluationContext& params) const override;
    void eachChild(const std::function<void(const Expression*)>& visit) const override;

    bool operator==(const Expression& e) const override {
        if (auto rhs = dynamic_cast<const Any*>(&e)) {
            if (inputs.size() != rhs->inputs.size()) return false;
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

class All : public Expression  {
public:
    All(std::vector<std::unique_ptr<Expression>> inputs_) :
        Expression(type::Boolean),
        inputs(std::move(inputs_))
    {}

    static ParseResult parse(const mbgl::style::conversion::Convertible& value, ParsingContext& ctx);

    EvaluationResult evaluate(const EvaluationContext& params) const override;
    void eachChild(const std::function<void(const Expression*)>& visit) const override;

    bool operator==(const Expression& e) const override {
        if (auto rhs = dynamic_cast<const All*>(&e)) {
            if (inputs.size() != rhs->inputs.size()) return false;
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

