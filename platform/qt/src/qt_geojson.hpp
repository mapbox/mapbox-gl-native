#pragma once

#include <mapbox/geojson.hpp>
#include <mbgl/style/conversion/geojson.hpp>
#include <mbgl/util/rapidjson.hpp>

#include <sstream>
#include <string>

#include <QByteArray>
#include <QVariant>

namespace mbgl {
namespace style {
namespace conversion {

template <>
Result<GeoJSON> convertGeoJSON(const QVariant& value) {
    if (value.type() != QVariant::ByteArray) {
        return Error { "JSON data must be in QByteArray" };
    }

    auto data = value.toByteArray();

    rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> d;
    if (data.endsWith(char(0))) {
        d.Parse<0>(value.toByteArray().data());
    } else {
        d.Parse<0>(value.toByteArray().constData());
    }

    // Needed to keep the error message alive
    // when we go out of this scope.
    static std::string error;

    if (d.HasParseError()) {
        std::stringstream message;
        message << d.GetErrorOffset() << " - " << rapidjson::GetParseError_En(d.GetParseError());

        error = message.str();
        return Error { error.c_str() };
    }

    conversion::Result<GeoJSON> geoJSON = conversion::convertGeoJSON<JSValue>(d);
    if (!geoJSON) {
        error = geoJSON.error().message;
        return Error { error.c_str() };
    }

    return geoJSON;
}

} // namespace conversion
} // namespace style
} // namespace mbgl
