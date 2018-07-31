#pragma once

#include <mbgl/util/variant.hpp>
#include <mbgl/style/undefined.hpp>
#include <mbgl/style/property_expression.hpp>

namespace mbgl {
namespace style {

template <class T>
class PropertyValue {
private:
    using Value = variant<
        Undefined,
        T,
        PropertyExpression<T>>;

    Value value;

    friend bool operator==(const PropertyValue& lhs,
                           const PropertyValue& rhs) {
        return lhs.value == rhs.value;
    }

    friend bool operator!=(const PropertyValue& lhs,
                           const PropertyValue& rhs) {
        return !(lhs == rhs);
    }

public:
    PropertyValue() = default;

    PropertyValue(T constant)
        : value(std::move(constant)) {}

    PropertyValue(PropertyExpression<T> expression)
        : value(std::move(expression)) {}

    bool isUndefined() const {
        return value.template is<Undefined>();
    }

    bool isConstant() const {
        return value.template is<T>();
    }

    bool isExpression() const {
        return value.template is<PropertyExpression<T>>();
    }

    bool isDataDriven() const {
        return value.match(
            [] (const Undefined&)                { return false; },
            [] (const T&)                        { return false; },
            [] (const PropertyExpression<T>& fn) { return !fn.isFeatureConstant(); }
        );
    }

    bool isZoomConstant() const {
        return value.match(
            [] (const Undefined&)                { return true; },
            [] (const T&)                        { return true; },
            [] (const PropertyExpression<T>& fn) { return fn.isZoomConstant(); }
        );
    }

    const T& asConstant() const {
        return value.template get<T>();
    }

    const PropertyExpression<T>& asExpression() const {
        return value.template get<PropertyExpression<T>>();
    }

    template <class... Ts>
    auto match(Ts&&... ts) const {
        return value.match(std::forward<Ts>(ts)...);
    }

    template <typename Evaluator>
    auto evaluate(const Evaluator& evaluator, TimePoint = {}) const {
        return Value::visit(value, evaluator);
    }

    bool hasDataDrivenPropertyDifference(const PropertyValue<T>& other) const {
        return *this != other && (isDataDriven() || other.isDataDriven());
    }
};

} // namespace style
} // namespace mbgl
