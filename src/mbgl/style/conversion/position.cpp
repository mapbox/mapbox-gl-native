#include <mbgl/style/conversion/position.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion_impl.hpp>

#include <array>

namespace mbgl {
namespace style {
namespace conversion {

optional<Position> Converter<Position>::operator()(const Convertible& value, Error& error) const {
    optional<std::array<float, 3>> spherical = convert<std::array<float, 3>>(value, error);

    if (!spherical) {
        return nullopt;
    }

    return Position(*spherical);
}

} // namespace conversion
} // namespace style
} // namespace mbgl
