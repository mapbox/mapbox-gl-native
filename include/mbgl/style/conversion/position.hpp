#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/position.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<Position> {
    optional<Position> operator()(const Convertible& value, Error& error) const;
};

} // namespace conversion
} // namespace style
} // namespace mbgl
