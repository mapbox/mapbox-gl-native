#pragma once

#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>
#include <mbgl/style/expression/literal.hpp>
#include <mbgl/style/expression/format_expression.hpp>
#include <mbgl/style/expression/formatted.hpp>
#include <mbgl/style/expression/format_section_override.hpp>
#include <mbgl/style/expression/value.hpp>

namespace mbgl {
namespace style {

template<typename PaintProperty>
struct FormatSectionOverrides;

template<typename... PaintProperty>
struct FormatSectionOverrides<TypeList<PaintProperty...>> {
    template<typename Property, typename T, typename U>
    static void setOverride(const T& overrides, U& overridable) {
        if (hasOverride<Property>(overrides.template get<TextField>())) {
            auto override =
                    std::make_unique<expression::FormatSectionOverride<typename Property::Type>>(Property::expressionType(),
                                                                                                 std::move(overridable.template get<Property>()),
                                                                                                 Property::name());
            PropertyExpression<typename Property::Type> expr(std::move(override));
            overridable.template get<Property>() = PossiblyEvaluatedPropertyValue<typename Property::Type>(std::move(expr));
        }
    }

    template<typename T, typename U>
    static void setOverrides(const T& overrides, U& overridable) {
        util::ignore({(setOverride<PaintProperty>(overrides, overridable), 0)...});
    }

    template<typename Property, typename T, typename U>
    static void updateOverride(T& evaluated, U& updated) {
        auto property = evaluated.template get<Property>();
        if (!property.isConstant()) {
            const bool hasFormatSectionOverride = property.match(
                    [] (const style::PropertyExpression<typename Property::Type>& e) {
                        return e.getExpression().getKind() == expression::Kind::FormatSectionOverride;
                    },
                    [] (const auto&) {
                        return false;
                    });
            if (hasFormatSectionOverride) {
                updated.template get<Property>() = std::move(property);
            }
        }
    }

    template<typename T, typename U>
    static void updateOverrides(T& evaluated, U& updated) {
        util::ignore({(updateOverride<PaintProperty>(evaluated, updated), 0)...});
    }

    template<typename Property, typename FormattedProperty>
    static bool hasOverride(const FormattedProperty& formatted) {

        const auto checkLiteral = [] (const TextField::Type& literal) {
            for (const auto& section : literal.sections) {
                 if (Property::hasOverride(section)) {
                     return true;
                 }
            }
            return false;
        };

        return formatted.match(
                [&checkLiteral] (const TextField::Type& literal) {
                    return checkLiteral(literal);
                },
                [&checkLiteral] (const PropertyExpression<TextField::Type>& property) {
                    bool expressionHasOverrides = false;
                    const std::function<void(const expression::Expression&)> checkExpression = [&](const expression::Expression& e) {
                        if (expressionHasOverrides) {
                            return;
                        }

                        if (e.getKind() == expression::Kind::Literal &&
                            e.getType() == expression::type::Formatted) {
                            const auto* literalExpr = static_cast<const expression::Literal*>(&e);
                            const auto formattedValue = expression::fromExpressionValue<expression::Formatted>(literalExpr->getValue());
                            if (formattedValue && checkLiteral(*formattedValue)) {
                                expressionHasOverrides = true;
                            }
                            return;
                        } else if (e.getKind() == expression::Kind::FormatExpression) {
                            const auto* formatExpr = static_cast<const expression::FormatExpression*>(&e);
                            for (const auto& section : formatExpr->getSections()) {
                                if (Property::hasOverride(section)) {
                                    expressionHasOverrides = true;
                                    break;
                                }
                            }
                        } else {
                            e.eachChild(checkExpression);
                        }
                    };

                    checkExpression(property.getExpression());
                    return expressionHasOverrides;
                },
                [] (const auto&) {
                    return false;
                }
         );
    }

    template <typename FormattedProperty>
    static bool hasOverrides(const FormattedProperty& formatted) {
        bool result = false;
        util::ignore({ (result |= hasOverride<PaintProperty>(formatted))... });
        return result;
    }

    template <typename PaintProperties>
    static bool hasPaintPropertyDifference(const PaintProperties& lhs, const PaintProperties& rhs) {
        bool result = false;
        util::ignore({ (result |= lhs.template get<PaintProperty>().value.isConstant() &&
                                  rhs.template get<PaintProperty>().value.isConstant() &&
                                  (lhs.template get<PaintProperty>().value.asConstant() != rhs.template get<PaintProperty>().value.asConstant()))... });
        return result;
    }
};

using SymbolLayerPaintPropertyOverrides = FormatSectionOverrides<SymbolPaintProperties::OverridableProperties>;

class SymbolLayer::Impl : public Layer::Impl {
public:
    using Layer::Impl::Impl;

    bool hasLayoutDifference(const Layer::Impl&) const override;
    void stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const override;
    void populateFontStack(std::set<FontStack>& fontStack) const final;

    SymbolLayoutProperties::Unevaluated layout;
    SymbolPaintProperties::Transitionable paint;

    DECLARE_LAYER_TYPE_INFO;

private:
    bool hasFormatSectionOverrides() const;
    mutable optional<bool> hasFormatSectionOverrides_;
};

} // namespace style
} // namespace mbgl
