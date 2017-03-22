#pragma once

#include <mapbox/geojson.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/geojson.hpp>
#include <jni/jni.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
optional<GeoJSON> convertGeoJSON(const mbgl::android::Value& value, Error& error) {

    if(value.isNull() || !value.isString()) {
        error = { "no json data found" };
        return {};
    }

    return convertGeoJSON(value.toString(), error);
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
