#pragma once

#include <mbgl/util/variant.hpp>
#include <mbgl/style/types.hpp>

namespace mbgl {

class Undefined {};

template <class T>
class PropertyValue {
private:
    using Value = variant<Undefined, T, Function<T>>;
    Value value;

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

    template <typename Visitor>
    static auto visit(const PropertyValue<T>& value, Visitor&& visitor) {
        return Value::visit(value.value, visitor);
    }
};

}
