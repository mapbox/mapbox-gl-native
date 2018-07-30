#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/parsing_context.hpp>
#include <mbgl/style/conversion.hpp>

#include <memory>
#include <map>

namespace mbgl {
namespace style {
namespace expression {

class Let : public Expression {
public:
    using Bindings = std::map<std::string, std::shared_ptr<Expression>>;
    
    Let(Bindings bindings_, std::unique_ptr<Expression> result_) :
        Expression(Kind::Let, result_->getType()),
        bindings(std::move(bindings_)),
        result(std::move(result_))
    {}
    
    static ParseResult parse(const mbgl::style::conversion::Convertible&, ParsingContext&);
    
    EvaluationResult evaluate(const EvaluationContext& params) const override;
    void eachChild(const std::function<void(const Expression&)>&) const override;

    bool operator==(const Expression& e) const override {
        if (e.getKind() == Kind::Let) {
            auto rhs = static_cast<const Let*>(&e);
            return *result == *(rhs->result);
        }
        return false;
    }

    std::vector<optional<Value>> possibleOutputs() const override;

    Expression* getResult() const {
        return result.get();
    }

    mbgl::Value serialize() const override;
    std::string getOperator() const override { return "let"; }
private:
    Bindings bindings;
    std::unique_ptr<Expression> result;
};

class Var : public Expression {
public:
    Var(std::string name_, std::shared_ptr<Expression> value_) :
        Expression(Kind::Var, value_->getType()),
        name(std::move(name_)),
        value(value_)
    {}

    static ParseResult parse(const mbgl::style::conversion::Convertible&, ParsingContext&);

    EvaluationResult evaluate(const EvaluationContext& params) const override;
    void eachChild(const std::function<void(const Expression&)>&) const override;

    bool operator==(const Expression& e) const override {
        if (e.getKind() == Kind::Var) {
            auto rhs = static_cast<const Var*>(&e);
            return *value == *(rhs->value);
        }
        return false;
    }

    std::vector<optional<Value>> possibleOutputs() const override;

    mbgl::Value serialize() const override;
    std::string getOperator() const override { return "var"; }
    
    const std::shared_ptr<Expression>& getBoundExpression() const { return value; }
    
private:
    std::string name;
    std::shared_ptr<Expression> value;
};

} // namespace expression
} // namespace style
} // namespace mbgl
