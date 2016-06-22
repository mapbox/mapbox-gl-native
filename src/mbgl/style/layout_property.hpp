#pragma once

#include <mbgl/style/property_value.hpp>
#include <mbgl/style/property_parsing.hpp>
#include <mbgl/style/property_evaluator.hpp>

#include <utility>

namespace mbgl {
namespace style {

template <typename T>
class LayoutProperty {
public:
    explicit LayoutProperty(T v)
        : value(std::move(v)),
          defaultValue(value) {}

    const PropertyValue<T>& get() const {
        return currentValue;
    }

    void set(const PropertyValue<T>& value_) {
        currentValue = value_;
    }

    void calculate(const CalculationParameters& parameters) {
        if (currentValue) {
            PropertyEvaluator<T> evaluator(parameters, defaultValue);
            value = PropertyValue<T>::visit(currentValue, evaluator);
        }
    }

    // TODO: remove / privatize
    operator T() const { return value; }
    T value;

private:
    T defaultValue;
    PropertyValue<T> currentValue;
};

} // namespace style
} // namespace mbgl
