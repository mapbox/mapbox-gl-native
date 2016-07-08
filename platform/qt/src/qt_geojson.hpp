#include <mbgl/style/conversion/geojson.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
Result<GeoJSON> convertGeoJSON(const QVariant&) {
    return Error { "not implemented" };
}

} // namespace conversion
} // namespace style
} // namespace mbgl
