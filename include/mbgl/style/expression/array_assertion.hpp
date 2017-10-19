#pragma once

#include <vector>
#include <memory>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/style/expression/check_subtype.hpp>
#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/type.hpp>
#include <mbgl/style/expression/parsing_context.hpp>
#include <mbgl/style/conversion.hpp>

namespace mbgl {
namespace style {
namespace expression {

class ArrayAssertion : public Expression  {
public:
    ArrayAssertion(type::Array type_, std::unique_ptr<Expression> input_) :
        Expression(type_),
        input(std::move(input_))
    {}

    static ParseResult parse(const mbgl::style::conversion::Convertible& value, ParsingContext ctx);

    EvaluationResult evaluate(const EvaluationParameters& params) const override;
    void eachChild(std::function<void(const Expression*)> visit) const override;

private:
    std::unique_ptr<Expression> input;
};

} // namespace expression
} // namespace style
} // namespace mbgl
