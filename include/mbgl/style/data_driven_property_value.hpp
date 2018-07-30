#pragma once

#include <mbgl/util/variant.hpp>
#include <mbgl/style/undefined.hpp>
#include <mbgl/style/property_expression.hpp>

namespace mbgl {
namespace style {

template <class T>
class DataDrivenPropertyValue {
private:
    using Value = variant<
        Undefined,
        T,
        PropertyExpression<T>>;

    Value value;

    friend bool operator==(const DataDrivenPropertyValue& lhs,
                           const DataDrivenPropertyValue& rhs) {
        return lhs.value == rhs.value;
    }

    friend bool operator!=(const DataDrivenPropertyValue& lhs,
                           const DataDrivenPropertyValue& rhs) {
        return !(lhs == rhs);
    }

public:
    DataDrivenPropertyValue() = default;
    DataDrivenPropertyValue(                   T  v) : value(std::move(v)) {}
    DataDrivenPropertyValue(PropertyExpression<T> v) : value(std::move(v)) {}

    bool isUndefined() const {
        return value.template is<Undefined>();
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

    const                    T & asConstant()   const { return value.template get<                   T >(); }
    const PropertyExpression<T>& asExpression() const { return value.template get<PropertyExpression<T>>(); }

    template <class... Ts>
    auto match(Ts&&... ts) const {
        return value.match(std::forward<Ts>(ts)...);
    }

    template <typename Evaluator>
    auto evaluate(const Evaluator& evaluator, TimePoint = {}) const {
        return Value::visit(value, evaluator);
    }

    bool hasDataDrivenPropertyDifference(const DataDrivenPropertyValue<T>& other) const {
        return *this != other && (isDataDriven() || other.isDataDriven());
    }
};

} // namespace style
} // namespace mbgl
