
#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/parsing_context.hpp>
#include <mbgl/style/conversion.hpp>

#include <mbgl/util/range.hpp>

#include <memory>
#include <map>


namespace mbgl {
namespace style {
namespace expression {

class Step : public Expression {
public:
    Step(const type::Type& type_,
          std::unique_ptr<Expression> input_,
          std::map<double, std::unique_ptr<Expression>> stops_
    ) : Expression(type_),
        input(std::move(input_)),
        stops(std::move(stops_))
    {}

    EvaluationResult evaluate(const EvaluationContext& params) const override;
    void eachChild(const std::function<void(const Expression&)>& visit) const override;

    const std::unique_ptr<Expression>& getInput() const { return input; }
    Range<float> getCoveringStops(const double lower, const double upper) const;

    bool operator==(const Expression& e) const override;

    static ParseResult parse(const mbgl::style::conversion::Convertible& value, ParsingContext& ctx);

private:
    const std::unique_ptr<Expression> input;
    const std::map<double, std::unique_ptr<Expression>> stops;
};

} // namespace expression
} // namespace style
} // namespace mbgl
