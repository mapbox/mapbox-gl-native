#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/conversion.hpp>

#include <memory>
#include <vector>

namespace mbgl {
namespace style {
namespace expression {

/**
 * Special form for error-coalescing coercion expressions "to-number",
 * "to-color".  Since these coercions can fail at runtime, they accept multiple
 * arguments, only evaluating one at a time until one succeeds.
 */
class Coercion : public Expression  {
public:
    Coercion(type::Type type_, std::vector<std::unique_ptr<Expression>> inputs_);

    static ParseResult parse(const mbgl::style::conversion::Convertible& value, ParsingContext& ctx);

    EvaluationResult evaluate(const EvaluationContext& params) const override;
    void eachChild(const std::function<void(const Expression&)>& visit) const override;

    bool operator==(const Expression& e) const override;

    std::vector<optional<Value>> possibleOutputs() const override;

    std::string getOperator() const override;
private:
    EvaluationResult (*coerceSingleValue) (const Value& v);
    std::vector<std::unique_ptr<Expression>> inputs;
};

} // namespace expression
} // namespace style
} // namespace mbgl

