#pragma once

#include <mbgl/util/variant.hpp>
#include <mbgl/style/function.hpp>

namespace mbgl {
namespace style {

class Undefined {};

inline bool operator==(const Undefined&, const Undefined&) { return true; }
inline bool operator!=(const Undefined&, const Undefined&) { return false; }

template <class T>
class PropertyValue {
private:
    using Value = variant<Undefined, T, Function<T>>;
    Value value;

    friend bool operator==(const PropertyValue& lhs, const PropertyValue& rhs) {
        return lhs.value == rhs.value;
    }

    friend bool operator!=(const PropertyValue& lhs, const PropertyValue& rhs) {
        return !(lhs == rhs);
    }

public:
    PropertyValue()                     : value()         {}
    PropertyValue(         T  constant) : value(constant) {}
    PropertyValue(Function<T> function) : value(function) {}

    bool isUndefined() const { return value.which() == 0; }
    bool isConstant()  const { return value.which() == 1; }
    bool isFunction()  const { return value.which() == 2; }

    const          T & asConstant() const { return value.template get<         T >(); }
    const Function<T>& asFunction() const { return value.template get<Function<T>>(); }

    explicit operator bool() const { return !isUndefined(); };

    template <typename Evaluator>
    auto evaluate(const Evaluator& evaluator) const {
        return Value::visit(value, evaluator);
    }
};

} // namespace style
} // namespace mbgl
