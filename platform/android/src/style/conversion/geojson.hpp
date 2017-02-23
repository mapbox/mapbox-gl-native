#pragma once

#include "../value.hpp"

#include <mapbox/geojson.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/geojson.hpp>
#include <mbgl/util/logging.hpp>
#include <jni/jni.hpp>

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

    return convertGeoJSON(value.get("data").toString(), error);
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
