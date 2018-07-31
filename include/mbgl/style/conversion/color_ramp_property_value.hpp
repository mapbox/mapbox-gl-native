#pragma once

#include <mbgl/style/color_ramp_property_value.hpp>
#include <mbgl/style/conversion.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<ColorRampPropertyValue> {
    optional<ColorRampPropertyValue> operator()(const Convertible& value, Error& error, bool /* allowDataExpressions */ = false, bool /* convertTokens */ = false) const;
};

} // namespace conversion
} // namespace style
} // namespace mbgl
