#pragma once
#include <mbgl/style/expression/expression.hpp>

namespace mbgl {
namespace style {
namespace expression {

class Any : public Expression  {
public:
    Any(type::Array type_, std::vector<std::unique_ptr<Expression>> inputs_) :
        Expression(type_),
        inputs(std::move(inputs_))
    {}

    static ParseResult parse(const mbgl::style::conversion::Convertible& value, ParsingContext ctx);

    EvaluationResult evaluate(const EvaluationParameters& params) const override;
    void accept(std::function<void(const Expression*)> visit) const override;

private:
    std::unique_ptr<Expression> inputs;
};

class All : public Expression  {
public:
    All(type::Array type_, std::vector<std::unique_ptr<Expression>> inputs_) :
        Expression(type_),
        inputs(std::move(inputs_))
    {}

    static ParseResult parse(const mbgl::style::conversion::Convertible& value, ParsingContext ctx);

    EvaluationResult evaluate(const EvaluationParameters& params) const override;
    void accept(std::function<void(const Expression*)> visit) const override;

private:
    std::unique_ptr<Expression> inputs;
};

} // namespace expression
} // namespace style
} // namespace mbgl

