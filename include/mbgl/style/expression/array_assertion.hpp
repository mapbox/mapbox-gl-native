#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/type.hpp>
#include <mbgl/style/expression/parsing_context.hpp>
#include <mbgl/style/conversion.hpp>

#include <memory>

namespace mbgl {
namespace style {
namespace expression {

class ArrayAssertion : public Expression  {
public:
    ArrayAssertion(type::Array type_, std::unique_ptr<Expression> input_) :
        Expression(type_),
        input(std::move(input_))
    {}

    static ParseResult parse(const mbgl::style::conversion::Convertible& value, ParsingContext& ctx);

    EvaluationResult evaluate(const EvaluationContext& params) const override;
    void eachChild(const std::function<void(const Expression&)>& visit) const override;

    bool operator==(const Expression& e) const override {
        if (auto rhs = dynamic_cast<const ArrayAssertion*>(&e)) {
            return getType() == rhs->getType() && *input == *(rhs->input);
        }
        return false;
    }

    std::vector<optional<Value>> possibleOutputs() const override {
        return input->possibleOutputs();
    }
    
    mbgl::Value serialize() const override;
    std::string getOperator() const override { return "array"; }

private:
    std::unique_ptr<Expression> input;
};

} // namespace expression
} // namespace style
} // namespace mbgl
