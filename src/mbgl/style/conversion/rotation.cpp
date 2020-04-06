#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/rotation.hpp>
#include <mbgl/style/conversion_impl.hpp>

namespace mbgl {
namespace style {
namespace conversion {

optional<style::Rotation> Converter<style::Rotation>::operator()(const Convertible& value, Error& error) const {
    optional<double> converted = toDouble(value);
    if (!converted) {
        error.message = "value must be a number";
        return nullopt;
    }
    return {*converted};
}

} // namespace conversion
} // namespace style
} // namespace mbgl
