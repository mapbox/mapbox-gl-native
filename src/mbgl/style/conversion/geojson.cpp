#include <mbgl/style/conversion/geojson.hpp>

#include <mbgl/util/rapidjson.hpp>

#include <string>
#include <sstream>

namespace mbgl {
namespace style {
namespace conversion {

template <>
Result<GeoJSON> convertGeoJSON(const std::string& string) {
    rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> d;
    d.Parse(string.c_str());

    if (d.HasParseError()) {
        std::stringstream message;
        message << d.GetErrorOffset() << " - " << rapidjson::GetParseError_En(d.GetParseError());

        return Error { message.str() };
    }

    Result<GeoJSON> geoJSON = convertGeoJSON<JSValue>(d);
    if (!geoJSON) {
        return Error { geoJSON.error().message };
    }

    return geoJSON;
}

} // namespace conversion
} // namespace style
} // namespace mbgl
