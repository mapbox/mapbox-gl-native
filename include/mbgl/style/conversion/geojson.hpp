#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/sources/geojson_source.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <class V>
Result<GeoJSON> convertGeoJSON(const V& value, const GeoJSONOptions& options);

} // namespace conversion
} // namespace style
} // namespace mbgl
