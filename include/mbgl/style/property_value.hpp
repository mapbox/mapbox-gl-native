#pragma once

#include <mbgl/util/variant.hpp>
#include <mbgl/style/undefined.hpp>
#include <mbgl/style/function/camera_function.hpp>

namespace mbgl {
namespace style {

template <class T>
class PropertyValue {
private:
    using Value = variant<Undefined, T, CameraFunction<T>>;
    Value value;

    friend bool operator==(const PropertyValue& lhs, const PropertyValue& rhs) {
        return lhs.value == rhs.value;
    }

    friend bool operator!=(const PropertyValue& lhs, const PropertyValue& rhs) {
        return !(lhs == rhs);
    }

public:
    PropertyValue()                           : value()         {}
    PropertyValue(               T  constant) : value(constant) {}
    PropertyValue(CameraFunction<T> function) : value(function) {}

    bool isUndefined()      const { return value.which() == 0; }
    bool isConstant()       const { return value.which() == 1; }
    bool isCameraFunction() const { return value.which() == 2; }
    bool isDataDriven()     const { return false; }

    const                T & asConstant()       const { return value.template get<               T >(); }
    const CameraFunction<T>& asCameraFunction() const { return value.template get<CameraFunction<T>>(); }

    template <typename Evaluator>
    auto evaluate(const Evaluator& evaluator, TimePoint = {}) const {
        return Value::visit(value, evaluator);
    }

    bool hasDataDrivenPropertyDifference(const PropertyValue<T>&) const {
        return false;
    }
};

} // namespace style
} // namespace mbgl
