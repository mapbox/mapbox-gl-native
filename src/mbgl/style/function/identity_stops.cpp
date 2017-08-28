#include <mbgl/style/function/identity_stops.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/util/enum.hpp>
#include <mbgl/util/color.hpp>

#include <array>

namespace mbgl {
namespace style {

template <>
optional<float> IdentityStops<float>::evaluate(const Value& value) const {
    return numericValue<float>(value);
}

template <>
optional<std::string> IdentityStops<std::string>::evaluate(const Value& value) const {
    if (!value.is<std::string>()) {
        return {};
    }
    
    return value.get<std::string>();
}

template <>
optional<Color> IdentityStops<Color>::evaluate(const Value& value) const {
    if (!value.is<std::string>()) {
        return {};
    }

    return Color::parse(value.get<std::string>());
}

template <>
optional<TextTransformType> IdentityStops<TextTransformType>::evaluate(const Value& value) const {
    if (!value.is<std::string>()) {
        return {};
    }

    return Enum<TextTransformType>::toEnum(value.get<std::string>());
}

template <>
optional<TextJustifyType> IdentityStops<TextJustifyType>::evaluate(const Value& value) const {
    if (!value.is<std::string>()) {
        return {};
    }

    return Enum<TextJustifyType>::toEnum(value.get<std::string>());
}

template <>
optional<SymbolAnchorType> IdentityStops<SymbolAnchorType>::evaluate(const Value& value) const {
    if (!value.is<std::string>()) {
        return {};
    }

    return Enum<SymbolAnchorType>::toEnum(value.get<std::string>());
}

template <>
optional<LineJoinType> IdentityStops<LineJoinType>::evaluate(const Value& value) const {
    if (!value.is<std::string>()) {
        return {};
    }

    return Enum<LineJoinType>::toEnum(value.get<std::string>());
}

template <>
optional<std::array<float, 2>> IdentityStops<std::array<float, 2>>::evaluate(const Value& value) const {
    if (!value.is<std::vector<Value>>()) {
        return {};
    }

    const auto& vector = value.get<std::vector<Value>>();
    if (vector.size() != 2 || !numericValue<float>(vector[0]) || !numericValue<float>(vector[1])) {
        return {};
    }

    std::array<float, 2> array {{
        *numericValue<float>(vector[0]),
        *numericValue<float>(vector[1])
    }};
    return array;
}

} // namespace style
} // namespace mbgl
