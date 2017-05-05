#include <mbgl/style/conversion/geojson.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/util/rapidjson.hpp>

#include <mapbox/geojson.hpp>
#include <mapbox/geojson/rapidjson.hpp>

namespace mbgl {
namespace style {
namespace conversion {

optional<GeoJSON> Converter<GeoJSON>::operator()(const std::string& value, Error& error) const {
    return convertJSON<GeoJSON>(value, error);
}

template <>
optional<GeoJSON> Converter<GeoJSON>::operator()(const JSValue& value, Error& error) const {
    try {
        return mapbox::geojson::convert(value);
    } catch (const std::exception& ex) {
        error = { ex.what() };
        return {};
    }
}

} // namespace conversion
} // namespace style
} // namespace mbgl
