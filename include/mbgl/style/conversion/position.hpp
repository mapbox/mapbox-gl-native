#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/position.hpp>
#include <mbgl/util/optional.hpp>

#include <array>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<Position> {
    optional<Position> operator()(const Value& value, Error& error) const {
        optional<std::array<float, 3>> spherical = convert<std::array<float, 3>>(value, error);

        if (!spherical) {
            return {};
        }

        return Position(*spherical);
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
