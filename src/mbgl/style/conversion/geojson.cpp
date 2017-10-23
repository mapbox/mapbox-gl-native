#include <mbgl/style/conversion/geojson.hpp>
#include <mbgl/style/conversion/json.hpp>

namespace mbgl {
namespace style {
namespace conversion {

optional<GeoJSON> Converter<GeoJSON>::operator()(const Convertible& value, Error& error) const {
    return toGeoJSON(value, error);
}

optional<GeoJSON> parseGeoJSON(const std::string& value, Error& error) {
    return convertJSON<GeoJSON>(value, error);
}

} // namespace conversion
} // namespace style
} // namespace mbgl
