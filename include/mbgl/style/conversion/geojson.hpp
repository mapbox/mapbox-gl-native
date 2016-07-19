#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/sources/geojson_source.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <class V>
Result<mapbox::geojson::geojson> convertGeoJSON(const V& value);

} // namespace conversion
} // namespace style
} // namespace mbgl
