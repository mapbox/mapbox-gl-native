#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/formatted.hpp>
#include <mbgl/style/expression/parsing_context.hpp>
#include <mbgl/style/conversion.hpp>

#include <memory>

namespace mbgl {
namespace style {
namespace expression {

struct FormatExpressionSection {
    FormatExpressionSection(std::unique_ptr<Expression> text_,
                            optional<std::unique_ptr<Expression>> fontScale_,
                            optional<std::unique_ptr<Expression>> textFont_);
    
    std::shared_ptr<Expression> text;
    optional<std::shared_ptr<Expression>> fontScale;
    optional<std::shared_ptr<Expression>> textFont;
};
    
class FormatExpression : public Expression {
public:
    FormatExpression(std::vector<FormatExpressionSection> sections);
    
    EvaluationResult evaluate(const EvaluationContext&) const override;
    static ParseResult parse(const mbgl::style::conversion::Convertible&, ParsingContext&);
    
    void eachChild(const std::function<void(const Expression&)>&) const override;
    
    bool operator==(const Expression& e) const override;
    
    std::vector<optional<Value>> possibleOutputs() const override {
        // Technically the combinatoric set of all children
        // Usually, this.text will be undefined anyway
        return { nullopt };
    }
    
    mbgl::Value serialize() const override;
    std::string getOperator() const override { return "format"; }
private:
    std::vector<FormatExpressionSection> sections;
    std::unique_ptr<Expression> text;
    optional<std::unique_ptr<Expression>> fontScale;
    optional<std::unique_ptr<Expression>> textFont;
};
    
} // namespace expression
} // namespace style
} // namespace mbgl
