#pragma once

#include <map>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/parsing_context.hpp>
#include <mbgl/style/conversion.hpp>

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

    static ParseResult parse(const mbgl::style::conversion::Convertible& value, ParsingContext ctx);


    EvaluationResult evaluate(const EvaluationParameters& params) const override;
    
    void eachChild(std::function<void(const Expression*)> visit) const override;

    std::size_t getLength() const {
        return args.size();
    }
    
    Expression* getChild(std::size_t i) const {
        return args.at(i).get();
    }
    
private:
    Args args;
};

} // namespace expression
} // namespace style
} // namespace mbgl
