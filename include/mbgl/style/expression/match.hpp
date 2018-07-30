#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/parsing_context.hpp>
#include <mbgl/style/conversion.hpp>

#include <memory>

namespace mbgl {
namespace style {
namespace expression {

template <typename T>
class Match : public Expression {
public:
    using Branches = std::unordered_map<T, std::shared_ptr<Expression>>;

    Match(type::Type type_,
          std::unique_ptr<Expression> input_,
          Branches branches_,
          std::unique_ptr<Expression> otherwise_
    ) : Expression(Kind::Match, type_),
        input(std::move(input_)),
        branches(std::move(branches_)),
        otherwise(std::move(otherwise_))
    {}

    EvaluationResult evaluate(const EvaluationContext& params) const override;

    void eachChild(const std::function<void(const Expression&)>& visit) const override;

    bool operator==(const Expression& e) const override;

    std::vector<optional<Value>> possibleOutputs() const override;
    
    mbgl::Value serialize() const override;
    std::string getOperator() const override { return "match"; }
private:
    std::unique_ptr<Expression> input;
    Branches branches;
    std::unique_ptr<Expression> otherwise;
};

ParseResult parseMatch(const mbgl::style::conversion::Convertible& value, ParsingContext& ctx);

} // namespace expression
} // namespace style
} // namespace mbgl
