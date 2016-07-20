#include <mbgl/style/conversion/geojson.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
Result<GeoJSON> convertGeoJSON(const v8::Local<v8::Value>&) {
    return Error { "not implemented" };
}

} // namespace conversion
} // namespace style
} // namespace mbgl
