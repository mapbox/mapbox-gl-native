#pragma once

#include <mbgl/util/variant.hpp>
#include <mbgl/style/undefined.hpp>
#include <mbgl/style/function/camera_function.hpp>

namespace mbgl {
namespace style {

/*
 * Special-case implementation of (a subset of) the PropertyValue<T> interface
 * used for building the HeatmapColor paint property traits class.
 */
class ColorRampPropertyValue {
private:
    std::shared_ptr<expression::Expression> value;

    friend bool operator==(const ColorRampPropertyValue& lhs, const ColorRampPropertyValue& rhs) {
        return (lhs.isUndefined() && rhs.isUndefined()) || (lhs.value && rhs.value && *(lhs.value) == *(rhs.value));
    }

    friend bool operator!=(const ColorRampPropertyValue& lhs, const ColorRampPropertyValue& rhs) {
        return !(lhs == rhs);
    }

public:
    ColorRampPropertyValue() : value(nullptr) {}
    ColorRampPropertyValue(std::shared_ptr<expression::Expression> value_) : value(std::move(value_)) {}

    bool isUndefined() const { return value.get() == nullptr; }

    // noop, needed for batch evaluation of paint property values to compile
    template <typename Evaluator>
    Color evaluate(const Evaluator&, TimePoint = {}) const { return {}; }

    Color evaluate(double rampEvaluationParameter) const {
        const auto result = value->evaluate(expression::EvaluationContext({}, nullptr, {rampEvaluationParameter}));
        return *expression::fromExpressionValue<Color>(*result);
    }

    bool isDataDriven()     const { return false; }
    bool hasDataDrivenPropertyDifference(const ColorRampPropertyValue&) const { return false; }
    
    const expression::Expression& getExpression() const { return *value; }
};


} // namespace style
} // namespace mbgl
