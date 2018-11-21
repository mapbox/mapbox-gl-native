#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/parsing_context.hpp>
#include <mbgl/style/conversion.hpp>

#include <memory>

namespace mbgl {
namespace style {
namespace expression {

class Literal : public Expression {
public:
    Literal(Value value_)
        : Expression(Kind::Literal, typeOf(value_))
        , value(value_)
    {}
    
    Literal(type::Array type_, std::vector<Value> value_)
        : Expression(Kind::Literal, type_)
        , value(value_)
    {}

    EvaluationResult evaluate(const EvaluationContext&) const override {
        return value;
    }
    
    static ParseResult parse(const mbgl::style::conversion::Convertible&, ParsingContext&);

    void eachChild(const std::function<void(const Expression&)>&) const override {}
    
    bool operator==(const Expression& e) const override {
        if (e.getKind() == Kind::Literal) {
            auto rhs = static_cast<const Literal*>(&e);
            return value == rhs->value;
        }
        return false;
    }

    std::vector<optional<Value>> possibleOutputs() const override {
        return {{ value }};
    }
    
    Value getValue() const {
        return value;
    }

    mbgl::Value serialize() const override;
    std::string getOperator() const override { return "literal"; }

private:
    Value value;
};

} // namespace expression
} // namespace style
} // namespace mbgl
