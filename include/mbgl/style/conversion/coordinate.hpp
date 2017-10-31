#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/util/geo.hpp>

namespace mbgl {
namespace style {
namespace conversion {
            
template<>
struct Converter<LatLng> {
public:
    optional<LatLng> operator() (const Convertible& value, Error& error) const;
};

} // namespace conversion
} // namespace style
} // namespace mbgl
