#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/expression/parsing_context.hpp>

#include <memory>
#include <map>

namespace mbgl {
namespace style {
namespace expression {

class Coalesce : public Expression {
public:
    using Args = std::vector<std::unique_ptr<Expression>>;
    Coalesce(const type::Type& type_, Args args_) :
        Expression(type_),
        args(std::move(args_))
    {}

    static ParseResult parse(const mbgl::style::conversion::Convertible& value, ParsingContext& ctx);


    EvaluationResult evaluate(const EvaluationContext& params) const override;
    
    void eachChild(const std::function<void(const Expression&)>& visit) const override;

    bool operator==(const Expression& e) const override;

    std::vector<optional<Value>> possibleOutputs() const override;

    std::size_t getLength() const {
        return args.size();
    }
    
    Expression* getChild(std::size_t i) const {
        return args.at(i).get();
    }
    
    std::string getOperator() const override { return "coalesce"; }
private:
    Args args;
};

} // namespace expression
} // namespace style
} // namespace mbgl
