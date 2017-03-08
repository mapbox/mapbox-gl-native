#include <mbgl/style/conversion/geojson.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
optional<GeoJSON> convertGeoJSON(const v8::Local<v8::Value>&, Error& error) {
    error = { "not implemented" };
    return {};
}

} // namespace conversion
} // namespace style
} // namespace mbgl
