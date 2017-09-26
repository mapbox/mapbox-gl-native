#include <mbgl/style/conversion/geojson.hpp>

namespace mbgl {
namespace style {
namespace conversion {

optional<GeoJSON> Converter<GeoJSON>::operator()(const Value& value, Error& error) const {
    return toGeoJSON(value, error);
}

} // namespace conversion
} // namespace style
} // namespace mbgl
