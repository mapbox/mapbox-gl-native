#pragma once

#include "../value.hpp"

#include <mapbox/geojson.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/geojson.hpp>
#include <mbgl/util/rapidjson.hpp>
#include <mbgl/platform/log.hpp>
#include <jni/jni.hpp>

#include <sstream>
#include <string>

namespace mbgl {
namespace style {
namespace conversion {

template <>
Result<GeoJSON> convertGeoJSON(const mbgl::android::Value& value) {

    //Value should be a string wrapped in an object
    mbgl::android::Value jsonValue = value.get("data");
    if(value.isNull()) {
        return Error { "no json data found" };
    }
    std::string jsonString = value.get("data").toString();

    rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> d;
    d.Parse(jsonString.c_str());

    if (d.HasParseError()) {
        std::stringstream message;
        message << d.GetErrorOffset() << " - " << rapidjson::GetParseError_En(d.GetParseError());
        return Error { message.str() };
    }

    conversion::Result<GeoJSON> geoJSON = conversion::convertGeoJSON<JSValue>(d);
    if (!geoJSON) {
        return Error { geoJSON.error().message };
    }

    return geoJSON;
}

template <>
struct Converter<GeoJSON> {

    Result<GeoJSON> operator()(const mbgl::android::Value& value) const {
        return convertGeoJSON(value);
    }

};

} // namespace conversion
} // namespace style
} // namespace mbgl