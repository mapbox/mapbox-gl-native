#pragma once

#include <mbgl/util/feature.hpp>

namespace mbgl {
namespace style {

/**
 * @brief Generic representation of a style property.
 */
class StyleProperty {
public:
    enum class Kind : uint8_t { Undefined, Constant, Expression, Transition };
    StyleProperty(Value value_, Kind kind_) : value(std::move(value_)), kind(kind_) {}
    StyleProperty() = default;
    const Value& getValue() const { return value; }
    Value& getValue() { return value; }
    Kind getKind() const { return kind; }

private:
    Value value;
    Kind kind = Kind::Undefined;
};

} // namespace style
} // namespace mbgl
