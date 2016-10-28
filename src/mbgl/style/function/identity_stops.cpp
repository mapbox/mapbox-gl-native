#include <mbgl/style/function/identity_stops.hpp>
#include <mbgl/util/color.hpp>

#include <array>

namespace mbgl {
namespace style {

template <>
float IdentityStops<float>::evaluate(const Value& value) const {
    return numericValue<float>(value)
        .value_or(0.0f);
}

template <>
Color IdentityStops<Color>::evaluate(const Value& value) const {
    if (!value.is<std::string>()) {
        return Color::black();
    }

    return Color::parse(value.get<std::string>())
        .value_or(Color::black());
}

template <>
std::array<float, 2> IdentityStops<std::array<float, 2>>::evaluate(const Value& value) const {
    if (!value.is<std::vector<Value>>()) {
        return {{ 0, 0 }};
    }

    const std::vector<Value>& vector = value.get<std::vector<Value>>();
    if (vector.size() != 2 || !numericValue<float>(vector[0]) || !numericValue<float>(vector[1])) {
        return {{ 0, 0 }};
    }

    return {{
        *numericValue<float>(vector[0]),
        *numericValue<float>(vector[1])
    }};
}

} // namespace style
} // namespace mbgl
