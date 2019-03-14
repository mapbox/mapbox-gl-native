#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/renderer/possibly_evaluated_property_value.hpp>

namespace mbgl {
namespace style {
namespace expression {

template<class T>
class FormatSectionOverride final : public Expression {
public:
    FormatSectionOverride(const type::Type& type_,
                         PossiblyEvaluatedPropertyValue<T> defaultValue_,
                         std::string propertyName_) :
        Expression(Kind::FormatSectionOverride, type_),
        defaultValue(std::move(defaultValue_)),
        propertyName(std::move(propertyName_))
    {}

    EvaluationResult evaluate(const EvaluationContext& context) const final {
        using Object = std::unordered_map<std::string, expression::Value>;
        if (context.formattedSection &&
            context.formattedSection->is<Object>()) {
            const auto& section = context.formattedSection->get<Object>();
            if (section.find(propertyName) != section.end()) {
                return section.at(propertyName);
            }
        }

        return defaultValue.match(
                [&context] (const style::PropertyExpression<T>& e) { return e.getExpression().evaluate(context); },
                [] (const T& t) -> EvaluationResult { return t; }
        );
    }

    void eachChild(const std::function<void(const Expression&)>& fn) const final {
        defaultValue.match([&fn] (const style::PropertyExpression<T>& e) { fn(e.getExpression()); },
                           [] (const T&) {});
    }

    bool operator==(const Expression& e) const final {
        if (e.getKind() == Kind::FormatSectionOverride) {
            const auto* other = static_cast<const FormatSectionOverride*>(&e);

            if (getType() != other->getType() || propertyName != other->propertyName) {
                return false;
            }

            // Check that default values or property expressions are equal.
            return defaultValue.match(
                    [other] (const style::PropertyExpression<T>& thisExpr) {
                        return other->defaultValue.match([&thisExpr] (const style::PropertyExpression<T>& otherExpr) {
                                                             return thisExpr == otherExpr;
                                                         },
                                                         [] (const T&) {
                                                             return false;
                                                         });
                    },
                    [other] (const T& thisValue) {
                        return other->defaultValue.match([&thisValue] (const T& otherValue) {
                                                            return thisValue == otherValue;
                                                         },
                                                         [] (const style::PropertyExpression<T>&) {
                                                            return false;
                                                        });
                    });
        }

        return false;
    }

    std::vector<optional<Value>> possibleOutputs() const final {
        return {nullopt};
    }

    std::string getOperator() const final { return "format-section-override"; }

private:
    PossiblyEvaluatedPropertyValue<T> defaultValue;
    std::string propertyName;
};

} // namespace expression
} // namespace style
} // namespace mbgl
