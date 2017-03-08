#pragma once

#include "../value.hpp"

#include <mapbox/geojson.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/geojson.hpp>
#include <mbgl/util/rapidjson.hpp>
#include <mbgl/util/logging.hpp>
#include <jni/jni.hpp>

#include <sstream>
#include <string>

namespace mbgl {
namespace style {
namespace conversion {

template <>
optional<GeoJSON> convertGeoJSON(const mbgl::android::Value& value, Error& error) {

    // Value should be a string wrapped in an object
    mbgl::android::Value jsonValue = value.get("data");
    if(value.isNull()) {
        error = { "no json data found" };
        return {};
    }
    std::string jsonString = value.get("data").toString();

    rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> d;
    d.Parse(jsonString.c_str());

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

template <>
struct Converter<GeoJSON> {

    optional<GeoJSON> operator()(const mbgl::android::Value& value, Error& error) const {
        return convertGeoJSON(value, error);
    }

};

} // namespace conversion
} // namespace style
} // namespace mbgl
