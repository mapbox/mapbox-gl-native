#include <mbgl/style/conversion/geojson.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
optional<GeoJSON> Converter<GeoJSON>::operator()(const v8::Local<v8::Value>&, Error& error) const {
    error = { "not implemented" };
    return {};
}

} // namespace conversion
} // namespace style
} // namespace mbgl
