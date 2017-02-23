#include <mbgl/style/conversion/geojson.hpp>

#include <mbgl/util/rapidjson.hpp>

#include <string>
#include <sstream>

namespace mbgl {
namespace style {
namespace conversion {

template <>
optional<GeoJSON> convertGeoJSON(const std::string& string, Error& error) {
    rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> d;
    d.Parse(string.c_str());

    if (d.HasParseError()) {
        std::stringstream message;
        message << d.GetErrorOffset() << " - " << rapidjson::GetParseError_En(d.GetParseError());
        error = { message.str() };
        return {};
    }

    optional<GeoJSON> geoJSON = conversion::convertGeoJSON<JSValue>(d, error);
    if (!geoJSON) {
        error = { error.message };
        return {};
    }

    return geoJSON;
}

} // namespace conversion
} // namespace style
} // namespace mbgl
