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
optional<std::array<float, 2>> IdentityStops<std::array<float, 2>>::evaluate(const Value& value) const {
    if (!value.is<std::vector<Value>>()) {
        return {};
    }

    const std::vector<Value>& vector = value.get<std::vector<Value>>();
    if (vector.size() != 2 || !numericValue<float>(vector[0]) || !numericValue<float>(vector[1])) {
        return {};
    }

    return {{{
        *numericValue<float>(vector[0]),
        *numericValue<float>(vector[1])
    }}};
}

} // namespace style
} // namespace mbgl
