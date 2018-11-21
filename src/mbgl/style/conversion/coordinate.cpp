#include <mbgl/style/conversion/coordinate.hpp>
#include <mbgl/style/conversion_impl.hpp>

namespace mbgl {
namespace style {
namespace conversion {

optional<LatLng> Converter<LatLng>::operator() (const Convertible& value, Error& error) const {
    if (!isArray(value) || arrayLength(value) < 2 ) {
        error.message = "coordinate array must contain numeric longitude and latitude values";
        return nullopt;
    }
    //Style spec uses GeoJSON convention for specifying coordinates
    optional<double> latitude = toDouble(arrayMember(value, 1));
    optional<double> longitude = toDouble(arrayMember(value, 0));

    if (!latitude || !longitude) {
        error.message = "coordinate array must contain numeric longitude and latitude values";
        return nullopt;
    }
    if (*latitude < -90 || *latitude > 90 ){
        error.message = "coordinate latitude must be between -90 and 90";
        return nullopt;
    }
    return LatLng(*latitude, *longitude);
}

} // namespace conversion
} // namespace style
} // namespace mbgl
