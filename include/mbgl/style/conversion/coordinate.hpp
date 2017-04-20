#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/util/geo.hpp>

namespace mbgl {
namespace style {
namespace conversion {
            
template<>
struct Converter<std::unique_ptr<LatLng>> {
public:
    template <class V>
    optional<std::unique_ptr<LatLng>> operator() (const V& value, Error& error) const {
        if (!isArray(value) || arrayLength(value) < 2 ) {
            error = { "coordinate array must contain numeric longtitude and latitude values" };
            return {};
        }
        //Style spec uses GeoJSON convention for specifying coordinates
        optional<float> latitude = toNumber(arrayMember(value, 1));
        optional<float> longitude = toNumber(arrayMember(value, 0));
    
        if (!latitude || !longitude) {
            error = { "coordinate array must contain numeric longtitude and latitude values" };
            return {};
        }
        if (*latitude < -90 || *latitude > 90 ){
            error = { "coordinate latitude must be between -90 and 90" };
            return {};
        }
        return { std::make_unique<LatLng>(*latitude, *longitude) };
    }
};

} // namespace conversion
} // namespace style
} // namespace mbgl
