#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/util/geo.hpp>

namespace mbgl {
namespace style {
namespace conversion {
            
template<>
struct Converter<LatLng> {
public:
    optional<LatLng> operator() (const Value& value, Error& error) const {
        if (!isArray(value) || arrayLength(value) < 2 ) {
            error = { "coordinate array must contain numeric longitude and latitude values" };
            return {};
        }
        //Style spec uses GeoJSON convention for specifying coordinates
        optional<double> latitude = toDouble(arrayMember(value, 1));
        optional<double> longitude = toDouble(arrayMember(value, 0));
    
        if (!latitude || !longitude) {
            error = { "coordinate array must contain numeric longitude and latitude values" };
            return {};
        }
        if (*latitude < -90 || *latitude > 90 ){
            error = { "coordinate latitude must be between -90 and 90" };
            return {};
        }
        return LatLng(*latitude, *longitude);
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
