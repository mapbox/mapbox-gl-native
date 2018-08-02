#pragma once

#include <mbgl/style/expression/collator_expression.hpp>
#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/parsing_context.hpp>
#include <mbgl/style/conversion.hpp>

#include <memory>

namespace mbgl {
namespace style {
namespace expression {

ParseResult parseComparison(const mbgl::style::conversion::Convertible&, ParsingContext&);

class BasicComparison : public Expression {
public:
    using CompareFunctionType = bool (*) (Value, Value);

    BasicComparison(
        std::string op,
        std::unique_ptr<Expression> lhs,
        std::unique_ptr<Expression> rhs);

    void eachChild(const std::function<void(const Expression&)>& visit) const override;
    bool operator==(const Expression&) const override;
    EvaluationResult evaluate(const EvaluationContext&) const override;
    std::vector<optional<Value>> possibleOutputs() const override;
    std::string getOperator() const override;

private:
    std::string op;
    CompareFunctionType compare;
    std::unique_ptr<Expression> lhs;
    std::unique_ptr<Expression> rhs;
    bool needsRuntimeTypeCheck;
};

class CollatorComparison : public Expression {
public:
    using CompareFunctionType = bool (*) (std::string, std::string, Collator);

    CollatorComparison(
        std::string op,
        std::unique_ptr<Expression> lhs,
        std::unique_ptr<Expression> rhs,
        std::unique_ptr<Expression> collator);

    void eachChild(const std::function<void(const Expression&)>& visit) const override;
    bool operator==(const Expression&) const override;
    EvaluationResult evaluate(const EvaluationContext&) const override;
    std::vector<optional<Value>> possibleOutputs() const override;
    std::string getOperator() const override;

private:
    std::string op;
    CompareFunctionType compare;
    std::unique_ptr<Expression> lhs;
    std::unique_ptr<Expression> rhs;
    std::unique_ptr<Expression> collator;
    bool needsRuntimeTypeCheck;
};



} // namespace expression
} // namespace style
} // namespace mbgl
