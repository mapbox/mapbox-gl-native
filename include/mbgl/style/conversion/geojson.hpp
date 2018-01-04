#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/util/geojson.hpp>

namespace mbgl {
namespace style {
namespace conversion {

// Workaround until https://github.com/mapbox/mapbox-gl-native/issues/5623 is done.
optional<GeoJSON> parseGeoJSON(const std::string&, Error&);

template <>
struct Converter<GeoJSON> {
public:
    optional<GeoJSON> operator()(const Convertible&, Error&) const;
};

} // namespace conversion
} // namespace style
} // namespace mbgl
