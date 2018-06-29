#pragma once

#include <mbgl/style/expression/expression.hpp>

#include <string>

namespace mbgl {
namespace style {
namespace expression {

class Error : public Expression {
public:
    Error(std::string message_) 
        : Expression(type::Error),
          message(std::move(message_)) {}

    void eachChild(const std::function<void(const Expression&)>&) const override {}

    bool operator==(const Expression& e) const override {
        return dynamic_cast<const Error*>(&e);
    }

    EvaluationResult evaluate(const EvaluationContext&) const override {
        return EvaluationError{message};
    }

    std::vector<optional<Value>> possibleOutputs() const override {
        return {};
    }

    std::string getOperator() const override { return "error"; }

private:
    std::string message;
};

} // namespace expression
} // namespace style
} // namespace mbgl
