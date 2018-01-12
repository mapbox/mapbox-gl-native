#pragma once

#include <mbgl/util/variant.hpp>
#include <mbgl/style/undefined.hpp>
#include <mbgl/style/function/camera_function.hpp>
#include <mbgl/renderer/property_evaluation_parameters.hpp>

namespace mbgl {
namespace style {

class NoopPropertyEvaluator {
public:
    using ResultType = Color;
    NoopPropertyEvaluator(const PropertyEvaluationParameters&, Color) {}
};

/*
 * Special-case implementation of (a subset of) the PropertyValue<T> interface
 * used for building the HeatmapColor paint property traits class.
 */
class HeatmapColorPropertyValue {
private:
    std::shared_ptr<expression::Expression> value;

    friend bool operator==(const HeatmapColorPropertyValue& lhs, const HeatmapColorPropertyValue& rhs) {
        return *(lhs.value) == *(rhs.value);
    }

    friend bool operator!=(const HeatmapColorPropertyValue& lhs, const HeatmapColorPropertyValue& rhs) {
        return !(lhs == rhs);
    }
    
public:
    HeatmapColorPropertyValue() : value(nullptr) {}
    HeatmapColorPropertyValue(std::shared_ptr<expression::Expression> value_) : value(std::move(value_)) {}
    
    bool isUndefined() const { return value.get() != nullptr; }

    // noop, needed for batch evaluation of paint property values to compile
    Color evaluate(const NoopPropertyEvaluator&, TimePoint = {}) const { return {}; }
    bool isDataDriven()     const { return false; }
    bool hasDataDrivenPropertyDifference(const HeatmapColorPropertyValue&) const { return false; }
};


} // namespace style
} // namespace mbgl
