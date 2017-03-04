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
Result<GeoJSON> convertGeoJSON(const mbgl::android::Value& value) {
    // Value should be a string wrapped in an object
    mbgl::android::Value jsonValue = value.get("data");
    if(value.isNull()) {
        return Error { "no json data found" };
    }
    return convertGeoJSON(value.get("data").toString());
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
