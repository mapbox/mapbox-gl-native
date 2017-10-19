#pragma once

#include <vector>
#include <memory>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/type.hpp>
#include <mbgl/style/expression/value.hpp>
#include <mbgl/style/expression/parsing_context.hpp>
#include <mbgl/style/conversion.hpp>


namespace mbgl {
namespace style {
namespace expression {

class Literal : public Expression {
public:
    Literal(Value value_) : Expression(typeOf(value_)), value(value_) {}
    Literal(type::Array type_, std::vector<Value> value_) : Expression(type_), value(value_) {}
    EvaluationResult evaluate(const EvaluationParameters&) const override {
        return value;
    }
    
    static ParseResult parse(const mbgl::style::conversion::Convertible&, ParsingContext);

    void eachChild(std::function<void(const Expression*)>) const override {}
    
private:
    Value value;
};

} // namespace expression
} // namespace style
} // namespace mbgl
