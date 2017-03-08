#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/sources/geojson_source.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <class V>
optional<GeoJSON> convertGeoJSON(const V& value, Error& error);

} // namespace conversion
} // namespace style
} // namespace mbgl
